
import sys
import os
import argparse
import subprocess

import make_ms2
import parent_parser

PYTHON_PBS_VERSION = 'python/2.7.10'
R_PBS_VERSION = 'R/3.3.0'
PBS_MODULE_LOAD_COMMAND = 'module load'
PBS_PYTHON_COMMAND = 'python'
MAKE_MS2_PY_PATH = 'python/make_ms2.py'

def makePBS(mem, ppn, walltime, nThread, makeMs2_args, wd):
    pbsName = '{}/make_ms2.pbs'.format(wd)
    #outF = open(pbsName, 'w')
    outF = sys.stdout

    outF.write("#!/bin/tcsh\n")
    outF.write('#PBS -l mem={}gb,nodes=1:ppn={},walltime={}\n\n'.format(mem, ppn, walltime))
    outF.write('{} {}\n'.format(PBS_MODULE_LOAD_COMMAND, R_PBS_VERSION))
    outF.write('{} {}\n\n'.format(PBS_MODULE_LOAD_COMMAND, PYTHON_PBS_VERSION))
    outF.write('cd {}\n'.format(wd))
    outF.write('{} {}/{} -t {} {}\n'.format(PBS_PYTHON_COMMAND, make_ms2.PROG_DIR, MAKE_MS2_PY_PATH,
                                       nThread, makeMs2_args))

    return pbsName


def main():
    parser = argparse.ArgumentParser('run_make_ms2',
                                     parents=[parent_parser.parent_parser],
                                     description = 'Run rscripts/makeMS2.R and manage parallelism.')

    parser.add_argument('-j', '--jobMode', choices = ['pbs', 'inplace'], default = 'pbs',
                        help = 'Choose how to run jobs. pbs will create and submit <NJOB> PBS jobs. '
                               'inplace will run in the current terminal.')

    parser.add_argument('-n', '--nJob', type=int,
                        help='Specify number of jobs to split into.'
                             ' --jobMode pbs must specified for this argument to be used.')

    parser.add_argument('-m', '--mem', default=4, type = int,
                        help = 'Amount of memory to allocate per PBS job in gb. Default is 4.')

    parser.add_argument('-p', '--ppn', default=8, type=int,
                        help='Number of processors to allocate per PBS job in gb. Default is 8.')

    parser.add_argument('-w', '--walltime', default='12:00:00',
                        help = 'Walltime per job in the format hh:mm:ss. Default is 12:00:00.')

    args = parser.parse_args()

    progDir = os.path.dirname(sys.argv[0])

    #get wd
    wd = args.dir
    if wd is None:
        wd = os.getcwd()

    #get number of threads if not specified
    nThread = args.nThread
    if nThread is None:
        nThread = make_ms2.getNThread()

    #make_ms2.make_ms2_parallel(nThread, wd, args.verbose,
    #                           mzLab=args.mzLab, pSize=args.pSize, simpleSeq=args.simpleSeq)

    #mods = make_ms2.FileModifications()
    #mods.initilize(wd, '.spectrum', '.pdf')
    spectraFiles = make_ms2.getFileLists(nThread, wd)
    for i, item in enumerate(spectraFiles):
        makeMs2_args = '-mzLab {} -pSize {} -simpleSeq {} {}'.format(args.mzLab,
                                                                     args.pSize,
                                                                     args.simpleSeq,
                                                                     ' '.join(item))
        pbsName = makePBS(args.mem, args.ppn, args.walltime, nThread, makeMs2_args, wd)
        command = 'qsub {}'.format(pbsName)
        print(command)
        #proc = subprocess.Popen(['tail -n 1 {}'.format(pbsName)], cwd = wd, shell = True)
        #proc = subprocess.Popen([command], cwd=wd, shell=True)
        #proc.wait()


if __name__ == '__main__':
    main()
