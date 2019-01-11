
BIN_DIR="bin"
PYTHON_EXE=$(which python2.7)


#returns the absolute path of a relative reference
function absPath {
	if [ -d "$1" ]; then
		( cd "$1"; echo $(dirs -l +0))
	else
		( cd "$(dirname "$1")"; d=$(dirs -l +0); echo "${d%/}/${1##*/}" )
	fi
}

moduleName=$(basename -s ".py" $1)
ofname=$BIN_DIR/$moduleName
dirname=$(absPath $(dirname $1))
echo "Adding "$ofname
echo "#!"$PYTHON_EXE > $ofname
echo -e "\nimport sys" >> $ofname
echo "sys.path.insert(0,\"$dirname\")" >> $ofname
echo "import $moduleName" >> $ofname
echo -e "\n$moduleName.main()" >> $ofname

chmod +x $ofname
