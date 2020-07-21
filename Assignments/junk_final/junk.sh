###############################################################################
# Author: Michael DelGaudio & Sunmin Lee
# Date: 05/21/2020
# Pledge: I pledge my honor that I have abided by the Stevens Honor System.
# Description: CS-392 Assignment 1 - Junk.sh, basic function of recycle bin
###############################################################################
#!/bin/bash

#the readonly keyword when setting up the variable name for the .junk directory
#some global variables
readonly tempJunk=~/.junk
readonly absoluteFilePath=$0
action="null"
i=0
declare -a listOfFiles

function run () {

#lets do work
if [ $action == "null" ]; then
    moveFiles
elif [ $action == "list" ]; then
#list contents of the temp
    list
elif [ $action == "help" ]; then
#help just console log
    consoleLogUsage
elif [ $action == "purge" ]; then
#actually delete stuff
    delete
fi
exit 0
}

#checks if there are files, we need to check if the file actually exists
#if it does exist -> move to the .junk dir
#if it doesn't -> display the warning
function moveFiles (){
if [ $fileCounter -ne 0 ]; then
        for file in ${listOfFiles[@]}; do
            if [ ! -d $file ] && [ ! -f $file ]; then
                printf "Warning: '$file' not found.\n" >&2
            else
                #make the dir if we dont have
                if [ ! -d "$tempJunk" ]; then
                    mkdir $tempJunk
                    
                fi 
                #find $tempJunk -type d -exec chmod 775 {} \;
                #find $tempJunk -type f -exec chmod 664 {} \;
                #move it to .junk dir
                mv $file $tempJunk
            fi
        done
    else
    #no files, just console log
    consoleLogUsage
    fi
}

#RESOURCE CITED: https://unix.stackexchange.com/questions/77127/rm-rf-all-files-and-all-hidden-files-without-error
#RESOURCE CITED: https://www.quora.com/How-can-I-remove-hidden-files-in-Linux 
#delete all hidden, dirs, or files found
function delete () {
    if [ ! -d "$tempJunk" ]; then
        mkdir $tempJunk
        
    fi 
    #find $tempJunk -type d -exec chmod 775 {} \;
    #find $tempJunk -type f -exec chmod 664 {} \;
    rm -rf $tempJunk/{,.[!.],..?}*
}

#list option
#RESOURCE CITED: https://stackoverflow.com/questions/3740152/how-do-i-change-permissions-for-a-folder-and-all-of-its-subfolders-and-files-in 
function list () {
    if [ ! -d "$tempJunk" ]; then
        mkdir $tempJunk
    fi 
    #find $tempJunk -type d -exec chmod 775 {} \;
    #find $tempJunk -type f -exec chmod 664 {} \;
    ls -lAF "$tempJunk"
}

#usage console log print
function consoleLogUsage (){
    cat <<EOF
Usage: $(basename "$0") [-hlp] [list of files]
   -h: Display help.
   -l: List junked files.
   -p: Purge all files.
   [list of files] with no other arguments to junk those files.
EOF
}


#returns length of array
function numberOfFiles (){
    fileCounter=${#listOfFiles[*]}
}

#make sure we dont have more than one option selected
function verifyFiles (){
    if [ $action !=  "null" ]; then
        printf "Error: Too many options enabled.\n" >&2
        consoleLogUsage
        exit 1
    fi
}

#we have multiple files so we need to now check we don't have
#multiple args 
function ensure (){
    if [ $fileCounter -gt 0 ]; then
        verifyFiles
    fi
}

#RESOURCE CITED: https://sookocheff.com/post/bash/parsing-bash-script-arguments-with-shopts/
#RESOURCE CITED: https://askubuntu.com/questions/385528/how-to-increment-a-variable-in-bash
#RESOURCE CITED: https://stackoverflow.com/questions/16483119/an-example-of-how-to-use-getopts-in-bash
#RESOURCE CITED: http://tldp.org/HOWTO/Bash-Prog-Intro-HOWTO.html 
#arg checker
while getopts ":hlp" choice; do
    case "${choice}" in
        l) verifyFiles
            action="list"
            ;;
        h) verifyFiles
            action="help"
            ;;
        p) verifyFiles
            action="purge"
            ;;
        \?) printf "Error: Unknown option '-$OPTARG'.\n" >&2
            consoleLogUsage
            exit 1
            ;;
    esac
done
shift "$((OPTIND-1))"
for file in $@; do
  listOfFiles[$i]=$file
  ((i+=1))
done
numberOfFiles
ensure
run