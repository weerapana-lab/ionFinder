
import sys
import os
import re
import argparse
import pandas as pd
import numpy as np

if sys.version_info[0] < 3:
    from StringIO import StringIO
else:
    from io import StringIO

from modules.tsv_constants import *
from modules.molecular_formula import MolecularFormula
from modules import atom_table

SEARCH_ENGINES = {'Proteome Discover':{SPECTRUM_NAME: r',scan_([0-9]+),type',
                      MS_MS_SAMPLE_NAME: r'^Experiment [\w\-: ]+ from ([\w\- ]+)'},
                  'Mascot':{SPECTRUM_NAME: r'\d+-\d+_(\d+)$',
                      MS_MS_SAMPLE_NAME: r'^(?:[\w \-]+:\s*)?([\w\- ]+)$'}}

MODIFICATION_REGEX = r'^([nc]-term|[a-zA-Z])([\d]*): ([\w\-\> ]+)(?: \([A-Z]+\))? \(([\-\+]?\d+\.?\d*)\)$'
UNIPROT_ID_RE = r'^(sp|tr)\|([A-Za-z0-9-]+)\|([A-Za-z0-9]+)(?:_\w+ (.+) OS=)?'

def parse_spectrum_report(fname):
    '''
    Parse scaffold spectrun report to a tsv file readable by pandas
    and return a processed pd.DataFrame.

    Parameters
    ----------
    fname: str
        Path to file to process.
    '''

    with open(fname, 'r') as inF:
        s = inF.read()

    i = s.find(RAW_SPECTRUM_NAME)
    if i == -1:
        raise RuntimeError('Could not find "{}" column!'.format(RAW_SPECTRUM_NAME))
    i = s.rfind('\n', 0, i)
    begin = i if i != -1 else 0
    s = s[begin:].strip()

    i = s.find(END_OF_FILE)
    end = i if i != -1 else len(s)
    s = s[:end].strip()

    with StringIO(s) as inF:
        ret = pd.read_csv(inF, sep = '\t')

    ret.columns = [x.replace(' ', '_').lower() for x in ret.columns.tolist()]

    return ret

def detect_search_engine(dat):
    '''
    Detect the search engnie used to in the scaffold file.

    Each regex in SEARCH_ENGINES are tested on the appropiate columns
    in dat. The key of the first sucessful match is returned.

    Parameters
    ----------
    dat: pd.DataFrame
        Initalized DataFrame from input_file.
    '''

    sys.stdout.write('\nAtempting to find the appropiate regex for search engine...\n')
    ret_key = str()
    for k1, v1 in SEARCH_ENGINES.items():
        sys.stdout.write('\tTrying to match regex for {}...'.format(k1))
        try:
            for k2, v2 in v1.items():
                dat[k2].apply(lambda x:re.search(v2, x).group(1))
            sys.stdout.write(' Matched!\n')
            ret_key = k1
        except AttributeError as e:
            sys.stdout.write(' No match\n')
            continue
    if not ret_key:
        raise RuntimeError('Can not parse input_file!')
    return ret_key



class AminoAcid(object):
    __slots__ = ['aa', 'mod']

    def __init__(self, aa, mod):
        self.aa = aa
        self.mod = mod

    def __str__(self):
        if self.mod == 0:
            return self.aa
        else:
            return '{}({:+})'.format(self.aa, self.mod)


def strToAminoAcids(seq):
    ret = list()

    # add n term
    ret.append(AminoAcid('', float()))

    for c in seq:
        ret.append(AminoAcid(c, float()))

    # add c term
    ret.append(AminoAcid('', float()))

    return ret

def get_unique_modifications(mods):
    '''
    Get a set of unique peptide modifications and the residues they occur on.

    Parameters
    ----------
    mods: Iterable
        Iterable object with all modifications observed in data set.

    Returns
    -------
    mods_set: list
        List of tuples (modification, residue)
    '''

    ret = set()
    for line in mods:
        if line is np.nan:
            continue

        for x in list(map(str.strip, line.split(','))):
            mod = re.search(MODIFICATION_REGEX, x)
            if mod:
                ret.add((mod.group(3).lower(), mod.group(1).upper()))
            else:
                ret.add(None)
                sys.stderr.write('ERROR: Could not parse modification.\n\t{}\n'.format(x))

    return list(ret)


