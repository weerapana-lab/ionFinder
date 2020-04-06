
from collections import Counter

'''
Average and exact molecular weights. Masses are (average, exact)
'''
_ATOM_MASSES = {"C": (12.011, 12),
                "H": (1.008, 1.00783),
                "O": (15.999, 15.99491),
                "N": (14.007, 14.00307),
                "S": (32.06, 31.97207),
                "P": (30.97376, 30.97376),
                "(15)N": (15.00011, 15.00011),
                "(2)H": (2.0141, 2.0141),
                "(13)C": (13.00335, 13.00335),
                "Se": (78.96, 79.91652),
                "Cl": (35.45, 34.96885),
                "Br": (79.904, 78.91834)}

# base residue atoms
BASE_RESIDUE_ATOMS = {
        'A': Counter({'C': 3, 'H': 5, 'O': 1, 'N': 1}),
        'C': Counter({'C': 3, 'H': 5, 'O': 1, 'N': 1, 'S': 1}),
        'D': Counter({'C': 4, 'H': 5, 'O': 3, 'N': 1}),
        'E': Counter({'C': 5, 'H': 7, 'O': 3, 'N': 1}),
        'F': Counter({'C': 9, 'H': 9, 'O': 1, 'N': 1}),
        'G': Counter({'C': 2, 'H': 3, 'O': 1, 'N': 1}),
        'H': Counter({'C': 6, 'H': 7, 'O': 1, 'N': 3}),
        'I': Counter({'C': 6, 'H': 11, 'O': 1, 'N': 1}),
        'K': Counter({'C': 6, 'H': 12, 'O': 1, 'N': 2}),
        'L': Counter({'C': 6, 'H': 11, 'O': 1, 'N': 1}),
        'M': Counter({'C': 5, 'H': 9, 'O': 1, 'N': 1, 'S': 1}),
        'N': Counter({'C': 4, 'H': 6, 'O': 2, 'N': 2}),
        'P': Counter({'C': 5, 'H': 7, 'O': 1, 'N': 1}),
        'Q': Counter({'C': 5, 'H': 8, 'O': 2, 'N': 2}),
        'R': Counter({'C': 6, 'H': 12, 'O': 1, 'N': 4}),
        'S': Counter({'C': 3, 'H': 5, 'O': 2, 'N': 1}),
        'T': Counter({'C': 4, 'H': 7, 'O': 2, 'N': 1}),
        'V': Counter({'C': 5, 'H': 9, 'O': 1, 'N': 1}),
        'W': Counter({'C': 11, 'H': 10, 'O': 1, 'N': 2}),
        'Y': Counter({'C': 9, 'H': 9, 'O': 2, 'N': 1}),
        'U': Counter({'C': 3, 'H': 5, 'O': 1, 'N': 1, 'Se': 1}),
        'C_TERM': Counter({'H': 1, 'O': 1}),
        'N_TERM': Counter({'H': 1})}

MODIFICATIONS = {
        'acetyl': {'N-TERM': Counter({'C': 2, 'H': 2, 'O': 1, 'N': 0, 'S': 0})},
        'acetylation of the protein n-terminus': {'N-TERM': Counter({'C': 2, 'H': 2, 'O': 1, 'N': 0, 'S': 0})},
        'citrullination': {'R': Counter({'C': 0, 'H': -1, 'O': 1, 'N': -1, 'S': 0})},
        'desthiocit1': {'R': Counter({'C': 32, 'H': 44, 'N': 6, 'O': 9})},
        'deamidated': {'N': Counter({'C': 0, 'H': -1, 'O': 1, 'N': -1, 'S': 0}),
                       'Q': Counter({'C': 0, 'H': -1, 'O': 1, 'N': -1, 'S': 0})},
        'deamidation': {'N': Counter({'C': 0, 'H': -1, 'O': 1, 'N': -1, 'S': 0}),
                       'Q': Counter({'C': 0, 'H': -1, 'O': 1, 'N': -1, 'S': 0})},
        'gln->pyro-glu': {'N-TERM': Counter({'C': 0, 'H': -2, 'O': -1, 'N': 0, 'S': 0})},
        'oxidation': {'M': Counter({'C': 0, 'H': 0, 'O': 1, 'N': 0, 'S': 0})},
        'carbamidomethyl': {'C': Counter({'C': 2, 'H': 3, 'O': 1, 'N': 1, 'S': 0})},
        'iodoacetamide derivative': {'C': Counter({'C': 2, 'H': 3, 'O': 1, 'N': 1, 'S': 0})}
        }

def get_mod(name, residue):
    '''
    Get a copy of the Counter for a modification.

    Parameters
    ----------
    name: str
        Name of modification.
    residue: str
        Single letter aa code on which modification occurred.

    Raises
    ------
        KeyError if modification is not known.
    '''
    try:
        temp_mod = MODIFICATIONS[name.lower()][residue.upper()].copy()
    except KeyError:
        raise KeyError('Unknown modification: {}, for residue: {}'.format(name, residue))
    return temp_mod


def calc_mass(formula, mono_avg=1):
    '''
    Calculate mass of molecular formula.

    Parameters
    ----------
    formula: Counter
        Formula to calculate mass of.
    mono_avg: int
        One of (0, 1). 0 for average, 1 for monoisotopic.

    Returns
    -------
    mass: float
        mass of formula.
    '''

    mass = 0
    for atom, count in formula.items():
        mass += (_ATOM_MASSES[atom][mono_avg] * count)
    return mass

