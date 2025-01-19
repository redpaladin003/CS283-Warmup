#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
void reverse_string(char *, int);
void print_words_and_lengths(char *, int);
void replace_word(char *, char *, char *, int);

int setup_buff(char *buff, char *user_str, int len) {
    char *src = user_str;  // Pointer to the source string
    char *dst = buff;      // Pointer to the destination buffer
    int count = 0;         // Tracks the number of characters written
    int space_added = 0;   // Tracks if a space was recently added

    // Process the user-supplied string
    while (*src != '\0') {
        // Check for buffer overflow
        if (count >= len) {
            fprintf(stderr, "Error: Provided input string is too large\n");
            return -1;  // Input string exceeds buffer size
        }

        // Handle whitespace characters (space or tab)
        if (*src == ' ' || *src == '\t') {
            if (!space_added) {
                *dst++ = ' ';  // Add a single space to the buffer
                count++;
                space_added = 1;  // Mark that a space has been added
            }
        } else {
            *dst++ = *src;  // Copy non-whitespace character
            count++;
            space_added = 0;  // Reset the space flag
        }
        src++;
    }

    // Fill the remaining buffer with dots
    while (count < len) {
        *dst++ = '.';
        count++;
    }

    return count;  // Return the length of the processed string
}



void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

void print_words_and_lengths(char *buff, int len) {
    char *ptr = buff;    // Pointer to traverse the buffer
    char *start = NULL;  // Start pointer for the current word
    int word_count = 0;  // Word counter

    printf("Word Print\n----------\n");

    for (int i = 0; i < len && *ptr != '.'; i++) {
        if (*ptr == ' ') {
            if (start) {
                // Print the word and its length
                printf("%d. ", ++word_count);
                for (char *p = start; p < ptr; p++) {
                    putchar(*p);
                }
                printf(" (%ld)\n", ptr - start);
                start = NULL;  // Reset the start pointer
            }
        } else if (!start) {
            start = ptr;  // Mark the start of a word
        }
        ptr++;
    }

    // Handle the last word
    if (start) {
        printf("%d. ", ++word_count);
        for (char *p = start; p < ptr; p++) {
            putchar(*p);
        }
        printf(" (%ld)\n", ptr - start);
    }
}


void replace_word(char *buff, char *find, char *replace, int len) {
    char *pos = buff;           // Pointer to traverse the buffer
    int find_len = 0;           // Length of the string to find
    int replace_len = 0;        // Length of the replacement string
    int remaining_len;          // Remaining length in the buffer after the replacement

    // Calculate the lengths of 'find' and 'replace' strings
    while (*(find + find_len) != '\0') {
        find_len++;
    }
    while (*(replace + replace_len) != '\0') {
        replace_len++;
    }

    // Find the first occurrence of 'find' in the buffer
    while (*pos != '\0' && *pos != '.') {
        char *current = pos;
        char *search = find;
        
        // Check if the current substring matches 'find'
        while (*current == *search && *search != '\0') {
            current++;
            search++;
        }

        // If we found a match
        if (*search == '\0') {
            remaining_len = len - (current - buff);

            // Check if the replacement will cause buffer overflow
            if (remaining_len < replace_len - find_len) {
                fprintf(stderr, "Error: Replacement causes buffer overflow\n");
                exit(3);
            }

            // Shift the remaining characters to accommodate the replacement
            if (replace_len > find_len) {
                for (char *shift_pos = buff + len - 1; shift_pos >= current; shift_pos--) {
                    *(shift_pos + (replace_len - find_len)) = *shift_pos;
                }
            } else if (replace_len < find_len) {
                for (char *shift_pos = current; *shift_pos != '\0'; shift_pos++) {
                    *(shift_pos - (find_len - replace_len)) = *shift_pos;
                }
            }

            // Copy the replacement string into the buffer
            for (int i = 0; i < replace_len; i++) {
                *(pos + i) = *(replace + i);
            }

            return; // Only replace the first occurrence
        }

        pos++;
    }

    // If no match was found
    fprintf(stderr, "Error: String '%s' not found in buffer\n", find);
    exit(3);
}



void reverse_string(char *buff, int len) {
    char *start = buff;           // Start pointer
    char *end = buff + len - 1;   // End pointer

    // Move the `end` pointer to exclude dots
    while (*end == '.' && end > start) {
        end--;
    }

    // Reverse the string
    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

int count_words(char *buff, int len, int str_len){
    //YOU MUST IMPLEMENT
    char *ptr = buff;  // Pointer to traverse the buffer
    int count = 0;     // Word count
    int in_word = 0;   // Tracks whether we're inside a word

    for (int i = 0; i < str_len; i++) {
        if (*ptr == ' ') {
            in_word = 0;  // End of a word
        } else if (!in_word) {
            in_word = 1;  // Start of a new word
            count++;
        }
        ptr++;
    }

    return count;  // Return the word count
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //  The combinations of both line checks prevent the program from accessing out-of-bound or invalid memory, by ensuring that the program is called with at least one argument and it also ensures that the input format follows the expected convention
   
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //    Ensures the user provides a string as the second argument (the string to be processed).
    //	  Without this check, accessing argv[2] (the input string) would cause undefined behavior if it doesn't exist.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    
    buff = (char *)malloc(BUFFER_SZ * sizeof(char));
    if (buff == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(2);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        free(buff);
	printf("Error setting up buffer, error = %d", user_str_len);
        exit(3);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
    	    printf("Buffer:  [");
    	    for (int i = 0; i < BUFFER_SZ; i++) {
            	putchar(buff[i]);
    	    }	
    	    printf("]\n");
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
	case 'r':
	    reverse_string(buff, user_str_len);
	    printf("Reversed String: %s\n", buff);
	    break;
	
	case 'w':
            print_words_and_lengths(buff, BUFFER_SZ);
            break;

        case 'x':
            if (argc < 5) {
                fprintf(stderr, "Error: '-x' requires two additional arguments\n");
                usage(argv[0]);
                free(buff);
                exit(1);
            }
	    replace_word(buff, argv[3], argv[4], BUFFER_SZ);
	    printf("Modified String: %s\n", buff);
	    break;

	default:
            usage(argv[0]);
            free(buff);
	    exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    
    free(buff);
    exit(0);
}
