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

int exec_local_cmd_loop()
{
   char cmd_buff[SH_CMD_MAX];
   command_list_t cmd_list;

   while (1) {
       printf("%s", SH_PROMPT);
       if (!fgets(cmd_buff, SH_CMD_MAX, stdin)) {
           printf("\n");
           break;
       }

       cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

       if (strlen(cmd_buff) == 0) {
           printf(CMD_WARN_NO_CMD);
           continue;
       }

       int rc = build_cmd_list(cmd_buff, &cmd_list);
       if (rc == WARN_NO_CMDS) {
           printf(CMD_WARN_NO_CMD);
           continue;
       } else if (rc == ERR_TOO_MANY_COMMANDS) {
           printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
           continue;
       }

       if (cmd_list.num == 1) {
           Built_In_Cmds bi_cmd = match_command(cmd_list.commands[0].argv[0]);
           if (bi_cmd == BI_CMD_EXIT) {
               return OK;
           } else if (bi_cmd == BI_CMD_CD) {
               exec_built_in_cmd(&cmd_list.commands[0]);
               continue;
           } else if (bi_cmd == BI_CMD_DRAGON) {
               extern void print_dragon();
               print_dragon();
               continue;
           }
           exec_cmd(&cmd_list.commands[0]); 
       } else {
           execute_pipeline(&cmd_list);
       }
   }
   return OK;
}

Built_In_Cmds match_command(const char *input)
{
   if (strcmp(input, "exit") == 0) {
       return BI_CMD_EXIT;
   } else if (strcmp(input, "cd") == 0) {
       return BI_CMD_CD;
   } else if (strcmp(input, "dragon") == 0) {
       return BI_CMD_DRAGON;
   }
   return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd)
{
   if (strcmp(cmd->argv[0], "exit") == 0) {
       exit(0);
   } else if (strcmp(cmd->argv[0], "cd") == 0) {
       if (cmd->argc > 1) {
           if (chdir(cmd->argv[1]) != 0) {
               perror("cd");
           }
       }
       return BI_EXECUTED;
   } else if (strcmp(cmd->argv[0], "dragon") == 0) {
       extern void print_dragon();
       print_dragon();
       return BI_EXECUTED;
   }
   return BI_NOT_BI;
}

int exec_cmd(cmd_buff_t *cmd)
{
   pid_t pid = fork();
   if (pid == 0) { 
       execvp(cmd->argv[0], cmd->argv);
       perror("execvp failed");
       exit(EXIT_FAILURE);
   } else if (pid > 0) { 
       int status;
       waitpid(pid, &status, 0);
       return status;
   } else { 
       perror("fork failed");
       return ERR_EXEC_CMD;
   }
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd)
{
   memset(cmd, 0, sizeof(cmd_buff_t));

   if (strlen(cmd_line) == 0) {
       return WARN_NO_CMDS;
   }

   char *ptr = cmd_line;
   int argc = 0;
   bool in_quotes = false;

   while (*ptr) {
       while (*ptr == ' ' && !in_quotes) ptr++;
       if (*ptr == '\0') break;

       if (*ptr == '"') {
           in_quotes = !in_quotes;
           ptr++;
           cmd->argv[argc++] = ptr;
       } else {
           cmd->argv[argc++] = ptr;
       }

       while (*ptr && (in_quotes || *ptr != ' ')) {
           if (*ptr == '"') {
               in_quotes = !in_quotes;
               *ptr = '\0';
           }
           ptr++;
       }

       if (*ptr == ' ') *ptr++ = '\0';

       if (argc >= CMD_ARGV_MAX - 1) {
           return ERR_TOO_MANY_COMMANDS;
       }
   }
   cmd->argv[argc] = NULL;
   cmd->argc = argc;

   if (argc == 0) {
       return WARN_NO_CMDS;
   } else {
       return OK;
   }
}

int build_cmd_list(char *cmd_line, command_list_t *clist)
{
   memset(clist, 0, sizeof(command_list_t));

   if (strlen(cmd_line) == 0) {
       return WARN_NO_CMDS;
   }

   char *token = strtok(cmd_line, PIPE_STRING);
   while (token) {
       if (clist->num >= CMD_MAX) {
           return ERR_TOO_MANY_COMMANDS;
       }

       int rc = build_cmd_buff(token, &clist->commands[clist->num]);
       if (rc != OK) return rc;

       clist->num++;
       token = strtok(NULL, PIPE_STRING);
   }

   return OK;
}

int execute_pipeline(command_list_t *clist)
{
   if (clist->num == 0) return WARN_NO_CMDS;

   int pipes[CMD_MAX - 1][2];
   pid_t pids[CMD_MAX];

   for (int i = 0; i < clist->num - 1; i++) {
       if (pipe(pipes[i]) == -1) {
           perror("pipe failed");
           return ERR_MEMORY;
       }
   }

   for (int i = 0; i < clist->num; i++) {
       pids[i] = fork();

       if (pids[i] < 0) {
           perror("fork failed");
           return ERR_EXEC_CMD;
       }

       if (pids[i] == 0) {
           if (i > 0) {
               dup2(pipes[i - 1][0], STDIN_FILENO);
           }
           if (i < clist->num - 1) {
               dup2(pipes[i][1], STDOUT_FILENO);
           }

           for (int j = 0; j < clist->num - 1; j++) {
               close(pipes[j][0]);
               close(pipes[j][1]);
           }

           execvp(clist->commands[i].argv[0], clist->commands[i].argv);
           perror("execvp failed");
           exit(ERR_EXEC_CMD);
       }
   }

   for (int i = 0; i < clist->num - 1; i++) {
       close(pipes[i][0]);
       close(pipes[i][1]);
   }

   for (int i = 0; i < clist->num; i++) {
       waitpid(pids[i], NULL, 0);
   }

   return OK;
}
