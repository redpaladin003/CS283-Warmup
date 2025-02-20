#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

static int last_return_code = 0;

int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(0);
    }
    if (pid == 0) {
        execvp(cmd->argv[0], cmd->argv);
        perror("execv");
        exit(1);
    } else {
        int s;
        wait(&s);
    }
    return 0;
}

int build_cmd_buff(char* cmd_line, cmd_buff_t* cmd_buff) {
    if (!cmd_buff) return -1;
    if (!cmd_line) {
        fprintf(stderr, "Error: cmd_line is NULL\n");
        return ERR_MEMORY;
    }
    cmd_buff->_cmd_buffer = cmd_line;
    cmd_buff->argc = 0;
    char *p = cmd_line;
    while (*p) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;
        char *token;
        if (*p == '"') {
            p++;
            token = p;
            while (*p && *p != '"') p++;
            if (*p == '"') {
                *p = '\0';
                p++;
            }
        } else {
            token = p;
            while (*p && !isspace((unsigned char)*p)) p++;
            if (*p) {
                *p = '\0';
                p++;
            }
        }
        if (cmd_buff->argc < CMD_ARGV_MAX - 1) {
            cmd_buff->argv[cmd_buff->argc++] = token;
        }
    }
    cmd_buff->argv[cmd_buff->argc] = NULL;
    return OK;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc > 1) {
            if (chdir(cmd->argv[1]) != 0) {
                perror("chdir");
            }
        }
        return BI_EXECUTED;
    }
    if (strcmp(cmd->argv[0], "pwd") == 0) {
        char cwd[256];
        if (getcwd(cwd, sizeof(cwd))) {
            printf("%s\n", cwd);
        } else {
            perror("getcwd");
        }
        return BI_EXECUTED;
    }
	
    if (cmd->argc < 2) return BI_EXECUTED;
    if (chdir(cmd->argv[1]) != 0) perror("chdir");
    return BI_EXECUTED;
}

int exec_local_cmd_loop()
{

    // TODO IMPLEMENT MAIN LOOP

    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"
    
    char *buf = malloc(sizeof(char) * SH_CMD_MAX);
    if (!buf) {
        fprintf(stderr, "Error: malloc failed\n");
        exit(ERR_MEMORY);
    }
    cmd_buff_t cmd;
    while (1) {
        printf("%s", SH_PROMPT);
        if (!fgets(buf, ARG_MAX, stdin)) {
            printf("\n");
            break;
        }
        buf[strcspn(buf, "\n")] = '\0';
        if (strlen(buf) == 0 || strspn(buf, " \t") == strlen(buf)) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }
        if (strcmp(buf, EXIT_CMD) == 0) {
            exit(0);
        }
        int rc = build_cmd_buff(buf, &cmd);
        if (rc == OK) {
            if (strcmp(cmd.argv[0], "cd") == 0) {
                exec_built_in_cmd(&cmd);
            } else {
                exec_cmd(&cmd);
            }
        } else if (rc == WARN_NO_CMDS) {
            printf("%s", CMD_WARN_NO_CMD);
        }
    }
    free(buf);
    return OK;
}


