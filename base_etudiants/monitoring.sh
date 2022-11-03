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
    ./tinydb "$file" & echo $!;

elif [ "${1}" == "sync" ]; then
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
            echo $pid
            read -p "Continue? (Y/N): " confirm && [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]] || kill -SIGKILL $pid   
            parentpid=$(ps -o ppid= $pid)
            kill -SIGCHLD $parentpid
        done

    else
        i=0;
        proc="tinydb"
        pids=$(pgrep "$proc")
        for pid in $pids
        do
        res=$((i%5));
        if [ $res = 0 ] && [ $pid = $2 ]; then
            echo "Shutdown $pid ...";
            kill -USR1 $pid;
        elif [ $res != 0 ] && [ $pid = $2 ]; then 
            echo This is not the main process, choose another pid > /dev/stderr;
        fi
        ((i++));
        done
    fi


elif [ "${1}" == "help" ]; then 
    echo "List of commands for launching the program"
    echo -e " \t " "./tinydb <path_to_database> < [<queries_file>]"
    echo -e " \t " "./monitoring run [<path_to_database>] [-f <queries_file>]"
    echo 
    echo "List of query commands"
    echo -e " \t " "select <field>=<value> -> selects all the students whose field and value correspond to the query"
    echo -e " \t " "insert <fname> <lname> <id> <section> <birthdate> -> inserts a new student"
    echo -e " \t " "delete <field>=<value> -> deletes all the students whose field and value correspond to the query"
    echo -e " \t " "update <field>=<value> set <modified_field>=<modified_value> -> updates the <modified_field> of all students whose field and value correspond to the query"
    echo -e " \t " "transaction"

else 
    echo "Unknown parameter"
fi