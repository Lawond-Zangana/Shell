#include <ctype.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "pish.h"

/*
 * Batch mode flag. If set to 0, the shell reads from stdin. If set to 1,
 * the shell reads from a file from argv[1].
 */
static int script_mode = 0;

/*
 * Prints a prompt IF NOT in batch mode (see script_mode global flag),
 */
int prompt(void)
{
    static const char prompt[] = { 0xe2, 0x96, 0xb6, ' ', ' ', '\0' };
    if (!script_mode) {
        printf("%s", prompt);
        fflush(stdout);
    }
    return 1;
}

/*
 * Print usage error for built-in commands.
 */
void usage_error(void)
{
    fprintf(stderr, "pish: Usage error\n");
    fflush(stdout);
}

/*
 * Break down a line of input by whitespace, and put the results into
 * a struct pish_arg to be used by other functions.
 *
 * @param command   A char buffer containing the input command
 * @param arg       Broken down args will be stored here.
 */
void parse_command(char *command, struct pish_arg *arg)
{
    // TODO
    // 1. Clear out the arg struct
    // 2. Parse the `command` buffer and update arg->argc & arg->argv.

    // Clear out the arg struct
    arg->argc = 0;
    for (int i = 0; i < MAX_ARGC; i++) {
        arg->argv[i] = NULL;
    }

    // Parse the `command` buffer and update arg->argc & arg->argv.
    char *token = strtok(command, " \t\n");
    while (token != NULL && arg->argc < MAX_ARGC - 1) {
        arg->argv[arg->argc] = token;
        arg->argc++;
        token = strtok(NULL, " \t\n");
    }
    arg->argv[arg->argc] = NULL; // Ensure the last element is NULL
}

/*
 * Run a command.
 *
 * Built-in commands are handled internally by the pish program.
 * Otherwise, use fork/exec to create child process to run the program.
 *
 * If the command is empty, do nothing.
 * If NOT in batch mode, add the command to history file.
 */
void run(struct pish_arg *arg)
{
    // TODO

    // Do nothing if the command is empty
    if (arg->argc == 0) {
        return;
    }

    // Add command to history if not in batch mode
    if (!script_mode) {
        add_history(arg);
    }

    // Handle built-in commands
    if (strcmp(arg->argv[0], "exit") == 0) {
        if (arg->argc != 1) {
            usage_error();
        } else {
            exit(EXIT_SUCCESS);
        }
    } else if (strcmp(arg->argv[0], "cd") == 0) {
        if (arg->argc != 2) {
            usage_error();
        } else {
            if (chdir(arg->argv[1]) != 0) {
                perror("cd");
            }
        }
    } else if (strcmp(arg->argv[0], "history") == 0) {
        if (arg->argc != 1) {
            usage_error();
        } else {
            print_history();
        }
    } else {
        // External command handling
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return;
        } else if (pid == 0) {
            // Child process
            execvp(arg->argv[0], arg->argv);
            perror("pish");
            exit(EXIT_FAILURE);
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

/*
 * The main loop. Continuously reads input from a FILE pointer
 * (can be stdin or an actual file) until `exit` or EOF.
 */
int pish(FILE *fp)
{
    // assume input does not exceed buffer size
    char buf[1024];
    struct pish_arg arg;

    /* TODO:
     * Figure out how to read input and run commands on a loop.
     * It should look something like:

            while (prompt() && [TODO] {
                parse_command(buf, &arg);
                run(&arg);
            }

     * The [TODO] is where you read a line from `fp` into `buf`.
     */
    while (prompt() && fgets(buf, sizeof(buf), fp) != NULL) {
        parse_command(buf, &arg);
        run(&arg);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    FILE *fp;

    /* TODO:
     * Set up fp to either stdin or an open file.
     * - If the shell is run without argument (argc == 1), use stdin.
     * - If the shell is run with 1 argument (argc == 2), use that argument
     *   as the file path to read from.
     * - If the shell is run with 2+ arguments, call usage_error() and exit.
     */
    if (argc == 1) {
        fp = stdin;
        script_mode = 0;
    } else if (argc == 2) {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            perror("open");
            return EXIT_FAILURE;
        }
        script_mode = 1;
    } else {
        usage_error();
        return EXIT_FAILURE;
    }

    pish(fp);

    /* TODO:
     * close fp if it is not stdin.
     */

    if (fp != stdin) {
        fclose(fp);
    }

    return EXIT_SUCCESS;
}
