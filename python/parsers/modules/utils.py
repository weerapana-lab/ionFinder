
import sys
import re
from numpy import nan

from .atom_table import MODIFICATIONS

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


def get_unique_modifications(mods, modification_regex):
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
            mod = re.search(modification_regex, x)
            if mod:
                ret.add((mod.group(3).lower(), mod.group(1).upper()))
            else:
                ret.add(None)
                sys.stderr.write('ERROR: Could not parse modification.\n\t{}\n'.format(x))

    return list(ret)


def check_modifications(mod_list, name, modification_regex, verbose=False):

    def print_found_mods(lst, name):
        sys.stdout.write('{} modifications:\n'.format(name))
        for mod in lst:
            known_mod = mod[0] in MODIFICATIONS.keys()
            if known_mod:
                known_residue = mod[1] in MODIFICATIONS[mod[0]].keys()
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

    mod_check = get_unique_modifications(mod_list, modification_regex)

    all_good = True
    for mod in mod_check:
        if mod is not None and not (mod[0] in MODIFICATIONS.keys()
                and mod[1] in MODIFICATIONS[mod[0]]):
            all_good = False
            break

    if not all_good or verbose:
        sys.stdout.write('\n')
        print_found_mods(mod_check, name)

    return all_good


