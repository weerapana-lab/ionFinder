
PROG_WD_HOME = paste(Sys.getenv("HOME"), 
                  "/Xcode_projects/ms2_anotator/rscripts", sep = "")
source(paste(PROG_WD_HOME, "/makeSpectrum.R", sep = ""), echo=FALSE)

argv <- commandArgs(trailingOnly = TRUE)
argc <- length(argv)

if(argc[1] == "RStudio")
{
  argv = c("")
}

for(arg in argv)
{
  
}



#ggsave('rplots/currentSpectrum.pdf', plot = spectrum, dpi = 600, width = 12, height = 4)
