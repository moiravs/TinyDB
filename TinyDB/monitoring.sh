#!/bin/bash
set -m
#Queries_file=""
#file=""
proc="tinydb"
if [ "${1}" == "run" ]; then
    for (( args=1; args<=$#; args++));
    do
        j=$((args+1))
        case "${!args}" in
            -f) Queries_file="${!j}";
            if [ -z "$Queries_file" ]; # if no queries file specified after -f
            then 
                echo "-f requires an argument"
                exit 1
            fi
            if [[ ! -f "$Queries_file" ]] # if queries file doesn't exist
            then    
                >&2 echo "Queries file doesn't exist"
                exit 1
            fi
        esac    
    done
    file="${2:-data/students.bin}" # default file if run without any arguments
    if [ "${file}" == "-f" ]; # if second argument is -f
    then
        file="data/students.bin"
    fi

    if [[ ! -f "${file}" ]] # if file.bin doesn't exist 
    then 
        >&2 echo "File created, file didn't exist"
        touch $file
    fi

    echo "Queries File: $Queries_file";
    echo "File : $file";
    if [ "${Queries_file}" != "" ]
    then
        ./tinydb "$file" < "$Queries_file" &
    else
        ./tinydb "$file"
fi


elif [ "${1}" == "sync" ]; then
    i=0;
    pids=$(pgrep "$proc") # list of processes
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
    pids=$(pgrep "$proc")
    if [[ -n $pids ]]; then
        printf "%s\n\nThere are currently %d processes running under the application '%s'\n" \
            "$pids" "$(wc -l <<< "$pids")" "$proc"
    else
        echo "No tinydb process running"
    fi

elif [ "${1}" == "shutdown" ]; then
    if [ $# -eq 1 ]; then
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
        pids=$(pgrep "$proc")
        for pid in $pids
        do
        parent=$(pstree "$pid");
        if [[ $parent != "tinydb" ]] && [ $pid = $2 ]; then # if main process
            echo "Shutdown $pid ...";
            kill -INT $pid;
        elif [[ $parent == "tinydb" ]] && [ $pid = $2 ]; then # if not main process
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