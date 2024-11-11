#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/stat.h>

//+
// File:    shell.c
//
// Pupose:  This program implements a simple shell program. It does not start
//      processes at this point in time. However, it will change directory
//      and list the contents of the current directory.
//
//      The commands are:
//         cd name -> change to directory name, print an error if the directory doesn't exist.
//                    If there is no parameter, then change to the home directory.
//         ls -> list the entries in the current directory.
//                If no arguments, then ignores entries starting with .
//                If -a then all entries
//         pwd -> print the current directory.
//         exit -> exit the shell (default exit value 0)
//              any argument must be numeric and is the exit value
//
//      if the command is not recognized an error is printed.
//-

#define CMD_BUFFSIZE 1024
#define MAXARGS 10

typedef int (*commandFunction)(char **args, int nargs);

struct cmdData {
    char *cmdName;
    commandFunction cmdFunc;
};


int splitCommandLine(char * commandBuffer, char* args[], int maxargs);
int doInternalCommand(char * args[], int nargs);
int doExternalCommand(char * args[], int nargs);
int doCd(char* args[], int nargs);
int doLs(char* args[], int nargs);
int doPwd(char* args[], int nargs);
int doExit(char* args[], int nargs);


//+
// Function:    main
//
// Purpose: The main function. Contains the read
//      eval print loop for the shell.
//
// Parameters:  (none)
//
// Returns: integer (exit status of shell)
//-

int main() {

    char commandBuffer[CMD_BUFFSIZE];
    // note the plus one, allows for an extra null
    char *args[MAXARGS+1];

    // print prompt.. fflush is needed because
    // stdout is line buffered, and won't
    // write to terminal until newline
    printf("%%> ");
    fflush(stdout);

    while(fgets(commandBuffer,CMD_BUFFSIZE,stdin) != NULL){
        //printf("%s",commandBuffer);

    // remove newline at end of buffer
    int cmdLen = strlen(commandBuffer);
    if (commandBuffer[cmdLen-1] == '\n'){
        commandBuffer[cmdLen-1] = '\0';
        cmdLen--;
            //printf("<%s>\n",commandBuffer);
    }

    // split command line into words.(Step 2)
    int nargs = splitCommandLine(commandBuffer, args, MAXARGS);
    // TODO

    // add a null to end of array (Step 2)

    // TODO


        // TODO: check if 1 or more args (Step 3)
        
        // TODO: if one or more args, call doInternalCommand  (Step 3)
        
        // TODO: if doInternalCommand returns 0, call doExternalCommand (Step 4)
        
        // TODO: if doExternalCommand returns 0, print error message (Step 3 & 4)
        // that the command was not found.
        if( nargs > 0) {
            if (doInternalCommand(args, nargs) == 0){
                if(doExternalCommand(args, nargs) == 0) {
                    printf("Command not found: %s\n", args[0]);
                }
            }
        }

    // print prompt
    printf("%%> ");
    fflush(stdout);
    }
    return 0;
}

////////////////////////////// String Handling (Step 1) ///////////////////////////////////

//+
// Function:    skipChar
//
// Purpose: This function skips over a given char in a string
//      For security, will not skip null chars.
//
// Parameters:
//    charPtr   Pointer to string
//    skip  character to skip
//
// Returns: Pointer to first character after skipped chars
//      ID function if the string doesn't start with skip,
//      or skip is the null character
//-

char * skipChar(char * charPtr, char skip){
    // TODO: contents of function
    while(*charPtr == skip && *charPtr != '\0'){
        charPtr++;
    }
    // TODO: replace null with proper value
    return charPtr;
}

//+
// Funtion: splitCommandLine
//
// Purpose: TODO: give descritption of function
//
// Parameters:
//  TODO: parametrs and purpose
//
// Returns: Number of arguments (< maxargs).
//
//-

int splitCommandLine(char * commandBuffer, char* args[], int maxargs){
   // TODO: Contents of function
    int nargs = 0;
    char *current = skipChar(commandBuffer, ' ');
    while(*current != '\0' && nargs < maxargs){
        args[nargs++] = current; //save start of word
        //move forward until hit a space or end of string
        current = strchr(current, ' ');
        if (current == NULL) {
            break; //reached end of command
        }
        *current = '\0'; //null terminate word
        current = skipChar(current + 1, ' '); //move to next word
    }
    args[nargs] = NULL;
   // TODO: reutrn proper value
   return nargs;
}

////////////////////////////// External Program  (Note this is step 4, complete doeInternalCommand first!!) ///////////////////////////////////

