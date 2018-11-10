
TEMP_PS="temp.ps"

groff -mandoc > $TEMP_PS `man -w db/helpFile.man`
ps2pdf $TEMP_PS helpFile.pdf
rm $TEMP_PS
