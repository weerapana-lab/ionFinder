
import sys
import os
import re
import argparse
import pandas as pd

from modules.parent_parser import PARENT_PARSER
from modules import tsv_constants
from modules import maxquant_constants
from modules.molecular_formula import MolecularFormula
from modules import atom_table
from modules import utils

MODIFICATION_REGEX = re.compile(r'([A-Z_])\((.*?)(\(.*?\))?\)')


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
        for match in MODIFICATION_REGEX.findall(seq):
            ret.add((match[1].strip().lower(), ('N-TERM' if match[0] == '_' else match[0])))
    return list(ret)


def extractModifications(modified_sequences):
    '''
    Extract and parse modifications from peptide sequences.

    Parameters
    ----------
    modified_sequences: list
        A list of peptide sequences with modification.

    Returns
    -------
    amino_acids: list, formulas: list
        A list of lists of AminoAcid(s), and a list of MolecularFormula(s).
    '''

    formulas = list()
    amino_acids = list()
    for s in modified_sequences:
        seq_no_mod = ''
        matches = list(MODIFICATION_REGEX.finditer(s))
        indecies = [True for _ in range(len(s))]
        modification_indecies = list()
        for m in matches:
            modification_indecies.append(m.start())
            for i in range(len(indecies)):
                if i in range(m.start() + 1, m.end()):
                    indecies[i] = False

        new_modification_indecies = list()
        for i, (char, boo) in enumerate(zip(s, indecies)):
            if boo:
                if char != '_':
                    seq_no_mod += char
                if i in modification_indecies:
                    new_modification_indecies.append(len(seq_no_mod))

        amino_acids.append(utils.strToAminoAcids(seq_no_mod))
        formulas.append(MolecularFormula(seq_no_mod))

        for i, site in enumerate(new_modification_indecies):
            name = matches[i].group(2).lower().strip()
            residue = matches[i].group(1)
            if residue == '_':
                if site == 0:
                    residue = 'N-TERM'
                else: # May have to also add c-term at some point but I am too lazy to do it now.
                    raise ValueError('Invalid char in sequence: {}'.format(seq_no_mod))
            mod_temp = atom_table.get_mod(name, residue)
            formulas[-1].add_mod(name, residue)
            amino_acids[-1][site].mod += atom_table.calc_mass(mod_temp)

    return amino_acids, formulas