// list of directorys to check for command.
// terminated by null value
char * path[] = {
    ".",
    "/bin",
    "/usr/bin",
    NULL
};

//+
// Funtion: doExternalCommand
//
// Purpose: TODO: add description of function
//
// Parameters:
//  TODO: add paramters and description
//
// Returns  int
//      1 = found and executed the file
//      0 = could not find and execute the file
//-

int doExternalCommand(char * args[], int nargs){
  // find the executable
  // TODO: add body.
  // Note this is step 4, complete doInternalCommand first!!!
    char* path = getenv("PATH");
    if(path == NULL){
        fprintf(stderr, "error getting PATH\n");
        return 0;
    }
    char* pathDir = strtok(path, ":");
    struct stat statbuf;
    char* cmd_path = NULL;
    while ( pathDir != NULL){
        int len = strlen(pathDir) + strlen(args[0]) + 2;
        cmd_path = (char*)malloc(len);
        if (cmd_path == NULL){
            fprintf(stderr, "error allocating memory\n");
            return 0;
        }
        sprintf(cmd_path, "%s/%s", pathDir, args[0]); //create full path ex /usr/bin/ls
        if (stat(cmd_path, &statbuf) == 0 && S_ISREG(statbuf.st_mode) && (statbuf.st_mode & S_IXUSR)) {
            break; //found the file exit the loop
        }

        free(cmd_path);
        cmd_path = NULL;
        pathDir = strtok(NULL, ":");
    }
    if (cmd_path == NULL){
        return 0; //failure to find the command
    }
    //fork and exec
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "error forking\n");
        free(cmd_path);
        return 0;
    }

    if (pid == 0) {
        //child
        execv(cmd_path, args);
        fprintf(stderr, "error executing command\n");
        free(cmd_path);
        exit(EXIT_FAILURE);
    }
    free(cmd_path);
    wait(NULL);
  return 1;
}

////////////////////////////// Internal Command Handling (Step 3) ///////////////////////////////////

// define command handling function pointer type


// prototypes for command handling functions
// TODO: add prototype for each comamand function
// list commands and functions
// must be terminated by {NULL, NULL} 
// in a real shell, this would be a hashtable.

struct cmdData commands[] = {
    {"cd", doCd},
    {"ls", doLs},
    {"pwd", doPwd},
    {"exit", doExit},
    {NULL, NULL} // terminator
};

//+
// Function:    doInternalCommand
//
// Purpose: TODO: add description
//
// Parameters:
//  TODO: add parameter names and descriptions
//
// Returns  int
//      1 = args[0] is an internal command
//      0 = args[0] is not an internal command
//-

int doInternalCommand(char * args[], int nargs){
    // TODO: function contents (step 3)
    for (int i = 0; commands[i].cmdName != NULL; i++){
        if (strcmp(args[0], commands[i].cmdName) == 0){
            return commands[i].cmdFunc(args, nargs);
        }
    }
    return 0; //not found
}

///////////////////////////////
// comand Handling Functions //
///////////////////////////////

// TODO: a function for each command handling function
// goes here. Also make sure a comment block prefaces
// each of the command handling functions.



/*
 * Function: doExit
 * --------------------
 * This function implements the exit command.
 *
 * Workflow:
 *    - The function calls exit(0) to terminate the program with a status code of 0,
 *      indicating a successful execution.
 *
 * Return:
 *    - This function does not return, as the program is terminated upon execution.
 */

int doExit(char **args, int nargs){
    int exitCode = 0;
    if (nargs > 1) {
        exitCode = atoi(args[1]);
    }
    exit(exitCode);
    //exit with status code 0
}
/*
 * Function: doPwd
 * --------------------
 * This function implements the print working directory (pwd) command.
 * It retrieves and prints the current working directory to the standard output.
 *
 * Workflow:
 *    - The function uses getcwd() to retrieve the current working directory.
 *    - If successful, it prints the directory path followed by a newline.
 *    - The memory allocated by getcwd() is then freed.
 *    - If the current directory cannot be retrieved, an error message is printed to stderr.
 *
 * Return:
 *    - Returns 1 if the directory was successfully printed.
 *    - Returns -1 if there was an error in retrieving the current directory.
 */

int doPwd(char **args, int nargs){
    char *cwd = getcwd(NULL, 0);
    if(cwd != NULL) {
        printf("%s\n", cwd);
        free(cwd);
        return 1;
    } else {
        fprintf(stderr, "error retrieving current directory\n");
        return -1;
    }
}

