
library(ggplot2)

Rcpp::sourceCpp('rscripts/src/cppFunctions.cpp')
Rcpp::sourceCpp('rscripts/src/spectrum.cpp')

dat <- read.csv("testFiles/testScan.tsv", stringsAsFactors = FALSE, sep = '\t', na.strings = NA, quote = "")
dat$includeLabel <- as.logical(dat$includeLabel)

SEQ <- "TVAAPSVFIFPPSDEQLK"
SEQ_STAR <- makeSeqVec(SEQ)
B_IONS <- makeIonLabelsExp("b", length(SEQ_STAR) - 1, 'f')
Y_IONS <- makeIonLabelsExp("y", length(SEQ_STAR) - 1, 'r')
Y_COLOR = "red"
B_COLOR = "blue"
BLANK_COLOR = 'grey35'
BY_LAB_OFFSET_Y = 5

MZ_RANGE <- max(dat$mz) - min(dat$mz)
SEQ_SPACE = MZ_RANGE/55

SEQ_X_LEN = SEQ_SPACE * length(SEQ_STAR)
SEQ_BEGIN_X = max(dat$mz) - SEQ_X_LEN

  
if(max(dat[dat$mz > (SEQ_BEGIN_X - 30),]$intensity) > 80)
{
  SEQ_Y_LEVLE = (max(dat[dat$mz > SEQ_BEGIN_X - 30,]$intensity) + 30)
} else {
  SEQ_Y_LEVLE = 100
}

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

y_labs <- c(0, 25, 50, 75, 100)

#r0 <- list(x = 1031.77, y = 103, w = 80, h = 6)
#r1 <- list(x = 946.405, y = 7.708, w = 50, h = 6)
#r2 <- list(x = 918.5136, y = 9.4521, w = 50, h = 6)

spectrum <- ggplot(data = dat, aes(x = mz, ymax = intensity, y = intensity, ymin = 0, color = ionType)) +
  geom_linerange() +
  #annotate('rect', xmin = 650.97 - 25, ymin = 100, xmax = 650.97 + 25, ymax = 106, color = 'grey') +
  #annotate('rect', xmin = r0$x - r0$w/2, ymin = r0$y - r0$h/2, xmax = r0$x + r0$w/2, ymax = r0$y + r0$h/2, color = 'grey', alpha = 0.2) +
  geom_text(data = subset(dat, dat$includeLabel == TRUE),
                  aes(label = mz, x = labelX, y = labelY),
                  parse = FALSE,
                  size = 3) +
  geom_text(data = subset(dat, dat$ionType == "b" | dat$ionType == 'y'),
            aes(label = formatedLabel, x = labelX, y = labelY + BY_LAB_OFFSET_Y),
            parse = TRUE,
            size = 3) +
  scale_y_continuous(breaks = y_labs) +
  scale_color_manual(values = c("b" = B_COLOR, "blank" = BLANK_COLOR, "y" = Y_COLOR)) +
  theme_bw() +
  theme(panel.grid = element_blank(),
        legend.position = "none") +
  ylab("Relative intensity")
  
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

spectrum

#ggsave('rplots/currentSpectrum.pdf', plot = spectrum, dpi = 600, width = 12, height = 4)

