#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dshlib.h"

/*
 * Implement your main function by building a loop that prompts the
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.  Since we want fgets to also handle
 * end of file so we can run this headless for testing we need to check
 * the return code of fgets.  I have provided an example below of how
 * to do this assuming you are storing user input inside of the cmd_buff
 * variable.
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
 *
 *   Expected output:
 *
 *      CMD_OK_HEADER      if the command parses properly. You will
 *                         follow this by the command details
 *
 *      CMD_WARN_NO_CMD    if the user entered a blank command
 *      CMD_ERR_PIPE_LIMIT if the user entered too */

typedef struct {
    int spaces;   // leading spaces
    int length;   // number of characters
    char ch;      // character to repeat
    int newline;  // should print newline after
} dragon_rle;

// Compressed representation of the dragon art
const dragon_rle dragon_data[] = {
    {4, 4, '%', 1},   // First row
    {5, 5, '%', 1},
    {5, 5, '%', 1},
    {0, 1, '%', 0}, {1, 7, '%', 1},
    {4, 1, '@', 1},
    {10, 10, '%', 1},
    {7, 7, '%', 1},
    {7, 7, '%', 0}, {4, 4, '%', 0}, {0, 4, '@', 0}, {2, 13, '%', 0}, {0, 1, '@', 1},
    {6, 6, '%', 0}, {1, 4, '%', 1},
    {0, 36, '%', 1},
    {0, 36, '%', 0}, {11, 11, '%', 1},
    {13, 13, '%', 1},
    {0, 36, '%', 1},
    {19, 19, '%', 0}, {3, 3, '%', 1},
    {1, 1, '@', 0}, {0, 36, '%', 1},
    {1, 1, '@', 0}, {19, 19, '%', 0}, {2, 2, '%', 1},
    {0, 36, '%', 1},
    {0, 36, '%', 1},
    {0, 64, '%', 1},
    {0, 47, '%', 0}, {0, 1, '@', 0}, {6, 6, '%', 0}, {0, 1, '@', 1},
    {8, 8, '%', 0}, {0, 1, '@', 0}, {11, 11, '%', 1},
    {0, 36, '%', 0}, {2, 2, '%', 1},
    {11, 11, '%', 0}, {2, 2, '%', 0}, {11, 11, '%', 0}, {11, 11, '%', 1},
    {11, 11, '%', 0}, {2, 2, '@', 0}, {1, 1, '%', 1},
    {8, 8, '%', 0}, {3, 3, '%', 0}, {11, 11, '%', 1},
    {0, 36, '%', 1},
    {9, 9, '%', 0}, {0, 1, '@', 0}, {2, 2, '%', 0}, {11, 11, '%', 1},
    {1, 1, '@', 0}, {0, 36, '%', 1},
    {9, 9, '%', 0}, {0, 1, '@', 0}, {11, 11, '%', 1},
    {1, 1, '@', 0}, {0, 36, '%', 1},
    {8, 8, '%', 0}, {0, 1, '@', 0}, {11, 11, '%', 1},
    {0, 36, '%', 1},
    {0, 0, 0, 0}  // End marker
};

void print_compressed_dragon() {
    const dragon_rle *curr = dragon_data;
    while (curr->length > 0) {
        // Print leading spaces
        for (int i = 0; i < curr->spaces; i++) {
            putchar(' ');
        }
        // Print the repeated character
        for (int i = 0; i < curr->length; i++) {
            putchar(curr->ch);
        }
        if (curr->newline) {
            putchar('\n');
        }
        curr++;
    }
}

int main() {
    char cmd_buff[SH_CMD_MAX];
    int rc = 0;
    command_list_t clist;

    while(1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        // Remove the trailing newline
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
        
        // Check for exit command
        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            break;
        }

        // Check for dragon command
        if (strcmp(cmd_buff, "dragon") == 0) {
            print_compressed_dragon();
            continue;
        }
        
        // Parse the command line
        rc = build_cmd_list(cmd_buff, &clist);
        
        // Handle different return codes
        if (rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
        } else if (rc == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
        } else if (rc == OK) {
            printf(CMD_OK_HEADER, clist.num);
            
            // Print each command
            for (int i = 0; i < clist.num; i++) {
                printf("<%d> %s", i + 1, clist.commands[i].exe);
                if (strlen(clist.commands[i].args) > 0) {
                    printf("[%s]", clist.commands[i].args);
                }
                printf("\n");
            }
        }
    }
    
    return 0;
}
