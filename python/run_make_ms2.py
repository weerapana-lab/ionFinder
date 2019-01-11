
import sys
import os
import argparse
import subprocess
from math import ceil

import make_ms2
import parent_parser

PYTHON_PBS_VERSION = 'python/2.7.10'
R_PBS_VERSION = 'R/3.3.0'
PBS_MODULE_LOAD_COMMAND = 'module load'
PBS_PYTHON_COMMAND = 'python'
MAKE_MS2_PY_PATH = 'python/make_ms2.py'

def makePBS(mem, ppn, walltime, nThread, makeMs2_args, wd, progDir):
    pbsName = '{}/make_ms2.pbs'.format(wd)
    outF = open(pbsName, 'w')
    #outF = sys.stdout

    outF.write("#!/bin/tcsh\n")
    outF.write('#PBS -l mem={}gb,nodes=1:ppn={},walltime={}\n\n'.format(mem, ppn, walltime))
    outF.write('{} {}\n'.format(PBS_MODULE_LOAD_COMMAND, R_PBS_VERSION))
    outF.write('{} {}\n\n'.format(PBS_MODULE_LOAD_COMMAND, PYTHON_PBS_VERSION))
    outF.write('cd {}\n'.format(wd))
    outF.write('{} {}/{} -t {} {}\n'.format(PBS_PYTHON_COMMAND, progDir, MAKE_MS2_PY_PATH,
                                       nThread, makeMs2_args))

    return pbsName


def getPlurality(num):
    if num > 1:
        return 's'
    else: return ''


def main():
    parser = argparse.ArgumentParser(prog = 'run_make_ms2', parents=[parent_parser.parent_parser],
                                     description = 'Run rscripts/makeMS2.R and manage parallelism. '
                                                   'Program will search current working directory for .spectrum files'
                                                   ' for input.')

    parser.add_argument('-g', '--go', action = 'store_true', default = False,
                        help = 'Should jobs be submitted? If this flag is not supplied, program will be a dry run. '
                               'Required system resources will be printed but jobs will not be submitted.')

    #parser.add_argument('--dry_run', choices = [0, 1], type = int, default = 0,
    #                    help = 'Prints summary of system resources needed, but does not submit jobs.'
    #                           'Overides -g command. Default is 0.')

    parser.add_argument('-j', '--jobMode', choices = ['pbs', 'inplace'], default = 'pbs',
                        help = 'Choose how to run jobs. pbs will create and submit <NJOB> PBS jobs. '
                               'inplace will run in the current terminal.')

    parser.add_argument('-n', '--nJob', type=int, default = 1,
                        help='Specify number of jobs to split into.'
                             ' --jobMode pbs must specified for this argument to be used.')

    parser.add_argument('-m', '--mem', default=1, type = int,
                        help = 'Amount of memory to allocate per PBS job in gb. Default is 1.')

    parser.add_argument('-p', '--ppn', default=4, type=int,
                        help='Number of processors to allocate per PBS job. Default is 4.')

    parser.add_argument('-w', '--walltime', default='12:00:00',
                        help = 'Walltime per job in the format hh:mm:ss. Default is 12:00:00.')

    args = parser.parse_args()

    progDir = os.path.dirname(sys.argv[0]).replace('python', '')
    ppn = args.ppn

    #get wd
    wd = args.dir
    if wd is None:
        wd = os.getcwd()

    spectraFiles, nFiles = make_ms2.getFileLists(args.nJob, wd)

    sys.stdout.write('{} v0.1\n'.format(parser.prog))

    #print summary of resources needed
    sys.stdout.write('\nRequested {} job{} with {} processor{} each...\n'.format(args.nJob, getPlurality(args.nJob),
                                                                              args.ppn, getPlurality(args.ppn)))
    sys.stdout.write('\t{} spectrum file{} found\n'.format(nFiles, getPlurality(nFiles)))
    if nFiles == 0:
        sys.stderr.write('No .spectra files found in {}\nExiting...\n'.format(wd))
        exit()

    filesPerJob = max([len(x) for x in spectraFiles])
    sys.stdout.write('\t{} job{} needed\n'.format(len(spectraFiles), getPlurality(len(spectraFiles))))
    sys.stdout.write('\t{} file{} per job\n'.format(filesPerJob, getPlurality(filesPerJob)))

    if filesPerJob < ppn:
        ppn = filesPerJob
    sys.stdout.write('\t{} processor{} per job\n'.format(ppn, getPlurality(ppn)))

    # get number of threads if not specified
    nThread = args.nThread
    if nThread is None:
        nThread = ppn * 2
    sys.stdout.write('\t{} thread{}\n'.format(nThread, getPlurality(nThread)))

    filesPerThread = int(ceil(float(filesPerJob) / float(nThread)))
    sys.stdout.write('\t{} file{} per thread\n\n'.format(filesPerThread, getPlurality(filesPerThread)))

    #submit jobs
    for i, item in enumerate(spectraFiles):
        makeMs2_args = '-v -mzLab {} ' \
                       '-pSize {} ' \
                       '-simpleSeq {} ' \
                       '-round {} {}'.format(args.mzLab,
                                             args.pSize,
                                             args.simpleSeq,
                                             args.round,
                                             ' '.join(item))

        pbsName = makePBS(args.mem, args.ppn, args.walltime, nThread, makeMs2_args, wd, progDir)
        command = 'qsub {}'.format(pbsName)
        if args.verbose:
            sys.stdout.write('{}\n'.format(command))
        if args.go:
            proc = subprocess.Popen([command], cwd=wd, shell=True)
            proc.wait()

if __name__ == '__main__':
    main()