def check_modifications(fixed_list, variable_list, verbose=False):

    def print_found_mods(lst, name):
        sys.stdout.write('{} modifications:\n'.format(name))
        for mod in lst:
            known_mod = mod[0] in atom_table.MODIFICATIONS.keys()
            if known_mod:
                known_residue = mod[1] in atom_table.MODIFICATIONS[mod[0]].keys()
            else:
                known_residue = False
            sys.stdout.write('\t{}: {} on {}'.format('Found' if known_mod and known_residue else 'NOT FOUND',
                                                   mod[0], mod[1]))
            if not known_mod:
                sys.stdout.write(' -> Unknown modification\n')
            elif not known_residue and known_mod:
                sys.stdout.write(' -> Known modification, but not for residue: {}\n'.format(mod[1]))
            else:
                sys.stdout.write('\n')

    var_mod_check = get_unique_modifications(variable_list)
    fixed_mod_check = get_unique_modifications(fixed_list)

    all_good = True
    for mod in var_mod_check + fixed_mod_check:
        if mod is not None and not (mod[0] in atom_table.MODIFICATIONS.keys() and mod[1] in atom_table.MODIFICATIONS[mod[0]]):
            all_good = False
            break

    if not all_good or verbose:
        sys.stdout.write('\n')
        print_found_mods(var_mod_check, 'variable')
        print_found_mods(fixed_mod_check, 'fixed')

    return all_good


def extractModifications(seq, mods, calc_formula=False):
    '''
    Extract modifications from modifications description and populate
    to a list of AminoAcid objects.

    Parameters
    ---------
    seq : list
        List of AminoAcid objects
    mod: str
        Description of modifications on peptide

    Returns
    -------
    formula: MolecularFormula
        Combined formula of modifications.
    '''

    ret = MolecularFormula()

    if mods is np.nan:
        return ret

    # extract modified residue, number, and mod mass from mod
    # and put into a list with an element for each mod
    for x in list(map(str.strip, mods.split(','))):
        mod = re.search(MODIFICATION_REGEX, x)

        try:
            float(mod.group(4))
        except (ValueError, AttributeError):
            sys.stderr('ERROR: Could not parse modifications from string: {}\n'.format(x))
            return MolecularFormula()

        if calc_formula:
            ret.add_mod(mod.group(3).lower(), mod.group(1).upper())

        modMass = float(mod.group(4))
        if mod.group(1) == 'n-term':
            seq[0].mod += modMass
        elif mod.group(1) == 'c-term':
            mod[-1].mod += modMass
        else:
            residue = mod.group(1).upper()
            num = int(mod.group(2))
            # sanity check
            if seq[num].aa.upper() != residue:
                raise RuntimeError('mods do not correspond to sequence.')

            # add modMass to seq[num].mods
            # this will not cause an off by 1 error because seq[0] is the n terminal modification
            seq[num].mod += modMass

    return ret


