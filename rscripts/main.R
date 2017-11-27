
PROG_WD_HOME = paste(Sys.getenv("HOME"), 
                  "/scripts/ms2_anotator/rscripts", sep = "")
source(paste(PROG_WD_HOME, "/makeSpectrum.R", sep = ""), echo=FALSE)
source(paste(PROG_WD_HOME, "/functions.R", sep = ""), echo=FALSE)
require(tools, quietly = TRUE, warn.conflicts = FALSE)

WD <- file_path_as_absolute("./")
OD <- WD
argv <- commandArgs(trailingOnly = TRUE)
iBeg <- 1

if(argv[1] == "RStudio")
{
  argv <- c("Rstudio", "testFiles/20160418_Healthy_P1_SF_02_TVAAPSVFIFPPSDEQLK_16141_3.spectrum", 
           "testFiles/20160418_Healthy_P1_SF_02_TVAAPSVFIFPPSDEQLK_13492_2.spectrum")
  iBeg <-  2
}

argc <- length(argv)
i = iBeg
endArgs = FALSE
inFiles <- NA
ofnames <- NA

while(i <= argc){
  if(!isArg(argv[i]) && !endArgs)
  {
    if(argv[i] == "-o")
    {
      i = i + 1
      ofnames = argv[i]
      if(is.na(ofnames))
      {
        printUsage()
        stop("Bad args!")
      }
    }
    else if(argv[i] == "-od")
    {
      i = i + 1
      OD <- file_path_as_absolute(argv[i])
      if(is.na(OD))
      {
        printUsage()
        stop("Bad args!")
      }
    }
  }
  else if(isArg(argv[i]))
  {
    endArgs = TRUE
    if(is.na(inFiles))
      inFiles <- file_path_as_absolute(argv[i])
    else inFiles <- c(inFiles, file_path_as_absolute(argv[i]))
  }
  i = i + 1
}

#fix args
OD <- fixOD(OD)

#read spectra files and make spectra
print("Reading spectrum files...", quote = FALSE)
spectraFiles <- getAllSpectra(inFiles)
print("Done!", quote = FALSE)
print("Generating labeled ms2 spectra...", quote = FALSE)
spectra <- makeAllSpectrum(spectraFiles)
print("Done!", quote = FALSE)
print("Writing spectra...", quote = FALSE)

#write files
for(spec in spectra)
{
  #ggsave(paste("rplots/", spec$ofname, sep = ""), plot = spec$spectrum, dpi = 600, width = 12, height = 4)
  print(paste("Working on ", spec$ofname, "...", sep = ""), quote = FALSE)
  ggsave(paste(OD, spec$ofname, sep = ""), plot = spec$spectrum, dpi = 600, width = 12, height = 4)
  print("Done", quote = FALSE)
}
