#!/bin/bash

# Passing two paramters. The first one is the file to write to and the second is the string to write
writefile=$1
writestr=$2

# Checking if the correct number of paramters were passed by the user
if [ $# -ne 2 ]
then
    echo "Error: Enter 2 paramters. First parameter with the file and the second paramter with the string to write"
    exit 1
fi

## Printing the parameters passed by the user
#echo "writefile = ${writefile}"
#echo "writestr =  ${writestr}"

#Checking if the path mentioned is valid
if [ -d "${writefile}" ]
then 
    echo "Error: File Path is directory "
    exit 1
else
    if [ -f "${writefile}" ]
    then   
        :
    else
        #Creates all the directories in the file path mentioned
        mkdir -p "$(dirname "${writefile}")"
        #Creates the file
        touch "${writefile}"
    fi
fi

echo "${writestr}" > ${writefile}