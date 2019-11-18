
import os
import re
import pandas as pd
import numpy as np
import argparse


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


def extractModifications(seq, mods):
    '''
    Extract modifications from modifications description and populate
    to a list of AminoAcid objects.

    Parameters
    ---------
    seq : list
        List of AminoAcid objects
    mod: str
        Description of modifications on peptide
    '''

    if mods is np.nan:
        return

    # extract modified residue, number, and mod mass from mod
    # and put into a list with an element for each mod
    for x in list(map(str.strip, mods.split(','))):
        mod = re.search('^([nc]-term|([a-zA-Z])([0-9]+)):.+\(([+-][0-9.]+)\)$', x)
        if not mod or len(mod.groups()) != 4:
            print('Warning: Error parsing modifications from string: {}'.format(x))
            continue

        modMass = float(mod.group(4))
        if mod.group(1) == 'n-term':
            seq[0].mod += modMass
        elif mod.group(1) == 'c-term':
            mod[-1].mod += modMass
        else:
            residue = mod.group(2).upper()
            num = int(mod.group(3))
            # sanity check
            if seq[num].aa.upper() != residue:
                raise RuntimeError('mods do not correspond to sequence.')

            # add modMass to seq[num].mods
            # this will not cause an off by 1 error because seq[0] is the n terminal modification
            seq[num].mod += modMass


def main():

    parser = argparse.ArgumentParser(prog='parse_scaffold',
                                     description='Convert Scaffold output to proper input for ionFinder tsv input.',
                                     epilog="parse_scaffold was written by Aaron Maurais.\n"
                                            "Email questions or bugs to aaron.maurais@bc.edu")

    parser.add_argument('input_file', help = 'Name of file to parse. Should be a tab delimited text file.')

    parser.add_argument('--inplace', default=False, action='store_true',
                        help='Should input_file be overwritten? Overrides ofname.')
    parser.add_argument('-o', '--ofname', help='Name of output file.', default = '')

    #arguments to customize mod residue
    parser.add_argument('--mod_residue', default='R',
                        help = "Residue to put '*' on")
    parser.add_argument('--mod_mass', default = 0.98, type=float,
                        help='Mass of modification.')

    args = parser.parse_args()

    if args.inplace:
        ofname = args.input_file
    else:
        if args.ofname == '':
            s = os.path.splitext(os.path.basename(args.input_file))
            ofname = '{}_parsed{}'.format(s[0], s[1])
        else: ofname = args.ofname

    # read and format properly
    dat = pd.read_csv(args.input_file, sep='\t')
    dat.columns = [x.replace(' ', '_').lower() for x in dat.columns.tolist()]

    # extract scan column
    dat['scanNum'] = dat['spectrum_name'].apply(lambda x: re.search(',scan_([0-9]+),type', x).group(1))

    # extract precursorFile column
    dat['precursorFile'] = dat['spectrum_name'].apply(lambda x: re.search('^(\w+),', x).group(1) + '.ms2')

    seq_list = dat['peptide_sequence'].apply(str.upper).apply(strToAminoAcids).tolist()

    # parse protein id and name
    uniprot_id_re = '^(sp|tr)\|([A-Z0-9-]+)\|([A-Za-z0-9]+)_\w+ (.+) OS='
    matches = [re.search(uniprot_id_re, s) for s in dat['protein_name'].values.tolist()]
    dat = dat[[bool(x) for x in matches]]
    matches = [re.search(uniprot_id_re, s) for s in dat['protein_name'].values.tolist()]
    dat['parentProtein'] = pd.Series(list(map(lambda x: x.group(3), matches)))
    dat['parentID'] = pd.Series(list(map(lambda x: x.group(2), matches)))
    dat['parentDescription'] = pd.Series(list(map(lambda x: x.group(4), matches)))

    # add static modifications
    for i, value in dat['fixed_modifications_identified_by_spectrum'].iteritems():
        extractModifications(seq_list[i], value)

    # add dynamic modifications
    for i, value in dat['variable_modifications_identified_by_spectrum'].iteritems():
        extractModifications(seq_list[i], value)

    # get seq as string and change R(+0.98) to R*
    seq_str_list = list()
    for seq in seq_list:
        s = str()
        for a in seq:
            s += str(a)
        _mod_temp = '{0}({1:+})'.format(args.mod_residue, args.mod_mass)
        s = s.replace(_mod_temp, '{}*'.format(args.mod_residue))
        seq_str_list.append(s)

    dat['sequence'] = pd.Series(seq_str_list)
    dat['fullSequence'] = pd.Series(seq_str_list)

    keep_cols = ["experiment_name",
                 'precursorFile',
                 "parentID",
                 'parentProtein',
                 'parentDescription',
                 'fullSequence',
                 'sequence',
                 'scanNum',
                 'exclusive',
                 'observed_m/z',
                 "spectrum_charge"]

    dat = dat[keep_cols]

    dat.rename({'experiment_name': 'sampleName',
                'exclusive': 'unique',
                'observed_m/z': 'precursorMZ',
                'spectrum_charge': 'charge'},
               axis='columns', inplace=True)

    dat.to_csv(ofname, sep='\t', index=False)


if __name__ == '__main__':
    main()

