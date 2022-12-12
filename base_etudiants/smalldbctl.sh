#!/bin/bash
set -m
#remember to use
#chmod u+x smalldbctl.sh
#chmod 744 smalldbctl.sh
#in terminal
#syntax is : smalldbctl {1 argument}
#Projet : SmallDB
#Section : B-INFO2
#Cours : INFO-F-201
#Auteurs :
#   Milan Skaleric #547963
#   Andrius Ezerskis #542698
#   Moira Vanderslagmolen #547486

list () {
#This function returns a list of every IP that is connected to the smalldb program
(ss --no-header -Ontp4 'sport = :28772' | grep "smalldb") > temp.txt 
filename='temp.txt'
var=0
n=1   
while read line; do
    for word in $line; do
        var=$((var+1))
        if [[ "$var" -eq 5 ]]
            then
            echo "Connected IP adress: '$word'"
            fi
            done
    var=0
    
done < $filename
rm temp.txt
}
sync(){
#This function sends a SIGUSR1 signal to the smalldb program
i=0;
    proc="smalldb"
    pids=$(pgrep -x "$proc")
    echo "$pids";
    for pid in $pids
        do
        parent=$(pstree "$pid");
        if [[ $parent != "smalldb" ]]; then
            echo "Sync process $pid ...";
            kill -USR1 $pid;
        fi
        ((i++));
    done
}
stop(){
#this function sends a SIGINT signal to the smalldb program
pid=`pgrep -x smalldb`
echo "$pid"
kill -SIGINT "$pid"

}
$1

