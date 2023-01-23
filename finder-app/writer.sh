#!/bin/bash

#############################################################################
# Write a shell script finder-app/writer.sh as described below
#
#    Accepts the following arguments: the first argument is a full path to a file (including filename) on the filesystem, referred to below as writefile; the second argument is a text string which will be written within this file, referred to below as writestr
#
#    Exits with value 1 error and print statements if any of the arguments above were not specified
#
#    Creates a new file with name and path writefile with content writestr, overwriting any existing file and creating the path if it doesn’t exist. Exits with value 1 and error print statement if the file could not be created.
#############################################################################


# Passing two paramters. The first one is the file to write to and the second is the string to write
writefile=$1
writestr=$2

# Checking if the correct number of paramters were passed by the user
if [ $# -ne 2 ]
then
    echo "Error: Enter 2 paramters. First parameter with the file and the second paramter with the string to write"
    exit 1
fi

# Checking if the path mentioned is valid
if [ -d "${writefile}" ]
then 
    # Checking if the file path is a directory
    echo "Error: File Path is directory "
    exit 1
else
    if [ -f "${writefile}" ]
    then   
        # Checking if the file path mentioned is present
        :
    else
        # Checking if the file path is not present and creating it 
        #Creates all the directories in the file path mentioned
        mkdir -p "$(dirname "${writefile}")" # Removing the file name from the file path
        #Creates the file
        touch "${writefile}" # Creating the file in the file path
    fi
fi

# Writing the string into the file
echo "${writestr}" > ${writefile}