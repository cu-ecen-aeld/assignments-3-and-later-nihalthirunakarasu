#include<stdio.h>
#include<fcntl.h>
#include<syslog.h>

int main (int argc, char** argv)
{
    // Prints the number of arguments passed
    // printf("The number of arguments are: %d\n", argc);

    // Adds writer.c ahead of all the logs by default t is the program name
    // LOG_PERRORalso logs error to stderr
    // Sets the facility to USER
    openlog("writer.c", LOG_PERROR, LOG_USER);

    if(argc!=3)
    {
        printf("\nError: Invalid number of paramters.\nEnter the file to write the string to as the first parameter (within double quotes if it is includes spaces) and the string that needs to be written as the second parameter (within double quotes if it is includes spaces)\n");
        syslog(LOG_ERR, "Invalid number of paramters");
        return 1;
    }

    char * writefile = argv[1];
    char * writestr = argv[2];
    
    int fd;
    FILE * file_ptr = NULL;

    // The O_RDWR flag is to set the read and write privilige for the program
    // The O_CREAT flag is used to direct the program to create the file if the file is not present
    // The 0666 is to set the permissions of the file that is created
    //fd = fopen(writefile, O_RDWR|O_CREAT, 0666);
    file_ptr = fopen(writefile, "w+");
    if(file_ptr == NULL)
    {
        printf("\nError: Invalid file path or file doesnt exist. Please check the file path entered.\n");
        syslog(LOG_ERR, "Invalid file path or file doesnt exist");
        return 1;
    }

    fprintf(file_ptr,"%s", writestr);

    fclose(file_ptr);

    closelog();

    return 0;
}   

