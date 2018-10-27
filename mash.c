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
#include <sys/mman.h>

#define MAXSTR 255
#define ARGCNT 6
#define FILE_NAME1 "rfile1.txt"
#define FILE_NAME2 "rfile2.txt"
#define FILE_NAME3 "rfile3.txt"

typedef struct Times {
    clock_t parent_start, p1_start, p2_start, p3_start;
    clock_t parent_end, p1_end, p2_end, p3_end;
} Times;

void parse(char ***data, char *cmd, char *filename); 
void run_commands(char * file, char ** cmd1, char ** cmd2, char ** cmd3,
                Times* times);
void execute_command(char** cmd, char* filename, int file_num);
void print_command_results(char * filename);
char** stripped_file_name(char** cmd);

void test_func(char *t, char* t2);
void test_struct(Times *t);
 
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

    Times *times = (Times*)mmap(NULL, sizeof(Times), PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);

    //run the commands on three different threads
    run_commands(file, args1, args2, args3, times);

    // testing allocating in memory on a shared heap /////
    
    // int size = 4096;
    // char* anon = (char*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
    // char* anon2 = (char*)mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
    // test_func(anon, anon2);
    // printf("main child: %s \n", anon);
    // printf("main parent: %s \n", anon2);

    /*** test mem allocation in function with forks **/
    Times *test = (Times*)mmap(NULL, sizeof(Times), PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
    // test->p1_end = clock();
    // printf("  test %lu   \n", test->p1_end);

    test_struct(test);
    printf("-----------main------------------\n");
    printf("in parent child time: %ld \n", (test->p1_end - test->p1_start));
    printf("in parent parent time: %ld \n", (test->parent_end - test->parent_start));

    /***** remove all tempory files used to store data ******/
    remove(FILE_NAME1);
    remove(FILE_NAME2);
    remove(FILE_NAME3);

    return 0;
}
//test char pointer
void test_func(char *t, char* t2) {
    pid_t p1;
    p1 = fork();
    if (p1 == 0) { // child
        printf("child: %d\n", getpid());
        strcpy(t, "child");
        printf("in child: %s \n", t);
    } else if (p1 > 0) {
        printf("parent: %d\n", getpid());
        strcpy(t2, "parent");
        printf("in parent: %s \n", t2);
    }
}
// test struct
void test_struct(Times *t) {
    pid_t p1;
    int status;
    printf("------------test_struct-----------------\n");
    t->parent_start = time(NULL);
    p1 = fork();
    if (p1 == 0) { // child  
        t->p1_start = time(NULL);
        sleep(2);
        printf("in child: %lu \n", t->p1_start);
        exit(0);
    } else if (p1 > 0) {
        waitpid(p1, &status, 0);
        t->p1_end = time(NULL);
        printf("in parent child time: %ld \n", (t->p1_end - t->p1_start));
        sleep(1);
         t->parent_end = time(NULL);
        printf("in parent parent time: %ld \n", (t->parent_end - t->parent_start));
    }
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
 * Run three commands entered in parallel. 
 */
void run_commands(char* file, char** cmd1, char** cmd2, char** cmd3, Times* times) {
       
    int status;
    pid_t p1, p2, p3;
    times->parent_start = time(NULL);
    p1 = fork(); // parent starts fork 1
    if (p1 == 0) { 
        // do child 1 stuff
        // printf("--start p1 forks pid(%d)\n", getpid());
        times->p1_start = time(NULL);
        execute_command(cmd1, FILE_NAME1, 1);
        exit(0);
        
    } else if (p1 > 0) {
        // parent starts fork 2
        p2 = fork();
        if (p2 == 0) {
            // do child 2 stuff
            // printf("--start p2 forks pid(%d)\n", getpid());
            times->p2_start = time(NULL);
            execute_command(cmd2, FILE_NAME2, 2);
            exit(0);
        
        } else if (p2 > 0) {
            // parent starts fork 3
            p3 = fork();
            if (p3 == 0) {
                // do child 3 stuff
                // printf("--start p3 forks pid(%d)\n", getpid());
                times->p3_start = time(NULL);
                execute_command(cmd3, FILE_NAME3, 3);
                exit(0);

            } else if (p3 > 0) {
                // parent made three threads with fork
                // wait for children to finish
                waitpid(p1, &status, 0);
                printf("First process finished...\n");
                times->p1_end = time(NULL);

                waitpid(p2, &status, 0);
                printf("Second process finished...\n");
                times->p2_end = time(NULL);

                waitpid(p3, &status, 0);
                printf("Third process finished...\n");
                times->p3_end = time(NULL);

                /********* Output results ************/
                // command  1 results
                print_command_results(FILE_NAME1);
                printf("Result took:%ld\n", 
                    ((times->p1_end - times->p1_start)));

                // command 2 results
                print_command_results(FILE_NAME2);
                printf("Result took:%ld\n", 
                    ((times->p2_end - times->p2_start)));

                // command 3 results
                print_command_results(FILE_NAME3);
                printf("Result took:%ld\n", 
                    ((times->p3_end - times->p3_start)));

                printf("Children process IDs: %d %d %d.\n", p1, p2, p3);
                times->parent_end = time(NULL);
                printf("Total elapsed time:%ld\n", 
                    ((times->parent_end - times->parent_start)) );
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
        printf("%s ", *(temp + count) );
        count++;
    }

    /*** to print trailing dashes ******/
    for (int i = 0; i < 68 - cmd_length; i++) {
        printf("-");
    }
    printf("\n");
    
    // printf("\ncommand len: %d \n", cmd_length); //todo: remove only for testing
    if (execvp(cmd[0], cmd) == -1) {
        printf("[SHELL %d] STATUS CODE=-1\n", file_num);
        exit(-1);
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
    *(temp + (i - 1)) = 0; // replace last value

    return temp;
}

/**************
 * Todo:
 *  -get thread timming working : should use wall clock?
 *  -memory leaks? not freeing malloc
 * 
 * 
 * ************/