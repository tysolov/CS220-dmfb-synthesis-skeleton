#!/usr/bin/env bash
printf '\e[6t'
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo "Generating graphs from .dot files."

for file in ${DIR}/Output/*.dot; do
    [ -e "$file" ] || continue
    name=${file##*/}
    base=${name%.dot}
    dot -Tpng ${file} -o ${DIR}/Sim/${base}.png
    echo "Output ${base}.png to Sim/"
done


#find ${DIR}/Output -type f -name "*.dot" -exec sh -c 'dot -Tpng "${0}" -o "${DIR}/Sim/%.*".png"' {} \;

#find ${DIR}/Output -type f -name "*.dot" | xargs dot -Tpng -O

exit;
