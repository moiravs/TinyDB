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
        file="data/students.bin"
    fi
    if [[ ! -f "${file}" ]]
    then 
        echo "${1}" 1>&2
        touch $file
    fi
    echo "Fichiers requêtes: $fichier_requetes";
    echo "File : $file";
    ./tinydb "$file"

elif [ "${1}" == "sync" ]; then
    i=0;
    proc="tinydb"
    pids=$(pgrep "$proc")
    echo "$pids";
    for pid in $pids
    do
        parent=$(pstree "$pid");
        if [[ $parent != "tinydb" ]]; then
            echo "Sync process $pid ...";
            kill -USR1 $pid;
        fi
        ((i++));
    done

elif [ "${1}" == "status" ]; then

    proc="tinydb"
    pids=$(pgrep "$proc")
    if [[ -n $pids ]]; then
        printf "%s\n\nThere are currently %d processes running under the application '%s'\n" \
            "$pids" "$(wc -l <<< "$pids")" "$proc"
    else
        echo "No tinydb process running"
    fi

elif [ "${1}" == "shutdown" ]; then
    if [ $# -eq 1 ]; then
        proc="tinydb"
        pids=$(pgrep "$proc")
        for pid in $pids
        do
            echo "/n"
            echo $pid
            parent=$(pstree "$pid");
            read -p "Kill Pid? (Y/N): " confirm && [[ $confirm == [nN] || $confirm == [nN][oO] ]] || kill -SIGKILL $pid   
        done

    else
        i=0;
        proc="tinydb"
        pids=$(pgrep "$proc")
        for pid in $pids
        do
        parent=$(pstree "$pid");
        if [[ $parent != "tinydb" ]] && [ $pid = $2 ]; then
            echo "Shutdown $pid ...";
            kill -INT $pid;
        elif [[ $parent == "tinydb" ]] && [ $pid = $2 ]; then 
            echo This is not the main process, choose another pid > /dev/stderr;
        fi
        ((i++));
        done
    fi


elif [ "${1}" == "-h" ] || [ "${1}" == "--help" ] ; then 
    echo "List of commands to launch the program"
    echo -e " \t " "./tinydb <path_to_database> < [<queries_file>]"
    echo -e " \t " "./monitoring run [<path_to_database>] [-f <queries_file>]"
    echo 
    echo "List of query commands"
    echo -e " \t " "select <field>=<value> -> Selects all the students whose field and value correspond to the query."
    echo -e " \t " "insert <fname> <lname> <id> <section> <birthdate> -> Inserts a new student."
    echo -e " \t " "delete <field>=<value> -> Deletes all the students whose field and value correspond to the query."
    echo -e " \t " "update <field>=<value> set <modified_field>=<modified_value> -> Updates the <modified_field> of all students whose field and value correspond to the query."
    echo -e " \t " "transaction"
    echo "List of monitoring commands"
    echo -e " \t " "./monitoring status -> Prints out the PIDs of all the running databases."
    echo -e " \t " "./monitoring sync -> Sends a signal to all the program's instances to save data on the disc."
    echo -e " \t " "./monitoring shutdown [<pid>] -> If the pid is the main process, then the command stops it. If no pid is given, the program asks for confirmation for every main process running."

else 
    echo "Unknown parameter"
    echo "Use --help or -h for more information"
fi