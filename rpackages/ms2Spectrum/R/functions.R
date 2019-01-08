
getAllSpectra <- function(fileVec)
{
  ret <- list()
  for(i in 1:length(fileVec))
  {
    ret[[i]] <- getSpectrum(fileVec[i])
  }
  return(ret)
}

