
import subprocess as sp
import argparse
import sys
import os
import re
import time
import threading

RSCRIPT_PATH = 'rscripts/makeMs2.R'
THIS_PATH = 'python/make_ms2.py'
RSCRIPT = 'Rscript'

def getFileLists(nThread, spectraDir):
    """
    Get input file names and split into a list for each subprocess.

    Args:
        nThread: number of threads to be used
        spectraDir: dir containing files

    Returns:
        List of list containing file to run in each subprocess
    """

    #get list of files
    spectraDirTemp = os.path.realpath(spectraDir)
    files = ['{}/{}'.format(spectraDirTemp, x)
             for x in os.listdir(spectraDirTemp) if
             re.search('.spectrum$', x)]

    #calculate number of files per thread
    nFiles = len(files)
    filesPerProcess = nFiles / nThread
    if nFiles % nThread != 0:
        filesPerProcess += 1

    #for file in files:
    #    if file.find('ror_pad_TNLFSREEVTSYQr_9314_2.pdf'):
    #        print('Found!')

    #split up files
    ret = list()
    i = 0
    while(i < nFiles):
        # get begining and end indecies
        begNum = i
        endNum = begNum + filesPerProcess
        if endNum > nFiles:
            endNum = nFiles

        ret.append(files[begNum:endNum])
        i += filesPerProcess

    fileSet = set()
    for i in ret:
        for j in i:
            fileSet.add(j)
    assert(len(fileSet) == len(files))

    print("\nfiles:")
    for file in files:
        print(file)

    print("\nFile set:")
    for file in fileSet:
        print(file)

    print("\nret")
    for l in ret:
        for file in l:
            print(file)
    print('\n')

    return ret


def progressBar(progress, barWidth = 50):

    sys.stdout.write('[')
    pos = int(barWidth * progress)

    for i in xrange(barWidth):
        if i < pos:
            sys.stdout.write('=')
        elif i == pos:
            sys.stdout.write('>')
        else: sys.stdout.write(' ')

    sys.stdout.write('] {}%\r'.format(int(progress * 100)))
    sys.stdout.flush()


def testProgBar():
    for i in [0,5,10,20,50,75,100]:
        time.sleep(0.5)
        progressBar(float(i) / float(100))

    sys.stdout.write('\n')


class FileModifications():
    def __init__(self):
        self.inputDir = ''
        self.modTimes = dict()

    def initilize(self, inputDir, sourceExt, outExt):
        inputDirTemp = os.path.realpath(inputDir)
        sourceFiles = ['{}/{}'.format(inputDirTemp, x.replace(sourceExt, outExt))
        #sourceFiles = ['{}/{}'.format(inputDirTemp, x)
                       for x in os.listdir(inputDirTemp)
                       if re.search('{}$'.format(sourceExt), x)]

        for file in sourceFiles:
            try:
                time = os.path.getmtime(file)
            except OSError:
                self.modTimes[file] = 0
                print(file)
                continue
            self.modTimes[file] = time
        print('poop')


def make_ms2_progress(spectraDir, sleepTime = 1, barWidth = 50):
    print('poop')


def make_ms2_parallel(nThread, spectraDir, verbose = False, mzLab = 1, pSize = 'large', simpleSeq = 0):
    files = getFileLists(nThread, spectraDir)

    progDir = os.path.realpath(__file__).replace('pyc', 'py').replace(THIS_PATH, RSCRIPT_PATH)
    rscriptCommand = '{} {}'.format(RSCRIPT, progDir)

    #spawn subprocecies
    procecies = list()
    for item in files:
        command = '{} -mzLab {} -pSize {} -simpleSeq {} {}'.format(rscriptCommand,
                                          mzLab, pSize, simpleSeq,
                                          ' '.join(item))
        print(command)
        procecies.append(sp.Popen(command, stdout = sp.PIPE, stderr = sp.PIPE,
                                  cwd = spectraDir,
                                  shell=True))

    #join threads and get output
    output = list()
    for i, proc in enumerate(procecies):
        proc.wait()
        stdout, stderr = proc.communicate()
        output.append('Process {}\nstdout:\n{}\nstderr:\n{}'.format(i, stdout, stderr))

    if verbose:
        for o in output:
            print(o)


def main(argv):
    parser = argparse.ArgumentParser('run_make_ms2',
                                     description='Run rscripts/makeMS2.R and manage parallelism.')

    parser.add_argument('-mzLab', choices=[0, 1], default=1,
                        help='Choose whether to include MZ labels in spectrum. Default is 1.')

    parser.add_argument('-pSize', choices=['small', 'large'], default='large',
                        help='Specfy plot size. Default is large (12x4 in). Small is 8x3 in.')

    parser.add_argument('-simpleSeq', choices=[0, 1], default=0,
                        help='Choose whether to write simplified peptide sequence on plot. 0 is the default.')

    parser.add_argument('-t', '--nThread', type=int,
                        help='Specify number of subprocesses to spawn per job. '
                             'By default, nThread will be the number of cores on your system. ')

    args = parser.parse_args()


if __name__ == '__main__':
    main(sys.argv)