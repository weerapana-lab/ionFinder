#!/bin/bash
 
BIN_DIR="bin"
PYTHON_EXE=$(which python)
python_script_path=""
PYTHON_MAIN_ARGS=""

function usage {
    echo -e  "usage: addPythonFiles [--binDir <path>] [--pythonExe <path>] [--mainArgs <args>] <python_script_path>"
    exit
}

#checks that $1 is not empty or a flag
function isArg {
	if [[ $1 == -* ]] || [[ -z "$1" ]] ; then
		usage
	fi
}

#returns the absolute path of a relative reference
function absPath {
	if [ -d "$1" ]; then
		( cd "$1"; echo $(dirs -l +0))
	else
		( cd "$(dirname "$1")"; d=$(dirs -l +0); echo "${d%/}/${1##*/}" )
	fi
}

#get args
while ! [[ -z "$1" ]] ; do
	case $1 in
		"--pythonExe" | "-p")
			shift
			isArg "$1"
			PYTHON_EXE=$(absPath "$1");;
		"--mainArgs" | "-a")
			shift
			isArg "$1"
			PYTHON_MAIN_ARGS="$1";;
		"--binDir")
			shift
			isArg "$1"
			BIN_DIR="$1";;
		"-h")
			usage ;;
		*)
            if [[ "$1" == -* ]] ; then
               usage 
            else
                python_script_path=$( absPath "$1" )
            fi
            shift
	esac
	shift
done

if [[ -z $python_script_path ]] ; then
	usage
fi

mkdir -p "$BIN_DIR"
moduleName=$(basename "$python_script_path" .py) 
ofname=$BIN_DIR/$moduleName
dirname=$(absPath $(dirname "$python_script_path"))
echo "Adding "$ofname
echo "#!"$PYTHON_EXE > $ofname
echo -e "\nimport sys" >> $ofname
echo "sys.path.insert(0, '$dirname')" >> $ofname
echo "import $moduleName" >> $ofname
if [[ -z "$PYTHON_MAIN_ARGS" ]] ; then		
	echo -e "\n$moduleName.main()" >> $ofname
else
	echo -e "\n$moduleName.main('$PYTHON_MAIN_ARGS')" >> $ofname
fi

chmod +x $ofname