/*
 * Function: doCd
 * --------------------
 * This function implements the change directory command.
 *
 * Parameters:
 *    args[]: An array of command-line arguments, where args[1] specify the target directory.
 *    nargs: The number of arguments passed (should be either 1 or 2).
 *
 * Workflow:
 *    - If only 1 argument (the "cd" command) is passed, the function changes the current working
 *      directory to the user's home directory.
 *    - The home directory is retrieved using `getpwuid(getuid())`.
 *    - If 2 arguments are passed, it changes the directory to the path specified in args[1].
 *    - If more than 2 arguments are passed, an error message is printed, indicating too many arguments.
 *
 * Error Handling:
 *    - If the home directory cannot be retrieved, or if the directory change fails, an error
 *      message is printed to stderr.
 *
 * Return:
 *    - Returns 1 if the directory was successfully changed.
 *    - Returns -1 if there was an error (e.g., failure to retrieve the home directory, invalid
 *      arguments, or failure to change the directory).
 */


int doCd(char** args, int nargs) {
    if(nargs == 1){
        struct passwd *pw = getpwuid(getuid());
        if(pw == NULL){
            fprintf(stderr, "error getting home directory");
            return -1;
        }
        if(chdir(pw->pw_dir) != 0){
            fprintf(stderr, "error changing directory");
            return -1;
        }
    } else if ( nargs == 2) {
        if (chdir(args[1]) != 0){
            fprintf(stderr, "error changing directory");
            return -1;
        }
    } else {
        fprintf(stderr, "cd: too many arguments\n");
        return -1;
    }
    return 1;
}

/*
 * Function: nonHiddenFilter
 * --------------------
 * This function acts as a filter to exclude hidden files (those whose names start with a dot)
 * from directory listings.
 *
 * Parameters:
 *    entry: A pointer to a struct dirent representing a directory entry.
 *
 * Workflow:
 *    - The function checks if the first character of the directory entry's name (`d_name[0]`) is a dot ('.').
 *    - If the name starts with a dot, the function returns false (0), indicating that this file should be excluded.
 *    - Otherwise, it returns true (non-zero), allowing non-hidden files to pass through the filter.
 *
 * Return:
 *    - Returns 1 (true) if the file is not hidden (doesn't start with a dot).
 *    - Returns 0 (false) if the file is hidden (starts with a dot).
 */


int nonHiddenFilter(const struct dirent *entry){
    //return false if the file name starts with a dot
    return entry->d_name[0] != '.';
}

/*
 * Function: doLs
 * --------------------
 * This function implements the ls command, listing files in the current directory.
 * It can list either non-hidden files by default or all files if the "-a" option is provided.
 *
 * Parameters:
 *    args[]: An array of command-line arguments.
 *            - If no arguments are passed, it lists only non-hidden files.
 *            - If the "-a" argument is passed, it lists all files, including hidden ones.
 *    nargs: The number of arguments passed.
 *
 * Workflow:
 *    - If no arguments are given (nargs == 1), the function calls `scandir()` to list
 *      non-hidden files using the `nonHiddenFilter` and sorts them alphabetically.
 *    - If one argument "-a" is given (nargs == 2), it lists all files, including hidden ones,
 *      by calling `scandir()` without a filter and sorting alphabetically.
 *    - If the arguments are invalid (e.g., anything other than "ls" or "ls -a"), an error
 *      message is printed to stderr, and the function returns -1.
 *
 * Error Handling:
 *    - If `scandir()` fails to list the directory, an error message is printed to stderr, and
 *      the function returns -1.
 *
 * Output:
 *    - The function prints the name of each file in the current directory to standard output.
 *    - After printing, the memory allocated for each directory entry is freed.
 *
 * Return:
 *    - Returns 1 on successful listing and output of file names.
 *    - Returns -1 if there is an error in listing the directory or if invalid arguments are passed.
 */


int doLs(char **args, int nargs){
    struct dirent **namelist;
    int numEnts;
    if(nargs == 1) {
        //no arg, list non-hidden files
        numEnts = scandir(".", &namelist, nonHiddenFilter, alphasort);
    } else if (nargs == 2 && strcmp(args[1] , "-a") == 0) {
        //list all files
        numEnts = scandir(".", &namelist, NULL, alphasort);
    } else {
        fprintf(stderr, "usage: ls [-a]\n");
        return -1;
    }
    if(numEnts < 0){
        fprintf(stderr, "error listing directory\n");
        return -1;
    }
    //printing file names
    for(int i = 0; i < numEnts; i++){
        printf("%s\n", namelist[i]->d_name);
        free(namelist[i]); //free memory
    }
    free(namelist); //free the array
    return 1;
}
