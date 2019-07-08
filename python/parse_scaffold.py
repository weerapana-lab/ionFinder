
import os
import re
import pandas as pd
import numpy as np


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


def strToAminoAcids(seq: str):
    ret = list()

    # add n term
    ret.append(AminoAcid('', float()))

    for c in seq:
        ret.append(AminoAcid(c, float()))

    # add c term
    ret.append(AminoAcid('', float()))

    return ret


def extractModifications(seq: list, mods: str):
    '''
    Extract modifications from modifications description and populate
    to a list of AminoAcid objects.

    Paramaters
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


# read and format properly
dat = pd.read_csv('data/20190301_ThompsonLab_Kaplan.tsv', sep='\t')
# dat = pd.read_csv('data/20190320_ThompsonLab_Kallin.tsv', sep = '\t')
dat.columns = [x.replace(' ', '_').lower() for x in dat.columns.tolist()]

# extract scan column
dat['scanNum'] = dat['spectrum_name'].apply(lambda x: re.search(',scan_([0-9]+),type', x).group(1))

# extract precursorFile column
dat['precursorFile'] = dat['spectrum_name'].apply(lambda x: re.search('^(\w+),', x).group(1) + '.ms2')

seq_list = dat['peptide_sequence'].apply(str.upper).apply(strToAminoAcids).tolist()

# parse protein id and name
matches = [re.search('^(sp|tr)\|([A-Z0-9-]+)\|([A-Za-z0-9]+)_\w+ (.+) OS=',
                     s) for s in dat['protein_name'].values.tolist()]
dat = dat[[bool(x) for x in matches]]
matches = [re.search('^(sp|tr)\|([A-Z0-9-]+)\|([A-Za-z0-9]+)_\w+ (.+) OS=',
                     s) for s in dat['protein_name'].values.tolist()]
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
    s = s.replace('R(+0.98)', 'R*')
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

dat.to_csv('tables/20190301_ThompsonLab_Kaplan_parsed.tsv', sep='\t', index=False)
# dat.to_csv('tables/20190320_ThompsonLab_Kaplan_parsed.tsv', sep = '\t', index = False)

