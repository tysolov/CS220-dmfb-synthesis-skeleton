#!/usr/bin/env bash
#printf '\e[6t'
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo "Creating video from sim files"
ls -1 ${DIR}/Sim/ | sort -n > ${DIR}/Sim/file-list
awk -v prefix=${DIR}"/Sim/" '{print prefix $0}' ${DIR}/Sim/file-list > ${DIR}/Sim/file-list2
grep ".png" ${DIR}/Sim/file-list2 > ${DIR}/Sim/file-list3
mv ${DIR}/Sim/file-list3 ${DIR}/Sim/file-list2
if [ -e ${DIR}/Sim/file-list ]
then
    mencoder mf://@${DIR}/Sim/file-list2 -o ${DIR}/sim.avi -ovc lavc
else 
    echo "Failed"
fi

# exit;
