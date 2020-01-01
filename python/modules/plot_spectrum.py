
import sys
import re
import matplotlib.pyplot as plt
import pandas as pd

if sys.version_info[0] < 3:
    from StringIO import StringIO
else:
    from io import StringIO

from spectrum_constants import *


class MS2_spectrum(object):

    SPECTRUM_DELIM = '\t'

    METADATA_VALUES = {PRECURSOR_FILE: str,
                       OFNAME: str,
                       SCAN_NUMBER: str,
                       SEQUENCE: str,
                       FULL_SEQUENCE: str,
                       RET_TIME: str,
                       PRECURSOR_CHARGE: str,
                       PLOT_HEIGHT: float,
                       PLOT_WIDTH: float,
                       PRECURSOR_INT: float,
                       PRECURSOR_SCAN: str}

    COLUMN_HEADERS = [MZ,
                      INTENSITY,
                      LABEL,
                      INCLUDE_LABEL,
                      ION_TYPE,
                      ION_NUM,
                      FORMATED_LABEL,
                      LABEL_X,
                      LABEL_Y,
                      INCLUDE_ARROW,
                      ARROW_BEG_X,
                      ARROW_BEG_Y,
                      ARROW_END_X,
                      ARROW_END_Y]

    def __init__(self, fname = ""):
        self.fname = fname
        self.metadata = {k:None for k in self.METADATA_VALUES.keys()}
        self.spectrum = pd.DataFrame()

    def read(self, fname = ''):
        '''
        Read .spectrum file into MS2_spectrum object.

        Paramaters
        ----------
        fname : str
            Path to file to read.

        Returns
        -------
            sucess : bool
                True if sucessful, False of not.
        '''

        if self.fname == '':
            self.fname = fname

        try:
            with open(self.fname, 'r') as inF:
                f_buff = inF.read()
        except FileNotFoundError as e:
            sys.stderr.write('Could not open spectra file: {}'.format(self.fname))
            return False

        def read_bounded_chunk(begin_tag, end_tag, chunk_name):
            m = re.search('{}([\w\W]*){}'.format(begin_tag, end_tag), f_buff)
            if not m:
                RuntimeError('Malformed .spectrum file! Could not find {}.'.format(chunk_name))
            return [x for x in [x.strip() for x in re.split(r'[\r]{0,1}\n', m.group(1))] if x]

        # Parse file chunks
        metadata_text = read_bounded_chunk(BEGIN_METADATA, END_METADATA, 'metadata')
        spectrum_text = read_bounded_chunk(BEGIN_SPECTRUM, END_SPECTRUM, 'spectrum data')

        # Parse metadata
        for line in metadata_text:
            elems = [x.strip() for x in line.split(sep= self.SPECTRUM_DELIM)]
            try:
                assert(len(elems) == 2)
                self.metadata[elems[0]] = self.METADATA_VALUES[elems[0]](elems[1])
            except AssertionError as e:
                sys.stderr.write('error: invalid line: {}'.format(line))
                return False
            except KeyError as e:
                sys.stderr.write('WARN: Unknown metadata value: {}'.format(elems[0]))
            except ValueError as e:
                sys.stderr.write('WARN: Invalid metadata data type: {}({}) -> {}'.format(elems[0],
                                                                                         self.METADATA_VALUES[elems[0]],
                                                                                         elems[1]))

        # Check that all values were found.
        sucess = True
        for k, v in self.metadata.items():
            if v is None:
                sys.stderr.write('Did not find metadata value: {}'.format(k))
                sucess = False
        if not sucess:
            return False

        #Parse spectrum
        with StringIO('\n'.join(spectrum_text)) as inF:
            self.spectrum = pd.read_csv(inF, sep='\t')

        return True


    def plot(self, base_path):
        pass


def plot_spectrum(spectrum, base_path):
    spectrum.plot(base_path)