def main():

    parser = argparse.ArgumentParser(prog='parse_scaffold',
                                     description='Convert Scaffold output to proper input for ionFinder tsv input.',
                                     epilog="parse_scaffold was written by Aaron Maurais.\n"
                                            "Email questions or bugs to aaron.maurais@bc.edu")

    parser.add_argument('input_file', help = 'Name of file to parse. Should be a Scaffold spectrum report.')

    parser.add_argument('--inplace', default=False, action='store_true',
                        help='Should input_file be overwritten? Overrides ofname.')
    parser.add_argument('-o', '--ofname', help='Name of output file.', default = '')
    parser.add_argument('-f', '--calc_formula', default=0, choices=[0,1], type=int,
                        help='Should molecular formula of peptides be calculated? 0 is the default.')

    #arguments to customize mod residue
    parser.add_argument('--mod_residue', default='R',
                        help = "Residue to put '*' on")
    parser.add_argument('--mod_mass', default = 0.98, type=float,
                        help='Mass of modification.')

    parser.add_argument('--verbose', default=False, action='store_true',
                        help='Produce verbose output?')

    args = parser.parse_args()

    if args.inplace:
        ofname = args.input_file
    else:
        if args.ofname == '':
            s = os.path.splitext(os.path.basename(args.input_file))
            ofname = '{}_parsed.tsv'.format(s[0])
        else: ofname = args.ofname

    # read and format properly
    sys.stdout.write('\nparse_scaffold\n\nReading {}...'.format(args.input_file))
    dat = parse_spectrum_report(args.input_file)
    sys.stdout.write(' Done!\n')

    # Check that all modifications are valid
    if args.calc_formula:
        sys.stdout.write('Iterating through modifications to make sure their composition is known...')
        if not check_modifications(dat[FIXED_MODIFICATIONS].to_list(),
                                   dat[VARIABLE_MODIFICATIONS].to_list(),
                                   verbose = args.verbose):
            return -1
        sys.stdout.write('Success!\n')

    # extract scan column
    engine = detect_search_engine(dat)
    dat[SCAN_NUM] = dat[SPECTRUM_NAME].apply(lambda x: re.search(SEARCH_ENGINES[engine][SPECTRUM_NAME], x).group(1))

    # extract precursorFile column
    dat[PRECURSOR_FILE] = dat[MS_MS_SAMPLE_NAME].apply(lambda x: re.search(SEARCH_ENGINES[engine][MS_MS_SAMPLE_NAME], x).group(1) + '.ms2')

    # parse protein id and name
    matches = [re.search(UNIPROT_ID_RE, s) for s in dat[PROTEIN_NAME].values.tolist()]
    good_matches = [m for m in matches if bool(m)]
    if len(dat.index) != len([m for m in matches if m]):
        sys.stderr.write('WARN: unable to parse acession for {} peptides!\n'.format(len(dat.index) - len(good_matches)))
        if args.verbose:
            sys.stderr.write('Bad acession(s):\n')
            bad_acessions = set(dat[[not bool(m) for m in matches]][PROTEIN_NAME].values.tolist())
            for acession in bad_acessions:
                sys.stderr.write('\t{}\n'.format(acession))
        dat = dat[[bool(x) for x in matches]]
        dat.reset_index()

    dat[PARENT_PROTEIN] = pd.Series(list(map(lambda x: x.group(3), good_matches)))
    dat[PARENT_ID] = pd.Series(list(map(lambda x: x.group(2), good_matches)))
    dat[PARENT_DESCRIPTION] = pd.Series(list(map(lambda x: x.group(4), good_matches)))

    # add static modifications
    seq_list = dat[PEPTIDE_SEQUENCE].apply(str.upper).apply(strToAminoAcids).tolist()
    formulas = [MolecularFormula() if not args.calc_formula else MolecularFormula(x.upper()) for x in dat[PEPTIDE_SEQUENCE]]
    for i, value in enumerate(dat[FIXED_MODIFICATIONS]):
        formulas[i] += extractModifications(seq_list[i], value, calc_formula=args.calc_formula)

    # add dynamic modifications
    for i, value in enumerate(dat[VARIABLE_MODIFICATIONS]):
        formulas[i] += extractModifications(seq_list[i], value, calc_formula=args.calc_formula)

    # add formula column
    dat[FORMULA] = pd.Series([str(x) for x in formulas])

    # get seq as string and change R(+0.98) to R*
    seq_str_list = list()
    for seq in seq_list:
        s = str()
        for a in seq:
            s += str(a)
        _mod_temp = '{0}({1:+})'.format(args.mod_residue, args.mod_mass)
        s = s.replace(_mod_temp, '{}*'.format(args.mod_residue))
        seq_str_list.append(s)

    dat[SEQUENCE] = pd.Series(seq_str_list)
    dat[FULL_SEQUENCE] = pd.Series(seq_str_list)

    keep_cols = [EXPERIMENT_NAME,
                 PRECURSOR_FILE,
                 PARENT_ID,
                 PARENT_PROTEIN,
                 PARENT_DESCRIPTION,
                 FULL_SEQUENCE,
                 SEQUENCE,
                 FORMULA,
                 SCAN_NUM,
                 EXCLUSIVE,
                 OBSERVED_M_Z,
                 SPECTRUM_CHARGE]

    dat = dat[keep_cols]

    dat.rename({EXPERIMENT_NAME: SAMPLE_NAME,
                EXCLUSIVE: UNIQUE,
                OBSERVED_M_Z: PRECURSOR_MZ,
                SPECTRUM_CHARGE: CHARGE},
               axis='columns', inplace=True)

    sys.stdout.write('Writing {}...'.format(ofname))
    dat.to_csv(ofname, sep='\t', index=False)
    sys.stdout.write(' Done!\n')


if __name__ == '__main__':
    main()

