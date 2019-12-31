
import matplotlib.pyplot as plt

class MS2_spectrum(object):
    def __init__(self, fname = ""):
        self.fname = fname

    def read(self, fname = ''):
        if self.fname == '':
            self.fname = fname

        try:
            with open(self.fname, 'r') as inF:
                lines = inF.readlines()
        except FileNotFoundError as e:
            raise FileNotFoundError('Could not open spectra file: {}'.format(self.fname))
        



    def plot(self, base_path):
        pass

def plot_spectrum(spectrum, base_path):
    spectrum.plot(base_path)

