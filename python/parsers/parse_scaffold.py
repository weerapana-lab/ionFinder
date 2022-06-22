
import sys
import os
import re
import argparse
from collections import Counter
import pandas as pd
from numpy import nan

if sys.version_info[0] < 3:
    from StringIO import StringIO
else:
    from io import StringIO

from modules.parent_parser import PARENT_PARSER
from modules.tsv_constants import *
from modules.scaffold_constants import *
from modules.molecular_formula import MolecularFormula
from modules import atom_table
from modules import utils

SEARCH_ENGINES = {'Proteome Discoverer':{'scan': (SPECTRUM_NAME, re.compile(r',scan_([0-9]+),type')),
                                         'msms': (MS_MS_SAMPLE_NAME, re.compile(r'^Experiment [\w\-: ]+ from ([\w\- ]+)'))},
                  'MaxQuant':{'scan': (SPECTRUM_NAME, re.compile(r'\d+-\d+_(\d+)$')),
                              'msms': (MS_MS_SAMPLE_NAME, re.compile(r'^(?:[\w \+\-]+:\s*)?([\w\+\- ]+)$'))},
                  'Mascot format 1': {'scan': (SPECTRUM_NAME, re.compile(r'Scan (\w+)')),
                               'msms': (SPECTRUM_NAME, re.compile(r'Scan \w+.*\\([A-Za-z0-9_\-\+ ]+\.\w+)'))},
                  'Mascot format 2': {'scan': (SPECTRUM_NAME, re.compile(r'^.+-\d+-\d+_(\d+)$')),
                               'msms': (SPECTRUM_NAME, re.compile(r'^(.+)-\d+-\d+_\d+$'))}}


MODIFICATION_REGEX = re.compile(r'^([nc]-term|[a-zA-Z])([\d]*): ([\w\-\> ]+)(?: \([A-Z]+\))? \(([\-\+]?\d+\.?\d*)\)$')
DESCRIPTION_REGEX = re.compile(r'^(?:\w+\|[A-Za-z0-9\-]+\|[A-Za-z0-9_]+\s)?([\w\-/, \\;.\[\]\{\}()]+)(?:OS=.*)$')
ACCESSION_REGEX = re.compile(r'\w+\|([A-Za-z0-9-]+)\|([A-Za-z0-9]+)_')
UNIPROT_ID_REGEX = re.compile(r'[OPQ][0-9][A-Z0-9]{3}[0-9]|[A-NR-Z][0-9]([A-Z][A-Z0-9]{2}[0-9]){1,2}')

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

    spectrum_start_re = '{}|{}'.format(RAW_SPECTRUM_NAME, RAW_SPECTRUM_NAME.lower().replace(' ', '_'))
    i = re.search(spectrum_start_re, s)
    i = -1 if i is None else i.start()
    if i == -1:
        raise RuntimeError('Could not find "{}" column!'.format(RAW_SPECTRUM_NAME))
    i = s.rfind('\n', 0, i)
    begin = i if i != -1 else 0
    s = s[begin:].strip()

    i = s.find(END_OF_FILE)
    end = i if i != -1 else len(s)
    s = s[:end].strip()

    with StringIO(s) as inF:
        ret = pd.read_csv(inF, sep= '\t', low_memory=False)

    ret.columns = [x.replace(' ', '_').lower() for x in ret.columns.tolist()]

    return ret


