
import subprocess as sp
from multiprocessing import cpu_count
import argparse
import sys
import os
import re

import parent_parser

RSCRIPT_PATH = 'rscripts/makeMs2.R'
THIS_PATH = ''
RSCRIPT = 'Rscript'
#PROG_DIR = os.path.realpath(__file__).replace('pyc', 'py').replace(THIS_PATH, '')

def getNThread():
    return cpu_count() / 2


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
    files = [x for x in os.listdir(spectraDirTemp) if re.search('.spectrum$', x)]

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

    fileSet = set()
    for i in ret:
        for j in i:
            fileSet.add(j)
    assert(len(fileSet) == len(files))

    return ret


def make_ms2_progress(spectraDir, sleepTime = 1, barWidth = 50):
    print('poop')


def make_ms2_parallel(nThread, spectraDir, progDir,
                      verbose = False, mzLab = 1, pSize = 'large', simpleSeq = 0,
                      files = None):
    if files is None:
        files = getFileLists(nThread, spectraDir)

    rscriptCommand = '{} {}/{}'.format(RSCRIPT, progDir, RSCRIPT_PATH)

    #spawn subprocecies
    procecies = list()
    for item in files:
        command = '{} -mzLab {} -pSize {} -simpleSeq {} {}'.format(rscriptCommand,
                                          mzLab, pSize, simpleSeq,
                                          ' '.join(item))
        print(command)
        procecies.append(sp.Popen([command], stdout = sp.PIPE, stderr = sp.PIPE,
                                  cwd = spectraDir,
                                  shell=True))

    #join threads and get output
    output = list()
    for i, proc in enumerate(procecies):
        pStatus = proc.wait()
        stdout, stderr = proc.communicate()
        output.append('Process {}\nstdout:\n{}\nstderr:\n{}'.format(i, stdout, stderr))
        if pStatus != 0:
            sys.stderr.write(output[i])

    if verbose:
        for o in output:
            print(o)


def main():
    parser = argparse.ArgumentParser('run_make_ms2',
                                     parents=[parent_parser.parent_parser],
                                     description='Run rscripts/makeMS2.R and manage parallelism.')

    parser.add_argument('input_files', nargs = '+',
                        help = '')

    args = parser.parse_args()

    progDir = os.path.dirname(sys.argv[0]).replace('python', '')

    # get wd
    wd = args.dir
    if wd is None:
        wd = os.getcwd()

    # get number of threads if not specified
    nThread = args.nThread
    if nThread is None:
        nThread = getNThread()

    make_ms2_parallel(nThread, wd, progDir, verbose = args.verbose,
                      mzLab=args.mzLab, pSize=args.pSize, simpleSeq=args.simpleSeq)


if __name__ == '__main__':
    main()