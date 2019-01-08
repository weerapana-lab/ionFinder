
argv <- commandArgs(trailingOnly = FALSE)

fileArgName <- "--file="
scriptName <- sub(fileArgName, "", argv[grep(fileArgName, argv)])
progWD <- sub("rscripts/makeMs2.R", "", scriptName)

library(ggplot2)
library(ms2Spectrum, lib.loc = paste0(progWD, 'lib'))
library(tools)

if(argv[1] == "RStudio")
{
  #argv <- c("Rstudio", "/Volumes/Data/msData/ms2_anotator/citFinder/rorpad_mouse/spectraFiles/ror_pad_rVEHLQYNLELAFHHHLCK_10974_4.spectrum")
  argv <- c('RStudio', "-mzLab", "1", "-pSize", "large", "-simpleSeq", "0", 
            "/Volumes/Data/msData/ms2_anotator/citFinder/rorpad_mouse/spectraFiles/ror_pad_TNLFSrEEVTSYQr_12030_2.spectrum",
            "/Volumes/Data/msData/ms2_anotator/citFinder/rorpad_mouse/spectraFiles/ror_pad_TNLFSREEVTSYQr_9314_2.spectrum", 
            "/Volumes/Data/msData/ms2_anotator/citFinder/rorpad_mouse/spectraFiles/ror_pad_TNLFSrEEVTSYQrK_11085_2.spectrum", 
            "/Volumes/Data/msData/ms2_anotator/citFinder/rorpad_mouse/spectraFiles/ror_pad_TNLFSrEEVTSYQrK_11182_3.spectrum", 
            "/Volumes/Data/msData/ms2_anotator/citFinder/rorpad_mouse/spectraFiles/ror_pad_TNLFSREEVTSYQrK_12763_2.spectrum",
            "/Volumes/Data/msData/ms2_anotator/citFinder/rorpad_mouse/spectraFiles/ror_pad_TPPAGSrGADTLTYTLGLSDGQLPLGASPDLPEASACPPGLLr_26105_3.spectrum")
  iBeg <-  2
} else {
  iBeg = min(grep('--args', argv))
}

WD <- file_path_as_absolute("./")
OD <- WD
argc <- length(argv)
i = iBeg
endArgs = FALSE
inFiles <- NA
ofnames <- NA
includeMzLab <- TRUE
simpleSeq <- FALSE
plotSize <- 'large'
plotWidth <- 12
plotHeight <- 4
nDigits <- 2


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
        stop("Bad arg for -od")
      }
    }
    else if(argv[i] == "-mzLab")
    {
      i = i + 1
      includeMzLab <- as.logical(as.integer(argv[i]))
      if(is.na(includeMzLab))
      {
        printUsage()
        stop("Bad arg for -mzLab")
      }
    }
    else if(argv[i] == "-round")
    {
      i = i + 1
      nDigits <- as.integer(argv[i])
      if(is.na(includeMzLab))
      {
        printUsage()
        stop("Bad arg for -round")
      }
    }
    else if(argv[i] == "-simpleSeq")
    {
      i = i + 1
      simpleSeq <- as.logical(as.integer(argv[i]))
      if(is.na(simpleSeq))
      {
        printUsage()
        stop("Bad arg for -simpleSeq")
      }
    }
    else if(argv[i] == "-pSize")
    {
      i = i + 1
      plotSize <- argv[i]
      if(plotSize == 'small'){
        plotWidth <- 8
        plotHeight <- 3
      } else if(plotSize == 'large' | plotSize == 'default'){
        plotWidth <- 12
        plotHeight <- 4
      } else {
        printUsage()
        stop("Bad arg for -pSize")
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
spectra <- makeAllSpectrum(spectraFiles, includeMZLab = includeMzLab, 
                           simpleSequence = simpleSeq, plotSize = plotSize, nDigits = nDigits)
print("Done!", quote = FALSE)
print("Writing spectra...", quote = FALSE)

#write files
for(i in 1:length(inFiles))
{
  print(paste("Working on ", spectra[[i]]$ofname, "...", sep = ""), quote = FALSE)
  ggsave(paste(base::dirname(inFiles[i]), spectra[[i]]$ofname, sep = '/'),
         plot = spectra[[i]]$spectrum, dpi = 600, width = plotWidth, height = plotHeight)
  print("Done", quote = FALSE)
}
