/*****************************************************************************
* Accepts the following arguments: the first argument is a full path to a file 
* (including filename) on the filesystem, referred to below as writefile; the 
* second argument is a text string which will be written within this file, 
* referred to below as writestr
* Exits with value 1 error and print statements if any of the arguments above 
* were not specified
* Creates a new file with name and path writefile with content writestr, 
* overwriting any existing file and creating the path if it doesn’t exist. Exits 
* with value 1 and error print statement if the file could not be created.
* 
* Author: Nihal Thirunakarasu
* University: University of Colorado Boulder
* Date: 1/29/2023
* Assignment 2
******************************************************************************/
#include<stdio.h>
#include<fcntl.h>
#include<syslog.h>

/*****************************************************************************
* This function is the entry point to the program. The first program is the
* file path and the second is the string that needs to be written to the file.
* The program will return an error code if the file path is invalid or if
* either the file path or the string is missing as parameters.
* The functions will return 1 if error else it will return 0.
******************************************************************************/
int main (int argc, char** argv)
{
    // Prints the number of arguments passed
    // printf("The number of arguments are: %d\n", argc);

    // Adds writer.c ahead of all the logs by default t is the program name
    // LOG_PERRORa lso logs error to stderr
    // Sets the facility to USER
    openlog("writer.c", LOG_PERROR, LOG_USER);

    // Checking if the parameters entered are valid
    if(argc!=3)
    {
        printf("\nError: Invalid number of paramters.\nEnter the file to write the string to as the first parameter (within double quotes if it is includes spaces) and the string that needs to be written as the second parameter (within double quotes if it is includes spaces)\n");
        // Syslog the error into the syslog file in /var/log
        syslog(LOG_ERR, "Invalid number of paramters");
        return 1;
    }

    // Storing the parameters into variables
    char * writefile = argv[1];
    char * writestr = argv[2];
    
    // Opening the file pointer and storing it in a variable    
    FILE * file_ptr = NULL;

    // The O_RDWR flag is to set the read and write privilige for the program
    // The O_CREAT flag is used to direct the program to create the file if the file is not present
    // The 0666 is to set the permissions of the file that is created
    //fd = fopen(writefile, O_RDWR|O_CREAT, 0666);

    // The file is to be opened with write permissions. The w+ is to denote that if the file is not present then we will create a file
    file_ptr = fopen(writefile, "w+");
    // If the file is pointed to NULL then the file open function failed
    if(file_ptr == NULL)
    {
        printf("\nError: Invalid file path or file doesnt exist. Please check the file path entered.\n");
        // Syslog the error into the syslog file in /var/log
        syslog(LOG_ERR, "Invalid file path or file doesnt exist");
        return 1;
    }

    // Printing the string into the file
    fprintf(file_ptr,"%s", writestr);

    // Clossing the file
    fclose(file_ptr);

    // Clossing the log
    closelog();

    // Returns 0 if success
    return 0;
}   

