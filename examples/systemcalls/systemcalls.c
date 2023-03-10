#include "systemcalls.h"
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>


/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    int res = 0;

    res = system(cmd);
    
    if(-1 == res)
    {   
        printf("\nError: system() returned error. Error code: %d", errno); 
        return false;
    }

    return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/

    int status_int;

    // PID to store the child PID
    pid_t child;   

    // Checking if the command path is absolute
    if(command[0][0] != '/')
    {
        printf("\nError: The file path entered is '%s' and is not absolute", command[0]);
        return false;
    }

    // Stores the return in the child PID variable
    child = fork();

    if(-1 == child)
    {
        // Error handling the fork()
        printf("\nError: fork() returned error. Error code: %d", errno);
        return false;
    }
    else if (0 == child)
    {
        // Child executes this block
        status_int = execv(command[0], command);

        if (-1 == status_int)
        {
            printf("\nError: execv() returned error. Error code: %d", errno);
            return false;
        }
    }
    else
    {
        // Parent executes this block
        pid_t status_pid = waitpid(child, &status_int, 0);

        if (-1 == status_pid)
        {
            printf("\nError: waitpid() returned error. Error code: %d", errno);
            return false;
        }
        // Checking id exited normally
        else if(WIFEXITED(status_int))
        {
            // Evaluates to the lower order of 8 bits of status if the child is passed though exit() or main()
            if(0 != WEXITSTATUS(status_int))
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        return false;
    }


    va_end(args);

    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
    int status_int;
    int re_dir_fd;

    // PID to store the child PID
    pid_t child;

    // Checking if the command path is absolute
    if(command[0][0] != '/')
    {
        printf("\nError: The file path entered is '%s' and is not absolute", command[0]);
        return false;
    }

    // Open with truncate and write only permissions. Create if file is not present
    // Create file with RW--W--W- permissions
    re_dir_fd = open(outputfile, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if(-1 == re_dir_fd)
    {
        // Error handling the open()
        printf("\nError: open() returned error. Error code: %d", errno);
        return false;
    }


    // Stores the return in the child PID variable
    child = fork();
    if(-1 == child)
    {
        // Error handling the fork()
        printf("\nError: fork() returned error. Error code: %d", errno);
        return false;
    }
    else if (0 == child)
    {
        // Child executes this block
        
        //Before running the code we will redirect the stdout (FD_1 to the new file)
        status_int = dup2(re_dir_fd, 1);
        if(-1 == status_int)
        {
            // Error handling the dup2()
            printf("\nError: dup2() returned error. Error code: %d", errno);
            return false;
        }

        status_int = close(re_dir_fd);
        if(-1 == status_int)
        {
            // Error handling the close()
            printf("\nError: close() returned error. Error code: %d", errno);
            return false;
        }

        status_int = execv(command[0], command);

        if (-1 == status_int)
        {
            printf("\nError: execv() returned error. Error code: %d", errno);
            return false;
        }
    }
    else
    {
        // Parent executes this block
        pid_t status_pid = waitpid(child, &status_int, 0);

        if (-1 == status_pid)
        {
            printf("\nError: waitpid() returned error. Error code: %d", errno);
            return false;
        }
        // Checking id exited normally
        else if(WIFEXITED(status_int))
        {
            // Evaluates to the lower order of 8 bits of status if the child is passed though exit() or main()
            if(0 != WEXITSTATUS(status_int))
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        return false;
    }


    va_end(args);

    return true;
}
