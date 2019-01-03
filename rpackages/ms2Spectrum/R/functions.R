
OF_EXT = ".pdf"

makeOfName <- function(specDat)
{
  ret = specDat$metaData$parentFile
  ret = paste(ret, "_", specDat$metaData$sequence,
              "_", specDat$metaData$scanNum, sep = "")
  if(specDat$metaData$precursorCharge != 0)
  {
    ret = paste(ret, "_", specDat$metaData$precursorCharge, sep = "")
  }
  ret = paste(ret, OF_EXT, sep = "")
  return(ret)
}

getAllSpectra <- function(fileVec, nThread)
{
  ret <- list()
  for(i in 1:length(fileVec))
  {
    ret[[i]] <- getSpectrum(fileVec[i])
  }
  return(ret)
}

