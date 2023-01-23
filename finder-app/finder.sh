#!/bin/bash

#############################################################################
# Write a shell script finder-app/finder.sh as described below:
#
#    Accepts the following runtime arguments: the first argument is a path to a directory on the filesystem, referred to below as filesdir; the second argument is a text string which will be searched within these files, referred to below as searchstr
#
#    Exits with return value 1 error and print statements if any of the parameters above were not specified
#
#    Exits with return value 1 error and print statements if filesdir does not represent a directory on the filesystem
#
#    Prints a message "The number of files are X and the number of matching lines are Y" where X is the number of files in the directory and all subdirectories and Y is the number of matching lines found in respective files.
#############################################################################


# Passing two paramters. The first one is the file path and the second is the string to search
filesdir=$1
searchstr=$2

# Checking if the correct number of paramters were passed by the user
if [ $# -ne 2 ]
then
    echo "Error: Enter 2 paramters. First parameter with the path and the second paramter with the string to search"
    exit 1
fi

#Checking if the path mentioned is valid
if [ -d "${filesdir}" ]
then 
    :
else
    if [ -f "${filesdir}" ]
    then   
        :
    else
        echo "Error: File Path is invalid"
        exit 1
    fi
fi

#Checking the number of files the searchstr is found
num_files="`grep -l -r ${searchstr} ${filesdir} | wc -l`"

#Checking the number of lines the searchstr is found
num_lines=$(grep -n -r ${searchstr} ${filesdir} | wc -l)

#Checking the number of files the searchstr is found
num_instances="`grep -o -i -r ${searchstr} ${filesdir} | wc -l`"

echo "The number of files are ${num_files} and the number of matching lines are ${num_lines}"
