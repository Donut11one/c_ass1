#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_WORD_LENGTH 10

void append_word(char*** array, int* wordcounter, const char* new_word) {
    // Increment the word counter
    (*wordcounter)++;

    // Reallocate the array for the new word count
    char** temp = realloc((*array), (*wordcounter) * sizeof(char*));
    if (temp != NULL) {
        (*array) = temp;

        // Allocate memory for the new word
        (*array)[(*wordcounter) - 1] = malloc(strlen(new_word) + 1);
        if ((*array)[(*wordcounter) - 1] != NULL) {
            // Copy the new word into the allocated space
            strcpy((*array)[(*wordcounter) - 1], new_word);
        }
    }
}

// /returns 1 as true/
int fitsInLine(char* word, int max_line_length, int currentTotal) {
    int length = strlen(word);
    if (currentTotal == 0)
    {
        if (length + currentTotal <= max_line_length) {
        return 1;
    }
    }
    else if (length + currentTotal + 1 <= max_line_length) {
        return 1;
    }
    return 0;
}
void printJustifiedLine(char **words, int word_count, int max_line_length, int is_last_line) {
    int total_chars = 0;
    
    // Calculate the total number of characters in the words
    for (int i = 0; i < word_count; i++) {
        total_chars += strlen(words[i]);
    }

    // Calculate total spaces to be distributed
    int total_spaces = max_line_length - total_chars;

    // For both the last line and non-last lines, evenly distribute spaces
    int min_space = word_count > 1 ? total_spaces / (word_count - 1) : 0; // Minimum space between words
    int extra_space = word_count > 1 ? total_spaces % (word_count - 1) : 0; // Extra spaces to distribute

    // Print words with evenly distributed spaces
    for (int i = 0; i < word_count; i++) {
        printf("%s", words[i]);

        // Add spaces after the word, except after the last word
        if (i < word_count - 1) {
            // Print the minimum spaces plus one extra space if needed
            int spaces_to_print = min_space + (i < extra_space ? 1 : 0);
            for (int j = 0; j < spaces_to_print; j++) {
                printf(" ");
            }
        }
    }

    // If the last line is shorter than the max line length, pad with trailing spaces
    if (is_last_line && word_count == 1) {
        for (int i = total_chars; i < max_line_length; i++) {
            printf(" ");
        }
    }
    printf("\n");
}
// Function to split and process a hyphenated word across lines:
// void processHyphenatedWord(char **words, int *word_count, int *current_line_length, int max_line_length) {


// }

int main(int argc, char *argv[]) {
    char word[MAX_WORD_LENGTH]; 
    int word_count = 0;
    int max_line_length;
    char **words = NULL;  // Initialize words array to NULL
    int currentTotal = 0;
    int currWordCount = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[2];
    max_line_length = atoi(argv[1]);
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Read words from the file
    while (fscanf(file, "%s", word) == 1) {
        if (fitsInLine(word, max_line_length, currentTotal) && currentTotal == 0) {
            append_word(&words, &word_count, word);
            currentTotal += strlen(word);
        } 
        else if (fitsInLine(word, max_line_length, currentTotal)) {
            append_word(&words, &word_count, " ");
            append_word(&words, &word_count, word);
            currentTotal += strlen(word) + 1;
        } 
        else {
            //check if word has hypen
            // if (strchr(word, '-') != NULL)
            // {
            //     processHyphenatedWord(*word,
            //     &words,
            //     &word_count,
            //     currentTotal,
            //     max_line_length); 
                    
            // }
            // else{

            // }

            // Print and justify the current line
            printJustifiedLine(words, word_count, max_line_length, 0);

            // Free words and reset variables for the next line
            for (int i = 0; i < word_count; i++) {
                free(words[i]);
            }
            free(words);

            words = NULL;  // Reset words array
            word_count = 0;
            currentTotal = 0;

            // Append the current word to the new line
            append_word(&words, &word_count, word);
            currentTotal += strlen(word);
        }
    }
    // Print and justify the last line if any words remain
    if (word_count > 0) {
        printJustifiedLine(words, word_count, max_line_length, 1);

        // Free the remaining words
        for (int i = 0; i < word_count; i++) {
            free(words[i]);
        }
        free(words);
    }
    free(words);
    fclose(file); // Close the file
    return 0;
}