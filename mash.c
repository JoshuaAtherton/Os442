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
#include <math.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define MAXSTR 255
#define ARGCNT 6
#define FILE_NAME1 "rfile1.txt"
#define FILE_NAME2 "rfile2.txt"
#define FILE_NAME3 "rfile3.txt"
#define my_max(x, y) ((x) < (y)) ? (x) : (y)

typedef struct wall_times {
    struct timespec parent_start, p1_start, p2_start, p3_start;
    struct timespec parent_end, p1_end, p2_end, p3_end;
} Wall_times;

void parse(char ***data, char *cmd, char *filename); 
void run_commands(char * file, char ** cmd1, char ** cmd2, char ** cmd3,
                Wall_times* times);
void execute_command(char** cmd, char* filename, int file_num);
void print_command_results(char * filename);
char** stripped_file_name(char** cmd);
 
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

    Wall_times *times = (Wall_times*)mmap(NULL, sizeof(Wall_times), PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
    
    //run the commands on three different threads
    run_commands(file, args1, args2, args3, times);

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
        for (int i = 0; *(str + i) != 0; i++) 
            if (*(str + i) == '\n')
                *(str + i) = 0;

        *(*data + cnt) = str;
        str = strtok(NULL, " ");
        cnt++;
    }
    // add filename to end and null terminate it
    for (int i = 0; *(filename + i) != 0; i++) 
        if (*(filename + i) == '\n')
            *(filename + i) = 0;

    *(*data + cnt) = filename;
    *(*data + (cnt + 1)) = 0;
}

/*
 * Run the commands entered in parallel output the results 
 */
void run_commands(char* file, char** cmd1, char** cmd2, char** cmd3, Wall_times* times) {
       
    int status;
    pid_t p1, p2, p3;
    clock_gettime(CLOCK_MONOTONIC, &times->parent_start);
    p1 = fork(); // parent starts fork 1
    if (p1 == 0) { 
        // do child 1 stuff
        // sleep(5);
        clock_gettime(CLOCK_MONOTONIC, &times->p1_start);
        execute_command(cmd1, FILE_NAME1, 1);
        
    } else if (p1 > 0) {
        // parent starts fork 2
        p2 = fork();
        if (p2 == 0) {
            // do child 2 stuff
            // sleep(3);
            clock_gettime(CLOCK_MONOTONIC, &times->p2_start);
            execute_command(cmd2, FILE_NAME2, 2);
        
        } else if (p2 > 0) {
            // parent starts fork 3
            p3 = fork();
            if (p3 == 0) {
                // do child 3 stuff
                // sleep(1);
                clock_gettime(CLOCK_MONOTONIC, &times->p3_start);
                execute_command(cmd3, FILE_NAME3, 3);

            } else if (p3 > 0) {
                // parent made three threads with fork
                // wait for children to finish record individual times
                int c3 = 1, c2 = 1, c1 = 1;
                while (c3 || c2 || c1) {
                    if ((waitpid(p3, &status, WNOHANG)) == 0) {
                        clock_gettime(CLOCK_MONOTONIC, &times->p3_end);
                        // printf(" %%  p3 done %% \n");
                        c3 = 0;
                    }
                    if ((waitpid(p2, &status, WNOHANG)) == 0) {
                        clock_gettime(CLOCK_MONOTONIC, &times->p2_end);
                        // printf(" %%  p2 done %% \n");
                        c2 = 0;
                    }
                    if ((waitpid(p1, &status, WNOHANG)) == 0) {
                        clock_gettime(CLOCK_MONOTONIC, &times->p1_end);
                        // printf(" %%  p1 done %% \n");
                        c1 = 0;
                    }
                }
                // clock the parent run time
                // clock_gettime(CLOCK_MONOTONIC, &times->parent_end);

                waitpid(p1, &status, 0);
                printf("First process finished...\n");

                waitpid(p2, &status, 0);
                printf("Second process finished...\n");

                waitpid(p3, &status, 0);
                printf("Third process finished...\n");

                // clock the parent run time
                clock_gettime(CLOCK_MONOTONIC, &times->parent_end);

                /********* Calculate times ************/
                long parent_runtime, p1_runtime, p2_runtime, p3_runtime;
                p1_runtime = ((times->p1_end.tv_nsec - times->p1_start.tv_nsec)/1.0e6) + 
                             (times->p1_end.tv_sec - times->p1_start.tv_sec) * 1000;
                p2_runtime = ((times->p2_end.tv_nsec - times->p2_start.tv_nsec)/1.0e6) + 
                             (times->p2_end.tv_sec - times->p2_start.tv_sec) * 1000;
                p3_runtime = ((times->p3_end.tv_nsec - times->p3_start.tv_nsec)/1.0e6) + 
                             (times->p3_end.tv_sec - times->p3_start.tv_sec) * 1000;
                parent_runtime = ((times->parent_end.tv_nsec - times->parent_start.tv_nsec)/1.0e6) + 
                                 (times->parent_end.tv_sec - times->parent_start.tv_sec) * 1000;
                
                /********* Output results ************/
                // command  1 results
                print_command_results(FILE_NAME1);
                printf("Result took:%ldms\n", labs(p1_runtime));

                // command 2 results
                print_command_results(FILE_NAME2);
                printf("Result took:%ldms\n", labs(p2_runtime));

                // command 3 results
                print_command_results(FILE_NAME3);
                printf("Result took:%ldms\n", labs(p3_runtime));
                // print 80 char delimeter
                for(int i = 0; i < 80; i++) {
                    printf("-");
                }
                printf("\n");
                printf("Children process IDs: %d %d %d.\n", p1, p2, p3);
                printf("Total elapsed time:%ldms\n", labs(parent_runtime));
            }
        }
    }
}

/*
 * Execute a single command and write out to a file.
 */
void execute_command(char** cmd, char* filename, int file_num) {

    int new_file_handler;
    close(STDOUT_FILENO);
    new_file_handler = open(filename, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);

    /*** format the command and output to file with 80 char spacing  *********/
    printf("----- CMD %d: ", file_num); // 12 char long
    
    /*** new char** with the file name and extention removed from the end of char** cmd **/ 
    char** temp = stripped_file_name(cmd);

    /**** output the rest of the command arguments to the file  **************/
    int count = 0, cmd_length = 0;
    while (*(temp + count) != NULL) {
        cmd_length += strlen(*(temp + count));
        cmd_length++;
        printf("%s ", *(temp + count) );
        count++;
    }

    /*** to print trailing dashes ******/
    for (int i = 0; i < 67 - cmd_length; i++) {
        printf("-");
    }
    printf("\n");
    
    if (execvp(cmd[0], cmd) == -1) {
        printf("[SHELL %d] STATUS CODE=-1\n", file_num);
        exit(-1);
    } 
}

/*
 * Read the results from the file and output the results to the console. 
 */
void print_command_results(char * filename) {
    
    FILE * temp_file = fopen(filename, "r");

    char line[MAXSTR];
    while ( fgets(line, MAXSTR, temp_file) != NULL) {
        printf("%s", line);
    }

    fclose(temp_file);
}

/** 
 * Strip the last non-null value from a char** array.
 */
char** stripped_file_name(char** cmd) {

    char** temp = (char**) malloc(sizeof(char*) * ARGCNT);
    int i;
    for (i = 0; *(cmd + i) != 0; i++) {
        char* s = malloc(sizeof(char) * sizeof(*(cmd + i)));
        strcpy(s, *(cmd + i));
        *(temp + i) = s;
    }
    *(temp + (i - 1)) = 0; // replace the last value

    return temp;
}
