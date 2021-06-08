
# ionFinder examples
There is a directory for each type of supported input format. Each folder has an `output` sub-directory which has the expected output files if you run the commands in the examples.

## DTASelect-filter
The default input mode is `dtafilter`. In `dtafilter` mode each DTASelect-filter file should have a directory containing:

1. The filter file.
2. All the MS data files corresponding to the PSMs in the filter file.
3. A `sequest.params` file with the modification parameters used in the search.

The command:
```bash
cd DTASelect-filter
ionFinder --citStats 20190912_Thompson_PAD2_*
```

Should produce a single `.tsv` file: `peptide_cit_stats.tsv`. The `peptide_cit_stats` file has a row for each PSM. The `contains_Cit` column has the value of `true`, `likely`, `ambiguous`, or `false` assigned to the PSM.

## tsv

The other input mode is `tsv`. In `tsv` input mode, the PSMs to validate are given as a `.tsv` file with the columns "sampleName", "sequence", "precursorFile", "formula", "scanNum" in any order.

A minimal example:

|sampleName | precursorFile | sequence| formula	| scanNum |
|---------- | ------------- | --------| -------	| ------- |
| sample | file.mzML | AEMMELNDR*FASYIEK | C84H130N20O29S2 | 47677 |
| sample | file.mzML | AEM(15.9900)MELNDR*FASYIEK | C84H130N20O30S2 | 43710 |
| sample | file.mzML | AGDKAGR*AGAGM(15.9900)PPYHR | C72H113N25O23S | 7415 |
| sample | file.mzML | AGFLVTAR*GGSGIVVAR | C72H122N22O21 | 41163 |
| sample | file.mzML | AGR*AGAGM(15.9900)PPYHR | C57H88N20O17S | 8204 |
| sample | file.mzML | ALAR*EVDLKDYEDQQK | C82H132N22O31 | 28728 |

In the "sequence" column, citrulline is represented by an asterisk ("*"); Other modifications are given as the delta mass in parenthesis following the modified amino acid (i.e M(15.9900)).

The command:

```bash
cd tsv
ionFinder -i tsv --citStats input.tsv
```

Should produce `peptide_cit_stats.tsv` in the same format as the `dtafilter` output.

## MaxQuant

The helper script `parse_maxquant` converts the `msms.txt` file to a `.tsv` file which is compatible with `ionFinder`.

```bash
cd MaxQuant
parse_maxquant msms.txt
ionFinder -i tsv --citStats msms_parsed.tsv
```

## Scaffold

The helper script `parse_scaffold` converts the Spectrum report file exported from Scaffold viewer to a `.tsv` file which is compatible with `ionFinder`.

```bash
cd Scaffold
parse_scaffold -f 1 spectrum_report.tsv
ionFinder -i tsv --citStats spectrum_report_parsed.tsv
```