def detect_search_engine(dat):
    '''
    Detect the search engnie used to in the scaffold file.

    Each regex in SEARCH_ENGINES are tested on the appropriate columns
    in dat. The key of the first successful match is returned.

    Parameters
    ----------
    dat: pd.DataFrame
        Initialized DataFrame from input_file.

    Returns
    -------
    rey_key: str
        The key for the appropite search engine in SEARCH_ENGINES
    '''

    sys.stdout.write('\nAtempting to find the appropriate regex for search engine...\n')
    ret_key = str()
    for engine, values in SEARCH_ENGINES.items():
        sys.stdout.write('\tTrying to match regex for {}...'.format(engine))
        try:
            for value in values.values():
                dat[value[0]].apply(lambda x: value[1].search(x).group(1))
            sys.stdout.write(' Matched!\n')
            ret_key = engine
        except AttributeError as e:
            sys.stdout.write(' No match\n')
            continue
    if not ret_key:
        raise RuntimeError('Can not parse input_file!')
    return ret_key


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
        if line is nan:
            continue

        for x in list(map(str.strip, line.split(','))):
            mod = MODIFICATION_REGEX.search(x)
            if mod:
                ret.add((mod.group(3).lower(), mod.group(1).upper()))
            else:
                ret.add(None)
                sys.stderr.write('ERROR: Could not parse modification.\n\t{}\n'.format(x))

    return list(ret)


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

    if mods is nan:
        return ret

    # extract modified residue, number, and mod mass from mod
    # and put into a list with an element for each mod
    for x in list(map(str.strip, mods.split(','))):
        mod = MODIFICATION_REGEX.search(x)

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
                                     parents=[PARENT_PARSER],
                                     description='Convert Scaffold output to proper input for ionFinder tsv input.',
                                     epilog="parse_scaffold was written by Aaron Maurais.\n"
                                            "Email questions or bugs to aaron.maurais@bc.edu")

    parser.add_argument('-i', '--bad_id_filter', default=1, choices=[0, 1], type=int,
                        help='Should entries with unparsable protein acessions be removed? 1 is the default.')

    parser.add_argument('-f', '--calc_formula', default=0, choices=[0, 1], type=int,
                        help='Should molecular formula of peptides be calculated? 0 is the default.')
    parser.add_argument('input_file', help='Name of file to parse. Should be a Scaffold spectrum report.')

    args = parser.parse_args()

    if args.inplace:
        ofname = args.input_file
    else:
        if args.ofname == '':
            s = os.path.splitext(os.path.basename(args.input_file))
            ofname = '{}_parsed.tsv'.format(s[0])
        else: ofname = args.ofname

    # read and format properly
    sys.stdout.write('\n{}\n\nReading {}...'.format(parser.prog, args.input_file))
    dat = parse_spectrum_report(args.input_file)
    sys.stdout.write(' Done!\n')

    # Check which columns exist
    required_cols = [PEPTIDE_SEQUENCE, SPECTRUM_NAME, EXPERIMENT_NAME, FIXED_MODIFICATIONS, VARIABLE_MODIFICATIONS]
    optional_cols = [MS_MS_SAMPLE_NAME, PROTEIN_NAME, PROTEIN_ACCESSION_NUMBERS,
                     OBSERVED_M_Z, EXCLUSIVE, SPECTRUM_CHARGE] 
    found_cols = list()
    sys.stdout.write('\nSearching for required columns.\n')
    for col in required_cols:
        if col in dat.columns:
            sys.stdout.write('\tFound "{}" column\n'.format(col))
            found_cols.append(col)
        else:
            sys.stderr.write('\tERROR: Could not find required column: "{}"\n\tExiting...\n'.format(col))
            sys.exit(-1)
    for col in optional_cols:
        if col in dat.columns:
            sys.stdout.write('\tFound "{}" column\n'.format(col))
            found_cols.append(col)
        else:
            sys.stderr.write('\tWARN: Could not find optional column: "{}"\n'.format(col))
    sys.stdout.write('Done!\n')

    # Check that all modifications are valid
    if args.calc_formula:
        sys.stdout.write('\nIterating through modifications to make sure their composition is known...')
        fixed_good = utils.check_modifications(get_unique_modifications(dat[FIXED_MODIFICATIONS].to_list()),
                                              'fixed', verbose=args.verbose)
        variable_good = utils.check_modifications(get_unique_modifications(dat[VARIABLE_MODIFICATIONS].to_list()),
                                             'variable', verbose=args.verbose)
        if not fixed_good and not variable_good:
            sys.exit(-1)
        sys.stdout.write('Success!\n')

    # extract scan column
    engine = detect_search_engine(dat)
    dat[SCAN_NUM] = dat[SEARCH_ENGINES[engine]['scan'][0]].apply(lambda x: SEARCH_ENGINES[engine]['scan'][1].search(x).group(1))

    # extract precursorFile column
    dat[PRECURSOR_FILE] = dat[SEARCH_ENGINES[engine]['msms'][0]].apply(lambda x: SEARCH_ENGINES[engine]['msms'][1].search(x).group(1))
    dat[PRECURSOR_FILE] = dat[PRECURSOR_FILE].apply(lambda x: '{}.{}'.format(os.path.splitext(x)[0], args.fileExt))

    # parse protein id and name
    # At some point should use re.findall
    if PROTEIN_ACCESSION_NUMBERS in found_cols:
        matches = [(ACCESSION_REGEX.search(s), s) for s in dat[PROTEIN_ACCESSION_NUMBERS].values.tolist()]
        good_matches = [m for m, _ in matches if bool(m)]
        if len(dat.index) != len([m for m, _ in matches if m]):
            sys.stderr.write('\nWARN: unable to parse protein acession for {} peptides!\n'.format(len(dat.index) -
                len(good_matches)))
            if args.verbose:
                sys.stderr.write('Bad acession(s):\n')
                bad_acessions = Counter(dat[[not bool(m) for m, _ in matches]][PROTEIN_ACCESSION_NUMBERS].values.tolist())
                for acession in bad_acessions.items():
                    sys.stderr.write('\t{} -> {} times\n'.format(*acession))
            if args.bad_id_filter:
                nBefore = len(dat.index)
                dat = dat[[bool(x) for x, _ in matches]]
                dat = dat.reset_index()
                nAfter = len(dat.index)
                sys.stderr.write('Removed {} unparsable entries.\n'.format(nBefore - nAfter))

        if args.bad_id_filter:
            dat[PARENT_ID] = pd.Series(list(map(lambda x: x.group(1), good_matches)))
            dat[PARENT_PROTEIN] = pd.Series(list(map(lambda x: x.group(2), good_matches)))
        else:
            ids = list()
            proteins = list()
            for match, string in matches:
                if match:
                    ids.append(match.group(1))
                    proteins.append(match.group(2))
                else:
                    match = UNIPROT_ID_REGEX.search(string)
                    if match:
                        ids.append(match.group(1))
                    else:
                        ids.append(string)
                    proteins.append(string)
            dat[PARENT_ID] = pd.Series(ids)
            dat[PARENT_PROTEIN] = pd.Series(ids)

    if PROTEIN_NAME in found_cols:
        dat[PARENT_DESCRIPTION] = dat[PROTEIN_NAME].str.extract(DESCRIPTION_REGEX)

    # add static modifications
    seq_list = dat[PEPTIDE_SEQUENCE].apply(str.upper).apply(utils.strToAminoAcids).tolist()
    formulas = [MolecularFormula() if not args.calc_formula else MolecularFormula(x.upper())
                for x in dat[PEPTIDE_SEQUENCE]]
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
        s = re.sub(r'{}\([\-\+\d\.]+\)'.format(args.mod_residue), '{}*'.format(args.mod_residue), s)
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
    select_cols = [x for x in keep_cols if x in dat.columns]
    dat = dat[select_cols]

    dat.rename({EXPERIMENT_NAME: SAMPLE_NAME,
                EXCLUSIVE: UNIQUE,
                OBSERVED_M_Z: PRECURSOR_MZ,
                SPECTRUM_CHARGE: CHARGE},
               axis='columns', inplace=True)

    sys.stdout.write('\nWriting {}...'.format(ofname))
    dat.to_csv(ofname, sep='\t', index=False)
    sys.stdout.write(' Done!\n')


if __name__ == '__main__':
    main()