def main():

    parser = argparse.ArgumentParser(prog='parse_maxquant',
                                     parents=[PARENT_PARSER],
                                     description='Convert MaxQuant output to proper input for ionFinder tsv input.',
                                     epilog="parse_maxquant was written by Aaron Maurais.\n"
                                            "Email questions or bugs to aaron.maurais@bc.edu")

    parser.add_argument('-g', '--groupMethod', choices=[0, 1, 2], default=1,
                        help='How many spectra per peptide? 0: include all scans, '
                             '1: Only show the best spectra per sequence, file and charge state, '
                             '2: Group by charge; show the best spectra, per sequence, and file. '
                             'Default is 1.')

    parser.add_argument('-f', '--fixedMod', default='C:carbamidomethyl',
                        help='Specify fixed modification(s) if there are multiple modifications, '
                              'they should be comma separated. Default is "C:carbamidomethyl"')

    parser.add_argument('input_file', help='Name of file to parse. Should be a MaxQuant msms.txt file.')

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
    dat = pd.read_csv(args.input_file, sep='\t', low_memory=False)
    dat.columns = [c.lower().replace(' ', '_').replace('/', '') for c in dat.columns]
    dat = dat[dat[maxquant_constants.MSMS_SCAN_NUMBER].isna().apply(lambda x: not x)]
    if maxquant_constants.GENE_NAMES in dat.columns:
        dat = dat[dat[maxquant_constants.GENE_NAMES].isna().apply(lambda x: not x)]
    elif maxquant_constants.LEADING_PROTEINS in dat.columns:
        dat = dat[dat[maxquant_constants.LEADING_PROTEINS].isna().apply(lambda x: not x)]
    else:
        raise RuntimeError('"{}" and "{}" columns not found.'.format(maxquant_constants.GENE_NAMES,
                                                                     maxquant_constants.LEADING_PROTEINS))
    dat = dat.reset_index()
    sys.stdout.write(' Done!\n')

    # parse fixed modifications
    fixed_modifications = list()
    for mod in re.split('\s?,\s?', args.fixedMod):
        match = re.search(r'^([A-Z]):(.+)$', mod.strip())
        if match is None:
            raise RuntimeError('Could not parse fixed modification: {}'.format(mod))
        fixed_modifications.append((match.group(2), match.group(1)))
    fixed_modifications = list(set(fixed_modifications))

    # Check that modifications are valid
    variable_modifications = get_unique_modifications(dat[dat[maxquant_constants.MODIFICATIONS] !=
        'Unmodified'][maxquant_constants.MODIFIED_SEQUENCE].to_list())
    sys.stdout.write('Iterating through modifications to make sure their composition is known...')
    fixed_good = utils.check_modifications(fixed_modifications,
                                          'fixed', verbose=args.verbose)
    variable_good = utils.check_modifications(variable_modifications,
                                         'variable', verbose=args.verbose)
    if not fixed_good and not variable_good:
        return -1
    sys.stdout.write('Success!\n')

    # add columns to ret
    ret = pd.DataFrame()
    ret[tsv_constants.SAMPLE_NAME] = dat[maxquant_constants.EXPERIMENT]
    ret[tsv_constants.PRECURSOR_FILE] = dat[maxquant_constants.RAW_FILE].apply(lambda x: '{}.ms2'.format(x))

    # get parent protein data
    ret[tsv_constants.PARENT_ID] = dat[maxquant_constants.LEADING_PROTEINS].apply(lambda x: [i for i in x.split(';')][0])
    if maxquant_constants.GENE_NAMES in dat.columns:
        ret[tsv_constants.PARENT_PROTEIN] = dat[maxquant_constants.GENE_NAMES].apply(lambda x: [i for i in x.split(';')][0])
    if maxquant_constants.PROTEIN_NAMES in dat.columns:
        ret[tsv_constants.PARENT_DESCRIPTION] = dat[maxquant_constants.PROTEIN_NAMES].apply(lambda x: [i for i in x.split(';')][0])

    # parse sequences
    seq_list, formulas = extractModifications(dat[maxquant_constants.MODIFIED_SEQUENCE].to_list())

    # add formula column
    ret[tsv_constants.FORMULA] = pd.Series([str(x) for x in formulas])

    # get seq as string and change R(+0.98) to R*
    seq_str_list = list()
    for seq in seq_list:
        s = str()
        for a in seq:
            s += str(a)
        s = re.sub(r'{}\([\-\+\d\.]+\)'.format(args.mod_residue), '{}*'.format(args.mod_residue), s)
        seq_str_list.append(s)

    ret[tsv_constants.SEQUENCE] = pd.Series(seq_str_list)
    ret[tsv_constants.FULL_SEQUENCE] = pd.Series(seq_str_list)

    # add other columns
    ret[tsv_constants.SCAN_NUM] = dat[maxquant_constants.MSMS_SCAN_NUMBER].apply(lambda x: int(x))
    ret[tsv_constants.UNIQUE] = dat[maxquant_constants.PROTEINS].apply(lambda x: len(x.split(';')) == 1)
    ret[tsv_constants.PRECURSOR_MZ] = dat[maxquant_constants.MZ]
    ret[tsv_constants.CHARGE] = dat[maxquant_constants.CHARGE]
    ret[tsv_constants.SCORE] = dat[maxquant_constants.SCORE]

    # group ret by user specified method
    if args.groupMethod != 0:
        group_cols = [tsv_constants.PRECURSOR_FILE, tsv_constants.SEQUENCE]
        group_method = 'Grouping by {}, {}'.format(tsv_constants.PRECURSOR_FILE, tsv_constants.SEQUENCE)
        if args.groupMethod == 2:
            group_cols.append(tsv_constants.CHARGE)
            group_method += ', and {}'.format(tsv_constants.CHARGE)
        sys.stdout.write('{}.\n'.format(group_method))
        idx = ret.groupby(group_cols, sort=False)[tsv_constants.SCORE].transform(max) == ret[tsv_constants.SCORE]
        ret = ret[idx]
        ret = ret.drop_duplicates(subset=group_cols, keep='first')

    sys.stdout.write('Writing {}...'.format(ofname))
    ret.to_csv(ofname, sep='\t', index=False)
    sys.stdout.write(' Done!\n')


if __name__ == '__main__':
    main()


