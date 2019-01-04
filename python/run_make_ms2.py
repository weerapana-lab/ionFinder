
import sys
import os
import argparse
from multiprocessing import cpu_count

import make_ms2

def makePBS(mem, ppn, walltime, wd):
    inF = open('{}/make_ms2.pbs'.format(wd), 'r')

def spawnJobs():
    print('poop')

def main(argv):
    parser = argparse.ArgumentParser('run_make_ms2',
                                     description = 'Run rscripts/makeMS2.R and manage parallelism.')

    parser.add_argument('-mzLab', choices = [0, 1], default = 1,
                        help = 'Choose whether to include MZ labels in spectrum. Default is 1.')

    parser.add_argument('-pSize', choices = ['small', 'large'], default = 'large',
                        help = 'Specfy plot size. Default is large (12x4 in). Small is 8x3 in.')

    parser.add_argument('-simpleSeq',choices = [0, 1], default = 0,
                        help = 'Choose whether to write simplified peptide sequence on plot. 0 is the default.')

    parser.add_argument('-t', '--nThread', type = int,
                        help = 'Specify number of subprocesses to spawn per job. '
                               'By default, nThread will be the number of cores on your system. ')

    parser.add_argument('-n', '--nJob', type = int,
                        help = 'Specify number of jobs to split into.'
                               ' --jobMode pbs must specified for this argument to be used.')

    parser.add_argument('-j', '--jobMode', choices = ['pbs', 'inplace'], default = 'inplace',
                        help = 'Choose how to run jobs. pbs will create and submit <NJOB> PBS jobs. '
                               'inplace will run in the current terminal.')

    parser.add_argument('--dir', help = 'Directory to run program from.'
                                        'If not specified, current working directory is used')

    parser.add_argument('-v', '--verbose', action = 'store_true', default = False,
                        help = 'Should output be verbose? Will print all R output to terminal.')

    args = parser.parse_args()

    progDir = os.path.dirname(sys.argv[0])


    #get wd
    wd = args.dir
    if wd is None:
        wd = os.getcwd()

    #get number of threads if not specified
    nThread = args.nThread
    if nThread is None:
        nThread = cpu_count() / 2

    make_ms2.make_ms2_parallel(nThread, wd, args.verbose,
                               mzLab=args.mzLab, pSize=args.pSize, simpleSeq=args.simpleSeq)

    mods = make_ms2.FileModifications()
    mods.initilize(wd, '.spectrum', '.pdf')


if __name__ == '__main__':
    main(sys.argv)
