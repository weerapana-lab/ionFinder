
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


def check_modifications(mod_list, name, verbose=False):
    '''
    Check whether modifications in mod_list exist in atom_table.MODIFICATIONS.

    Parameters
    ----------
    mod_list: list of tuples
        List of tuples of modifications where the first element in each tuple
        is the modification name, and the second element is the residue.
    name: str
        'fixed' or 'variable'

    Returns
    -------
    all_good: bool
        True if all modifications are known.
    '''

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

    all_good = True
    for mod in mod_list:
        if mod is not None and not (mod[0] in MODIFICATIONS.keys()
                and mod[1] in MODIFICATIONS[mod[0]]):
            all_good = False
            break

    if not all_good or verbose:
        sys.stdout.write('\n')
        print_found_mods(mod_list, name)

    return all_good


