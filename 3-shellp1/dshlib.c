#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */

void trim(char *str) {
    if (!str) return;
    
    // Trim leading spaces
    char *start = str;
    while(isspace((unsigned char)*start)) start++;
    
    if(*start == 0) {  // All spaces
        *str = 0;
        return;
    }
    
    // Trim trailing spaces
    char *end = str + strlen(str) - 1;
    while(end > start && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    
    // Shift string if there were leading spaces
    if(start != str)
        memmove(str, start, strlen(start) + 1);
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    // Initialize command list
    memset(clist, 0, sizeof(command_list_t));
    
    // Make a copy of cmd_line since strtok modifies the string
    char cmd_line_copy[SH_CMD_MAX];
    strncpy(cmd_line_copy, cmd_line, SH_CMD_MAX - 1);
    cmd_line_copy[SH_CMD_MAX - 1] = '\0';
    
    // Check if the command line is empty
    trim(cmd_line_copy);
    if (strlen(cmd_line_copy) == 0) {
        return WARN_NO_CMDS;
    }
    
    // Split commands by pipe
    char *saveptr1;
    char *pipe_cmd = strtok_r(cmd_line_copy, PIPE_STRING, &saveptr1);
    
    while (pipe_cmd != NULL) {
        // Check if we've exceeded the maximum number of commands
        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }
        
        // Get current command structure
        command_t *curr_cmd = &clist->commands[clist->num];
        
        // Trim whitespace from the command
        trim(pipe_cmd);
        
        // Split command into executable and arguments
        char *saveptr2;
        char *token = strtok_r(pipe_cmd, " ", &saveptr2);
        
        if (token != NULL) {
            // Copy executable name
            if (strlen(token) >= EXE_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            strcpy(curr_cmd->exe, token);
            
            // Process arguments
            char args_buffer[ARG_MAX] = "";
            token = strtok_r(NULL, " ", &saveptr2);
            
            while (token != NULL) {
                // Check if adding this argument would exceed the buffer
                if (strlen(args_buffer) + strlen(token) + 2 > ARG_MAX) {
                    return ERR_CMD_OR_ARGS_TOO_BIG;
                }
                
                // Add space between arguments if not the first argument
                if (strlen(args_buffer) > 0) {
                    strcat(args_buffer, " ");
                }
                strcat(args_buffer, token);
                
                token = strtok_r(NULL, " ", &saveptr2);
            }
            
            strcpy(curr_cmd->args, args_buffer);
            clist->num++;
        }
        
        pipe_cmd = strtok_r(NULL, PIPE_STRING, &saveptr1);
    }
    
    return OK;
}
