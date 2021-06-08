
import argparse

PARENT_PARSER = argparse.ArgumentParser(add_help=False)

PARENT_PARSER.add_argument('--inplace', default=False, action='store_true',
                           help='Should input_file be overwritten? Overrides ofname.')
PARENT_PARSER.add_argument('--fileExt', default='mzML',
                           help='File extension to add to MS file names. Default is "mzML".')
PARENT_PARSER.add_argument('-o', '--ofname', help='Name of output file.', default='')
PARENT_PARSER.add_argument('--verbose', default=False, action='store_true',
                           help='Produce verbose output?')

# arguments to customize mod residue
PARENT_PARSER.add_argument('--mod_residue', default='R',
                           help="Residue to put '*' on. Default is 'R'.")

