
import argparse

parent_parser = argparse.ArgumentParser(add_help=False)

parent_parser.add_argument('-mzLab', choices=[0, 1], default=1, type = int,
                    help='Choose whether to include MZ labels in spectrum. Default is 1.')

parent_parser.add_argument('-pSize', choices=['small', 'large'], default='large',
                    help='Specfy plot size. Default is large (12x4 in). Small is 8x3 in.')

parent_parser.add_argument('-simpleSeq', choices=[0, 1], default=0, type = int,
                    help='Choose whether to write simplified peptide sequence on plot. '
                         '0 is the default.')

parent_parser.add_argument('-t', '--nThread', type=int,
                    help='Specify number of subprocesses to spawn per job. '
                         'By default, nThread will be the number of cores on your system. ')

parent_parser.add_argument('--dir', help = 'Directory to run program from.'
                                        'If not specified, current working directory is used')

parent_parser.add_argument('-v', '--verbose', action = 'store_true', default = False,
                        help = 'Should output be verbose? Will print all R output to terminal.')
