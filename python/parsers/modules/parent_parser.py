
import argparse

PARENT_PARSER = argparse.ArgumentParser(add_help=False)

PARENT_PARSER.add_argument('--inplace', default=False, action='store_true',
                           help='Should input_file be overwritten? Overrides ofname.')
PARENT_PARSER.add_argument('-o', '--ofname', help='Name of output file.', default='')
PARENT_PARSER.add_argument('-f', '--calc_formula', default=0, choices=[0, 1], type=int,
                           help='Should molecular formula of peptides be calculated? 0 is the default.')

# arguments to customize mod residue
PARENT_PARSER.add_argument('--mod_residue', default='R',
                           help="Residue to put '*' on")
PARENT_PARSER.add_argument('--mod_mass', default=0.98, type=float,
                           help='Mass of modification.')

PARENT_PARSER.add_argument('--verbose', default=False, action='store_true',
                           help='Produce verbose output?')
