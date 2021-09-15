# ionFinder
Search MS2 spectra for diagnostic neutral loss ions.

# Configuring and building ionFinder

## System requirements
`ionFinder` in written in c++ 11 and be compiled on Linux or macOS. Windows is not supported. Additional helper scripts are written in Python 3.

* **GCC** or other compiler compatible with c++ 11
* **Cmake** (>= 3.9)
* **Python** (> 3.6) with **pandas** package installed.

## Configuring
The following example assumes that the program is being built in `~/code/ionFinder`
```bash
mkdir -p ~/code/ionFinder
cd ~/code/ionFinder
git clone --recurse-submodules https://github.com/weerapana-lab/ionFinder
mkdir -p ~/code/ionFinder/build
cd ~/code/ionFinder/build
cmake ~/code/ionFinder/ionFinder
```

You can set CMake configuration variables adding `-DVARIABLE=VALUE` options when calling CMake. The most important CMake variables are:

| Variable | Description |
| -------- | ----------- |
| CMAKE_CXX_COMPILER | Defines the C++ compiler to use.|
| PYTHON_EXE | Define python interpreter to use. |

## Building
To *build* ionFinder run:
```bash
make
```

If everything worked, the executable files should be in the `bin` directory
```bash
$ ls bin
ionFinder
parse_maxquant
parse_scaffold
```

# Usage

ionFinder works with 2 input modes.
1. `dtafilter` which reads PSMs from `DTASelect-filter.txt` files
2. `tsv` which reads PSMs from `.tsv` files 

```bash
ionFinder -i dtafilter --citStats <directory> [...] # dtafilter mode
ionFinder -i tsv --citStats <tsv_file>              # tsv mode
```

Run:
```bash
ionFinder --help
```
to get a comprehensive list of optional arguments and settings.

See the `examples` folder in the ionFinder parent directory for examples of each type of input file.

# Citation

Please cite:

Maurais, A. J.;  Salinger, A. J.;  Tobin, M.;  Shaffer, S. A.;  Weerapana, E.; Thompson, P. R., A Streamlined Data Analysis Pipeline for the Identification of Sites of Citrullination. _Biochemistry_ **2021**.
