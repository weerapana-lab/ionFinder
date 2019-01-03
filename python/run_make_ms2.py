
import sys
import os
import argparse
import subprocess

def main(argv):
    parser = argparse.ArgumentParser('run_make_ms2',
                                     description = 'run rscripts/makeMS2.R and manage parallelism.')
