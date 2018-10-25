/*
 * Assignment 1 MASH
 * Joshua Atherton  |  Armoni Atherton
 * TCSS 422 Fall 2018
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXSTR 255
#define ARGCNT 6
#define FILE_NAME1 "rfile1.txt"
#define FILE_NAME2 "rfile2.txt"
#define FILE_NAME3 "rfile3.txt"

void parse(char ***data, char *cmd, char *filename); 
void run_commands(char * file, char ** cmd1, char ** cmd2, char ** cmd3);
void execute_command(char** cmd, char* filename, int file_num);
void print_command_results(char * filename);
/*
 * Driver takes in three commands and a file to perform those commands on.
 * Program runs the commands in a parallel fashion. 
 */
int main(int argc, char *argv[]) {

    char cmd1[MAXSTR], cmd2[MAXSTR], cmd3[MAXSTR], file[MAXSTR];

    // This will get command 1;
    printf("mash-1->");
    fgets(cmd1, MAXSTR, stdin);

    // This will get command 2;
    printf("mash-2->");
    fgets(cmd2, MAXSTR, stdin);

    // This will get command 3;
    printf("mash-3->");
    fgets(cmd3, MAXSTR, stdin);

    // This will get file
    printf("File->");
    fgets(file, MAXSTR, stdin);

    //parse each command arguments
    char ** args1;
    parse(&args1, cmd1, file);
    char ** args2;
    parse(&args2, cmd2, file);
    char ** args3;
    parse(&args3, cmd3, file);

    //run the commands on three different threads
    run_commands(file, args1, args2, args3);

    /**** example of execvp *****/ //remove at the end!
    // char * myargs[3];
    // myargs[0] = strdup("wc");
    // myargs[1] = strdup("t.t");
    // myargs[2] = NULL;
    // execvp(myargs[0], myargs);

    /***** remove all tempory files used to store data ******/
    remove(FILE_NAME1);
    remove(FILE_NAME2);
    remove(FILE_NAME3);

    return 0;
}

/*
 * Will parse the terminal inputed comands into char arrays.
 */
void parse(char ***data, char *cmd, char *filename) {

    *data = (char**) malloc(sizeof(char*) * ARGCNT);

    char* str = strtok(cmd, " ");
    int cnt = 0;
    while (str != NULL && cnt < ARGCNT) {
        // remove newline char if found in string
        for (int i = 0; *(str + i) != NULL; i++) 
            if (*(str + i) == '\n')
                *(str + i) = 0;

        *(*data + cnt) = str;
        str = strtok(NULL, " ");
        cnt++;
    }
    // add filename to end and null terminate it
    for (int i = 0; *(filename + i) != NULL; i++) 
            if (*(filename+ i) == '\n')
                *(filename + i) = 0;

    *(*data + cnt) = filename;
    *(*data + (cnt + 1)) = 0;
}

/*
 * Run three commands entered in parallel. 
 */
void run_commands(char* file, char** cmd1, char** cmd2, char** cmd3) {
       
    // printf("Starting forks pid(%d)\n", getpid());
    int status;
    pid_t p1, p2, p3;
    clock_t parent_start, p1_start, p2_start, p3_start;
    clock_t parent_end, p1_end, p2_end, p3_end;

    parent_start = clock();
    p1 = fork(); // parent starts fork 1
    if (p1 == 0) { 
        // do child 1 stuff
        // printf("--start p1 forks pid(%d)\n", getpid());
        p1_start = clock();
        execute_command(cmd1, FILE_NAME1, 1);
        
    } else if (p1 > 0) {
        // parent starts fork 2
        p2 = fork();
        if (p2 == 0) {
            // do child 2 stuff
            // printf("--start p2 forks pid(%d)\n", getpid());
            p2_start = clock();
            execute_command(cmd2, FILE_NAME2, 2);
        
        } else if (p2 > 0) {
            // parent starts fork 3
            p3 = fork();
            if (p3 == 0) {
                // do child 3 stuff
                // printf("--start p3 forks pid(%d)\n", getpid());
                p3_start = clock();
                execute_command(cmd3, FILE_NAME3, 3);

            } else if (p3 > 0) {
                // parent made three threads with fork
                // wait for children to finish
                waitpid(p1, &status, 0);
                printf("First process finished...\n");
                p1_end = clock();

                waitpid(p2, &status, 0);
                printf("Second process finished...\n");
                p2_end = clock();

                waitpid(p3, &status, 0);
                printf("Third process finished...\n");
                p3_end = clock();

                /********* Output results ************/
                // command  1 results
                print_command_results(FILE_NAME1);
                printf("Result took:%dms\n", 
                    ((int) (p1_end - p1_start)) / CLOCKS_PER_SEC );

                // command 2 results
                print_command_results(FILE_NAME2);
                printf("Result took:%dms\n", 
                    ((int) (p2_end - p2_start)) / CLOCKS_PER_SEC );

                // command 3 results
                print_command_results(FILE_NAME3);
                printf("Result took:%dms\n", 
                    ((int) (p3_end - p3_start)) / CLOCKS_PER_SEC );

                printf("Children process IDs: %d %d %d.\n", p1, p2, p3);
                parent_end = clock();
                printf("Total elapsed time:%dms\n", 
                    ((int) (parent_end - parent_start)) / CLOCKS_PER_SEC );
            }
        }
    }
    // printf("Ending pid(%d)\n", getpid());
}

/*
 * Execute a single command and write out to a file.
 */
void execute_command(char** cmd, char* filename, int file_num) {

    int new_file_handler;
    close(STDOUT_FILENO);
    new_file_handler = open(filename, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);

    /*** todo: format the command and output to file with 80 char spacing ****/
    printf("----- CMD %d: ", file_num); // 12 char long
    int count = 0, cmd_length = 0;
    while (*(cmd + count) != NULL) {
        cmd_length += strlen(*(cmd + count));
        printf("%s ", *(cmd + count) );
        count++;
    }

    /*** to print trailing dashes ******/
    for (int i = 0; i < 68 - cmd_length; i++) {
        printf("-");
    }
    printf("\n");
    
    // printf("\ncommand len: %d \n", cmd_length); //todo: remove
    // todo: why is this not outputting to file?
    if (execvp(cmd[0], cmd) == -1) {
        printf("[SHELL %d] STATUS CODE=-1\n", file_num);
    } 
}

/*
 * Read the results from the file and output the results to the console. 
 */
void print_command_results(char * filename) {
    // todo: get the file results and output
    FILE * temp_file = fopen(filename, "r");
    // printf("-----file results here -----\n");
    char line[MAXSTR];
    while ( fgets(line, MAXSTR, temp_file) != NULL) {
        printf("%s", line);
    }

    fclose(temp_file);
}


/**************
 * Todo:
 *  -remove the file from comand input
 *  -get thread timming working
 *  -three crashes with bad input?
 * 
 * 
 * ************/