#!/bin/bash
    
if [ "${1}" == "run" ]; then
    for (( i=1; i<=$#; i++));
    do
        j=$((i+1))
        case "${!i}" in
            -f) fichier_requetes="${!j}";;
        esac    
    done
    file="${2:-students.bin}"
    if [ "$file" == "-f" ]; then
        file="students.bin"
    fi
fi

echo "Fichiers requetes: $fichier_requetes";
echo "File : $file";

./tinydb "$file"
