
TEMP_PS="temp.ps"

groff -mandoc > $TEMP_PS `man -w $1`
ps2pdf $TEMP_PS $2
rm $TEMP_PS
