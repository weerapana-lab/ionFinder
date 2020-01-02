
import argparse

PARENT_PARSER = argparse.ArgumentParser(add_help=False)

PARENT_PARSER.add_argument('--mzLab', choices=[0, 1], default=1, type=int,
                           help='Choose whether to include MZ labels in spectrum. Default is 1.')

PARENT_PARSER.add_argument('--simpleSeq', choices=[0, 1], default=0, type=int,
                           help='Choose whether to write simplified peptide sequence on plot. '
                           '0 is the default.')

PARENT_PARSER.add_argument('--round_to', type=int, default=2,
                           help='Specify number of decimal places in ion labels in plot.'
                                  'Default is 2.')

PARENT_PARSER.add_argument('-t', '--nThread', type=int,
                           help='Specify number of subprocesses to spawn per job. '
                           'By default, nThread will be the number of logical cores on your system.')

PARENT_PARSER.add_argument('--dir', help='Directory to run program from.'
                                        ' If not specified, current working directory is used.')

PARENT_PARSER.add_argument('-v', '--verbose', action='store_true', default=False,
                           help='Should output be verbose?')
