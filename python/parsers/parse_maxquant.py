
import sys
import os
import re
import argparse
import pandas as pd

from modules.parent_parser import PARENT_PARSER
from modules.tsv_constants import *
from modules.maxquant_constants import *
from modules.molecular_formula import MolecularFormula
from modules import atom_table


def get_unique_modifications(modified_sequences):
    '''
    Get a set of unique peptide modifications and the residues they occur on.

    Parameters
    ----------
    modified_sequences: Iterable
        Iterable object with all modifications observed in data set.

    Returns
    -------
    mods_set: list
        List of tuples (modification, residue)
    '''

    ret = set()
    for seq in modified_sequences:
        for match in re.findall(r'([A-Z_])\((.*?)(\(.*?\))?\)', seq):
            ret.add((('N-TERM' if match[0] == '_' else match[0]), match[1].strip().lower()))
    return list(ret)


# def main():
# 
#     parser = argparse.ArgumentParser(prog='parse_maxquant',
#                                      parents=[PARENT_PARSER],
#                                      description='Convert MaxQuant output to proper input for ionFinder tsv input.',
#                                      epilog="parse_maxquant was written by Aaron Maurais.\n"
#                                             "Email questions or bugs to aaron.maurais@bc.edu")
# 
#     parser.add_argument('-f', '--fixedMod', default='C:carbamidomethyl',
#                         help='Specify fixed modification(s) if there are multiple modifications, '
#                               'they should be comma separated. default is "C:carbamidomethyl"')
# 
#     parser.add_argument('input_file', help='Name of file to parse. Should be a MaxQuant evidence.txt file.')
# 
#     args = parser.parse_args()
# 
#     if args.inplace:
#         ofname = args.input_file
#     else:
#         if args.ofname == '':
#             s = os.path.splitext(os.path.basename(args.input_file))
#             ofname = '{}_parsed.tsv'.format(s[0])
#         else: ofname = args.ofname

class args:
    input_file = '/Volumes/Data/msData/ionFinder/parse_maxquant_files/evidence.tsv'
    fixedMod = 'C:carbamidomethyl'

class parser:
    prog = 'test'

# read and format properly
sys.stdout.write('\n{}\n\nReading {}...'.format(parser.prog, args.input_file))
dat = pd.read_csv(args.input_file, sep='\t')
dat.columns = [c.lower().replace(' ', '_').replace('/', '') for c in dat.columns]
sys.stdout.write(' Done!\n')

# parse fixed modifications
fixed_modifications = list()
for mod in re.split('\s?,\s?', args.fixedMod):
    match = re.search(r'^([A-Z]):(.+)$', mod.strip())
    if match is None:
        raise RuntimeError('Could not parse fixed modification: {}'.format(mod))
    fixed_modifications.append((match.group(1), match.group(2)))
fixed_modifications = set(fixed_modifications)

# Check that modifications are valid
variable_modifications = get_unique_modifications(dat[dat[MODIFICATIONS] != 'Unmodified'][MODIFIED_SEQUENCE].to_list())
    

# if __name__ == '__main__':
#     main()


