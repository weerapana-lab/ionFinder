
import subprocess as sp
from multiprocessing import cpu_count
import argparse
import sys
import os
import re

from modules import parent_parser

RSCRIPT_PATH = '../ionFinder/rscripts/makeMs2.R'
RSCRIPT = 'Rscript'

def getFileLists(nThread, spectraDir, inputFiles=None):
    """
    Get input file names and split into a list for each subprocess.

    Args:
        nThread: number of threads to be used
        spectraDir: dir containing files

    Returns:
        List of list containing file to run in each subprocess
    """

    #get list of files
    if inputFiles is None or len(inputFiles) == 0:
        spectraDirTemp = os.path.realpath(spectraDir)
        files = [x for x in os.listdir(spectraDirTemp) if re.search('.spectrum$', x)]
    else:
        files = [x for x in inputFiles if re.search('.spectrum$', x)]

    #calculate number of files per thread
    nFiles = len(files)
    filesPerProcess = nFiles // nThread
    if nFiles % nThread != 0:
        filesPerProcess += 1

    #split up files
    ret = list()
    i = 0
    while(i < nFiles):
        # get beginning and end indecies
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

    return ret, nFiles


def _make_ms2_parallel(nThread, spectraDir, make_ms2_r_path, round_to, inputFiles,
                      verbose=False, mzLab=1, simpleSeq=0):

    files, nFiles=getFileLists(nThread, spectraDir, inputFiles)

    if nFiles == 0:
        raise RuntimeError('No .spectra files found in {}'.format(spectraDir))

    rscriptCommand="{} {}".format(RSCRIPT, make_ms2_r_path)

    #spawn subprocesses
    procecies = list()
    for item in files:
        command = '{} -mzLab {} -simpleSeq {} -round {} {}'.format(rscriptCommand,
                                                                 mzLab, simpleSeq, round_to,
                                                                 ' '.join(item))
        if verbose:
            sys.stdout.write('{}\n'.format(command))

        procecies.append(sp.Popen([command], stdout=sp.PIPE, stderr=sp.PIPE,
                                  cwd=spectraDir,
                                  shell=True))

    #join processes and get output
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


def main(make_ms2_r_path):
    parser = argparse.ArgumentParser('run_make_ms2',
                                     parents=[parent_parser.PARENT_PARSER],
                                     description='Run rscripts/makeMS2.R and manage parallelism.')

    parser.add_argument('input_files', nargs='*',
                        help='')

    args = parser.parse_args()

    # get wd
    wd = args.dir
    if wd is None:
        wd = os.getcwd()

    # get number of threads if not specified
    nThread = args.nThread
    if nThread is None:
        nThread = cpu_count()

    _make_ms2_parallel(nThread, wd, make_ms2_r_path, args.round_to, args.input_files, verbose=args.verbose,
                       mzLab=args.mzLab, simpleSeq=args.simpleSeq)


if __name__ == '__main__':
    main(RSCRIPT_PATH)
