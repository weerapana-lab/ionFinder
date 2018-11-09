
#/bin/bash

rsync -vr -e 'ssh -p 22022' --exclude='.*' --exclude='*sourceCpp*' build.sh db include rscripts src mauraisa@pleiades.bc.edu:~/scripts/ms2_anotator/
