
import subprocess as sp
import argparse
import sys
import os
import re
import time

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
    files = ['{}/{}'.format(spectraDirTemp, x) for x in os.listdir(spectraDir) if re.search('.spectrum$', x)]

    #calculate number of files per thread
    nFiles = len(files)
    filesPerProcess = nFiles / nThread
    if nFiles % nThread != 0:
        filesPerProcess += 1

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


def make_ms2_parallel(nThread, spectraDir, verbose = False, mzLab = 1, pSize = 'large', simpleSeq = 0):
    files = getFileLists(nThread, spectraDir)

    progDir = os.path.realpath(__file__).replace('pyc', 'py').replace(THIS_PATH, RSCRIPT_PATH)
    rscriptCommand = '{} {}'.format(RSCRIPT, progDir)

    #spawn subprocecies
    procecies = list()
    for item in files:
        command = '{} {}'.format(rscriptCommand, ' '.join(item))
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