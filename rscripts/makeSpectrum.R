
library(ggplot2)
library(ggrepel)
library(stats)

Rcpp::sourceCpp('rscripts/cppFunctions.cpp')
#source('~/Xcode_projects/ms2_anotator/rscripts/functions.R', echo=FALSE)

dat <- read.csv("testFiles/testScan.tsv", stringsAsFactors = FALSE, sep = '\t', na.strings = NA, quote = "")
dat[dat$label == "",]$label <- NA
dat$labBool <- ((dat$intensity > 5) | !is.na(dat$label))
#dat[dat$labBool,]$formated_label <- paste(dat[dat$labBool,]$formated_label, "\n", dat[dat$labBool,]$mz, sep = "")

SEQ <- "WCAVSEHEATK"
SEQ_STAR <- makeSeqVec(SEQ)
B_IONS <- makeIonLabelsExp("B", length(SEQ_STAR) - 1, 'f')
Y_IONS <- makeIonLabelsExp("Y", length(SEQ_STAR) - 1, 'r')
Y_COLOR = "red"
B_COLOR = "blue"

SEQ_BEGIN_X = 900
SEQ_SPACE = 20
SEQ_Y_LEVLE = 110

LINE_BEGIN_X = SEQ_BEGIN_X + (SEQ_SPACE / 2)
LINE_LEN = 10
LINE_Y_BEGIN = SEQ_Y_LEVLE - (LINE_LEN/2)
LINE_Y_END = SEQ_Y_LEVLE + (LINE_LEN/2)

FRAG_LINE_LEN = 10
TOP_FRAG_LINE_END_X = LINE_BEGIN_X + FRAG_LINE_LEN
BOTTOM_FRAG_LINE_END_X = LINE_BEGIN_X - FRAG_LINE_LEN

TOP_LABEL_BEGIN_X = SEQ_BEGIN_X + 8 + 10
BOT_LABEL_BEGIN_X = SEQ_BEGIN_X + 8
TOP_LABEL_Y_LEVEL = 110 + LINE_LEN/2 + 5
BOT_LABEL_Y_LEVEL = 110 - LINE_LEN/2 - 5

y_labs <- c(0, 25, 50, 75, 100)

spectrum <- ggplot(data = dat, aes(x = mz, ymax = intensity, y = intensity, ymin = 0)) +
  geom_linerange() +
  geom_text(data = subset(dat, dat$labBool == TRUE),
                  aes(label = mz), #, x = label_x, y = label_y),
                  parse = TRUE) +
  scale_y_continuous(breaks = y_labs) +
  theme_bw() +
  theme(panel.grid = element_blank())
  
#add sequence to spectrum
for(i in 1:length(SEQ_STAR))
{
  spectrum <- spectrum + annotate('text', y = SEQ_Y_LEVLE, x = SEQ_BEGIN_X, label = SEQ_STAR[i])
  if(i < length(SEQ_STAR))
  {
    #vertical lines
    spectrum <- spectrum + annotate('segment', x = LINE_BEGIN_X, xend = LINE_BEGIN_X, y = LINE_Y_BEGIN, yend = LINE_Y_END)
    #top frag line
    spectrum <- spectrum + annotate('segment', x = LINE_BEGIN_X, xend = TOP_FRAG_LINE_END_X, y = LINE_Y_END, yend = LINE_Y_END)
    #bottom frag line
    spectrum <- spectrum + annotate('segment', x = LINE_BEGIN_X, xend = BOTTOM_FRAG_LINE_END_X, y = LINE_Y_BEGIN, yend = LINE_Y_BEGIN)
    #B and Y ions
    spectrum <- spectrum + annotate('text', x = TOP_LABEL_BEGIN_X, y = TOP_LABEL_Y_LEVEL,
                                    label = Y_IONS[i], parse = TRUE, size = 3, color = 'red')
    spectrum <- spectrum + annotate('text', x = BOT_LABEL_BEGIN_X, y = BOT_LABEL_Y_LEVEL, 
                                    label = B_IONS[i], parse = TRUE, size = 3, color = 'blue')
  }
  SEQ_BEGIN_X = SEQ_BEGIN_X + SEQ_SPACE
  LINE_BEGIN_X = LINE_BEGIN_X + SEQ_SPACE
  TOP_FRAG_LINE_END_X = LINE_BEGIN_X + FRAG_LINE_LEN
  BOTTOM_FRAG_LINE_END_X = LINE_BEGIN_X - FRAG_LINE_LEN
  TOP_LABEL_BEGIN_X = TOP_LABEL_BEGIN_X + SEQ_SPACE
  BOT_LABEL_BEGIN_X = BOT_LABEL_BEGIN_X + SEQ_SPACE
}
  
