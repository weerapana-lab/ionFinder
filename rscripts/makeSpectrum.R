
require(ggplot2, warn.conflicts = FALSE, quietly = TRUE)
#require(foreach, warn.conflicts = FALSE, quietly = TRUE)
#require(doParallel, warn.conflicts = FALSE, quietly = TRUE)

PROG_WD_HOME = paste(Sys.getenv("HOME"), "/local/ms2_annotator/rscripts", sep = "")
Rcpp::sourceCpp(paste(PROG_WD_HOME, '/src/cppFunctions.cpp', sep = ""), cacheDir = paste(PROG_WD_HOME, "/lib/cppFunctions", sep = ""))
Rcpp::sourceCpp(paste(PROG_WD_HOME, '/src/spectrum.cpp', sep = ""), cacheDir = paste(PROG_WD_HOME, "/lib/spectrum", sep = ""))
source(paste(PROG_WD_HOME, "/functions.R", sep = ""), echo=FALSE)

makeSpectrum <- function(specDat, simpleSequence = FALSE, includeMZLab = TRUE, plotSize = 'large')
{
  dat <- specDat$spectrum
  dat$label <- as.character(dat$label)
  #dat$ionType <- as.character(dat$ionType)
  dat$ionType <- factor(x = dat$ionType, levels = c('blank', 'b', 'y', 'y_nl', 'b_nl'))
  dat$formatedLabel <- as.character(dat$formatedLabel)
  
  SEQ_STAR <- makeSeqVec(specDat$metaData$fullSequence, !simpleSequence)
  foundIons <- unique(getFoundIons(dat[dat$ionType != 'blank',]$ionType, dat[dat$ionType != 'blank',]$ionNum))
  B_IONS_EXP <- makeIonLabelsExp("b", length(SEQ_STAR) - 1, 'f')
  Y_IONS_EXP <- makeIonLabelsExp("y", length(SEQ_STAR) - 1, 'r')
  B_IONS = makeIonLabels("b", length(SEQ_STAR) - 1, 'f')
  Y_IONS <- makeIonLabels("y", length(SEQ_STAR) - 1, 'r')
  Y_COLOR = "red"
  B_COLOR = "blue"
  Y_NL_COLOR = "orange1"
  B_NL_COLOR = "green4"
  BLANK_COLOR = 'grey35'
  INCLUDE_ARROWS = (nrow(dat[dat$includeArrow,]) > 0)
  
  MZ_RANGE <- max(dat$mz) - min(dat$mz)
  
  if(simpleSequence){
    if(plotSize == 'small'){
      SEQ_SPACE = MZ_RANGE/64.90187
    } else {
      SEQ_SPACE = MZ_RANGE/88.50255
    }
    SEQ_X_LEN = SEQ_SPACE * length(SEQ_STAR)
  } else {
    if(plotSize == 'small'){
      SEQ_SPACE = MZ_RANGE/36
    } else {
      SEQ_SPACE = MZ_RANGE/55
    }
    SEQ_X_LEN = SEQ_SPACE * length(SEQ_STAR)
  }
  
  SEQ_BEGIN_X = max(dat$mz) - SEQ_X_LEN
  
  if(includeMZLab) {
    BY_LAB_OFFSET_Y = 5
  } else {
    BY_LAB_OFFSET_Y = 0
  }
  
  if(max(dat[dat$mz > (SEQ_BEGIN_X - 30),]$intensity) > 80)
  {
    SEQ_Y_LEVLE = (max(dat[dat$mz > SEQ_BEGIN_X - 30,]$intensity) + 30)
  } else {
    SEQ_Y_LEVLE = 100
  }
  Y_SCALE_LIM = SEQ_Y_LEVLE + 15
  
  y_labs <- c(0, 25, 50, 75, 100)
  mspect <- ggplot(data = dat, aes(x = mz, ymax = intensity, y = intensity, ymin = 0, color = ionType)) +
    geom_linerange()
  if(includeMZLab) {
    mspect <- mspect + geom_text(data = subset(dat, dat$includeLabel == TRUE),
                  aes(label = mz, x = labelX, y = labelY),
                  parse = FALSE,
                  size = 3)
  }
  mspect <- mspect + geom_text(data = subset(dat, dat$ionType == "b" | dat$ionType == 'y' | dat$ionType == 'y_nl' | dat$ionType == 'b_nl'),
              aes(label = formatedLabel, x = labelX, y = labelY + BY_LAB_OFFSET_Y),
              parse = TRUE,
              size = 3) +
    scale_y_continuous(breaks = y_labs, expand = c(0, 0), limits = c(0, Y_SCALE_LIM)) +
    scale_color_manual(values = c("b" = B_COLOR, "blank" = BLANK_COLOR, "y" = Y_COLOR, "y_nl" = Y_NL_COLOR, "b_nl" = B_NL_COLOR)) +
    theme_classic() +
    theme(panel.grid = element_blank(),
          legend.position = "none") +
    ylab("Relative intensity") + 
    xlab("m/z")
  
  if(INCLUDE_ARROWS){
    mspect <- mspect + geom_segment(data = subset(dat, dat$includeArrow),
                 aes(x = arrowBegX, y = arrowBegY, xend = arrowEndX, yend = arrowEndY),
                 linetype = 'dashed')
  }
  
  if(simpleSequence){
    mspect <- mspect + annotate("text", y = SEQ_Y_LEVLE - 5, x = SEQ_BEGIN_X, label = B_IONS_EXP[1], 
                                    parse = TRUE, color = B_COLOR, hjust = 1) +
      annotate("text", y = SEQ_Y_LEVLE + 5, x = SEQ_BEGIN_X, label = Y_IONS_EXP[1], parse = TRUE, 
               color = Y_COLOR, hjust = 1)
    for(i in 1:length(SEQ_STAR)){
      SEQ_BEGIN_X = SEQ_BEGIN_X + SEQ_SPACE
      mspect <- mspect + annotate("text", y = SEQ_Y_LEVLE, x = SEQ_BEGIN_X, size = 5,
                                      label = SEQ_STAR[i], hjust = 0.5, family = "Courier")
    }
    SEQ_BEGIN_X = SEQ_BEGIN_X + SEQ_SPACE
    mspect <- mspect + annotate("text", y = SEQ_Y_LEVLE - 5, x = SEQ_BEGIN_X, label = B_IONS_EXP[length(B_IONS_EXP)], 
                                    parse = TRUE, color = B_COLOR, hjust = 0) +
      annotate("text", y = SEQ_Y_LEVLE + 5, x = SEQ_BEGIN_X, label = Y_IONS_EXP[length(Y_IONS_EXP)], 
               parse = TRUE, color = Y_COLOR, hjust = 0)
  } else {
    
    LINE_BEGIN_X = SEQ_BEGIN_X + (SEQ_SPACE / 2)
    LINE_LEN = SEQ_Y_LEVLE / 10
    LINE_Y_BEGIN = SEQ_Y_LEVLE - (LINE_LEN/2)
    LINE_Y_END = SEQ_Y_LEVLE + (LINE_LEN/2)
    
    FRAG_LINE_LEN = SEQ_SPACE/2
    TOP_FRAG_LINE_END_X = LINE_BEGIN_X + FRAG_LINE_LEN
    BOTTOM_FRAG_LINE_END_X = LINE_BEGIN_X - FRAG_LINE_LEN
    
    TOP_LABEL_BEGIN_X = SEQ_BEGIN_X + (SEQ_SPACE * (1/3)) * 2
    BOT_LABEL_BEGIN_X = SEQ_BEGIN_X + SEQ_SPACE * (1/3)
    TOP_LABEL_Y_LEVEL = SEQ_Y_LEVLE + LINE_LEN/2 + 5
    BOT_LABEL_Y_LEVEL = SEQ_Y_LEVLE - LINE_LEN/2 - 5
    
    #add sequence to mspect
    for(i in 1:length(SEQ_STAR))
    {
      mspect <- mspect + annotate('text', y = SEQ_Y_LEVLE, x = SEQ_BEGIN_X, label = SEQ_STAR[i])
      if(i < length(SEQ_STAR))
      {
        bFound = B_IONS[i] %in% foundIons
        yFound = Y_IONS[i] %in% foundIons
        
        if(bFound | yFound){
          #vertical lines
          mspect <- mspect + annotate('segment', x = LINE_BEGIN_X, xend = LINE_BEGIN_X, y = LINE_Y_BEGIN, yend = LINE_Y_END)
        }
        
        if(bFound){
          #bottom frag line
          mspect <- mspect + annotate('segment', x = LINE_BEGIN_X, xend = BOTTOM_FRAG_LINE_END_X, y = LINE_Y_BEGIN, yend = LINE_Y_BEGIN)
          
          #b ion label
          mspect <- mspect + annotate('text', x = BOT_LABEL_BEGIN_X, y = BOT_LABEL_Y_LEVEL,
                                      label = B_IONS_EXP[i], parse = TRUE, size = 3, color = 'blue')
        }
        
        if(yFound){
          #top frag line
          mspect <- mspect + annotate('segment', x = LINE_BEGIN_X, xend = TOP_FRAG_LINE_END_X, y = LINE_Y_END, yend = LINE_Y_END)
          
          #y ion label
          mspect <- mspect + annotate('text', x = TOP_LABEL_BEGIN_X, y = TOP_LABEL_Y_LEVEL,
                                          label = Y_IONS_EXP[i], parse = TRUE, size = 3, color = 'red')
        }
       
      }
      SEQ_BEGIN_X = SEQ_BEGIN_X + SEQ_SPACE
      LINE_BEGIN_X = LINE_BEGIN_X + SEQ_SPACE
      TOP_FRAG_LINE_END_X = LINE_BEGIN_X + FRAG_LINE_LEN
      BOTTOM_FRAG_LINE_END_X = LINE_BEGIN_X - FRAG_LINE_LEN
      TOP_LABEL_BEGIN_X = TOP_LABEL_BEGIN_X + SEQ_SPACE
      BOT_LABEL_BEGIN_X = BOT_LABEL_BEGIN_X + SEQ_SPACE
    }
  }
  
  return(list(ofname = makeOfName(specDat), spectrum = mspect))
}

makeAllSpectrum <- function(spectraFiles, ...)
{
  ret <- list()
  for(i in 1:length(spectraFiles))
  {
    ret[[i]] <- makeSpectrum(spectraFiles[[i]], ...)
  }
  return(ret)
}

