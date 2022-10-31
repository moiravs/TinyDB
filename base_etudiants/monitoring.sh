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
    if [ "${file}" == "-f" ]; 
    then
        file="students.bin"
    fi
    if [[ ! -f "${file}" ]]
    then 
        echo "${1}" 1>&2
        touch $file
    fi
    echo "Fichiers requêtes: $fichier_requetes";
    echo "File : $file";
    echo "whit";
    ./tinydb "$file" & echo $!;
    echo "what";
    pids = $(pgrep <tinydb>);
    echo "pids : $pids";
fi




if [ "${1}" == "sync" ]
then
    i=0;
    proc="tinydb"
    pids=$(pgrep "$proc")
    echo "$pids";
    for pid in $pids
    do
        res=$((i%5));
        if [[ $res = 0 ]];
        then
            echo "Sync process $pid ...";
            kill -USR1 $pid;
        fi
        ((i++));
    done

elif [ "${1}" == "status" ]
then
    echo "status"
    proc="tinydb"
    pids=$(pgrep "$proc")
    if [[ -n $pids ]]; then
        printf "%s\n\nThere are currently %d processes running under the application '%s'\n" \
            "$pids" "$(wc -l <<< "$pids")" "$proc"
    fi

elif [ "${1}" == "shutdown" ]
then
    if [ $# -eq 1 ]
    then
        proc="tinydb"
        pids=$(pgrep "$proc")
        for pid in $pids
        do
            echo $pid
            read -p "Continue? (Y/N): " confirm && [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]] || kill -SIGKILL $pid   
        done

    else
        echo An error message > /dev/stderr
        kill "${2}"
    fi


else 
    echo "Wrong parameters"
fi

