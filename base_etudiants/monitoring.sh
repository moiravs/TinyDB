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
    echo "Fichiers requetes: $fichier_requetes";
    echo "File : $file";
    ./tinydb "$file"
fi


if [ "${1}" == "sync" ]
then
    echo "sync"
elif [ "${1}" == "status" ]
then
    process_id= pidof tinydb
    echo "status"
    echo "${process_id}"

elif [ "${1}" == "shutdown" ]
then
    if [ $# -eq 1 ]
    then
        echo "No arguments provided"
        exit 1
    else
        kill "${2}"
    fi

else 
    echo "Wrong parameters"
fi

