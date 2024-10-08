#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LENGTH 100

void freeAndResetWords(char*** words, int* word_count, int* current_line_length) {
    // Free the current words array
    if (*words != NULL) {
        for (int i = 0; i < *word_count; i++) {
            free((*words)[i]);
        }
        free(*words);
        *words = NULL; // Reset pointer
    }

    // Resetting for the next line
    *word_count = 0;
    *current_line_length = 0;
}

int fitsInLine(const char* word, int max_line_length, int current_line_length) {
    int length = strlen(word);

    // Case 1: First word in the line, only check if it fits without extra spaces
    if (current_line_length == 0) {
        return length <= max_line_length; // Fits
    }
    // Case 2: Not the first word, account for the space before the word
    return (length + current_line_length + 1) <= max_line_length; // Fits
}

void printSpaces(int space_count) {
    for (int i = 0; i < space_count; i++) {
        putchar(' '); // Print a space character
    }
}

void printJustifiedLine(char **words, int word_count, int max_line_length, int is_last_line) {
    int total_characters = 0;
    int spaces_between_words = word_count - 1;

    // Calculate the total number of characters in all words
    for (int x = 0; x < word_count; x++) {
        total_characters += strlen(words[x]);
    }

    // Case 1: Center justify for the last line or a single word
    if (is_last_line || word_count == 1) {
        int total_spaces = max_line_length - total_characters;
        int leading_spaces = total_spaces / 2;  // Center align
        int trailing_spaces = total_spaces - leading_spaces; // Remaining spaces on the right

        printSpaces(leading_spaces);  // Print leading spaces
        for (int x = 0; x < word_count; x++) {
            printf("%s", words[x]);
            if (x < word_count - 1) { // Print space between words
                printf(" ");
            }
        }
        printSpaces(trailing_spaces);  // Print trailing spaces (if any)
    }
    // Case 2: Justify evenly for lines that are not the last one
    else {
        int total_spaces = max_line_length - total_characters;
        int even_spaces = total_spaces / spaces_between_words;  // Spaces between each word
        int extra_spaces = total_spaces % spaces_between_words; // Extra spaces to distribute

        for (int x = 0; x < word_count; x++) {
            printf("%s", words[x]);
            if (x < word_count - 1) { // Print spaces after each word except the last one
                printSpaces(even_spaces + (extra_spaces > 0 ? 1 : 0)); // Add extra space from left to right
                if (extra_spaces > 0) {
                    extra_spaces--;  // Decrease extra spaces once used
                }
            }
        }
    }
    printf("\n");  // Move to the next line after printing
}

void appendWord(char*** array, int* word_counter, const char* new_word) {
    // Increment the word counter
    (*word_counter)++;

    // Reallocate the array for the new word count
    char** temp = realloc((*array), (*word_counter) * sizeof(char*));
    if (temp != NULL) {
        (*array) = temp;

        // Allocate memory for the new word
        (*array)[(*word_counter) - 1] = malloc(strlen(new_word) + 1);
        if ((*array)[(*word_counter) - 1] != NULL) {
            // Copy the new word into the allocated space
            strcpy((*array)[(*word_counter) - 1], new_word);
        }
    }
}

int processHyphenatedWord(char* word,
    char*** words,
    int* word_count,
    int* current_line_length,
    int max_line_length) {
    // Find the position of the hyphen
    char *hyphen = strchr(word, '-');

    // Calculate lengths
    size_t len1 = hyphen - word + 1; // Length of the first part including the hyphen
    size_t len2 = strlen(hyphen + 1); // Length of the second part
    size_t total_length = len1 + len2; // Total length of the hyphenated word

    // Allocate memory for the first part and copy it
    char *first_part = malloc(len1 + 1); // +1 for the null terminator
    strncpy(first_part, word, len1);
    first_part[len1] = '\0'; // Null-terminate

    // Allocate memory for the second part and copy it
    char *second_part = malloc(len2 + 1); // +1 for the null terminator
    strcpy(second_part, hyphen + 1); // Copy second part

    // Check if the first part fits in the current line
    if (fitsInLine(first_part, max_line_length, *current_line_length)) {
        appendWord(words, word_count, first_part); // Add the first part to words
        *current_line_length += len1; // Update current line length
        printJustifiedLine(*words, *word_count, max_line_length, 0); // Print the line

        // Free and reset words after printing
        freeAndResetWords(words, word_count, current_line_length);

        // Check if the second part fits in a new line
        if (strlen(second_part) > max_line_length) {
            printf("Error. The word processor can’t display the output.");
            free(first_part);
            free(second_part);
            return 1;
        }
        appendWord(words, word_count, second_part); // Add the second part
        *current_line_length += len2; // Update current line length

    } else {
        // If the first part doesn't fit, print the current line
        printJustifiedLine(*words, *word_count, max_line_length, 0);

        // Free and reset words after printing
        freeAndResetWords(words, word_count, current_line_length);

        // Check if the whole word fits
        if (fitsInLine(word, max_line_length, 0)) {
            appendWord(words, word_count, word); // Add the whole word to words
            *current_line_length += total_length; // Update current line length

        } else {
            // If the whole word doesn't fit, append the first part
            if (strlen(first_part) > max_line_length) {
                printf("Error. The word processor can’t display the output.");
                free(first_part);
                free(second_part);
                return 1;
            }
            appendWord(words, word_count, first_part);
            *current_line_length += len1; // Update current line length
            printJustifiedLine(*words, *word_count, max_line_length, 0); // Print the line

            // Free and reset words after printing
            freeAndResetWords(words, word_count, current_line_length);

            // Append the second part to the new line
            if (strlen(second_part) > max_line_length) {
                printf("Error. The word processor can’t display the output.");
                free(first_part);
                free(second_part);
                return 1;
            }
            appendWord(words, word_count, second_part);
            *current_line_length += len2; // Update current line length
        }
    }

    // Free first_part and second_part after use
    free(first_part);
    free(second_part);
    return 0;
}

void processFile(const char *filename, int max_line_length) {
    char word[MAX_WORD_LENGTH];
    int word_count = 0;
    char **words = NULL;  // Initialize words array to NULL
    int current_total_length = 0;

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    while (fscanf(file, "%s", word) == 1) {

        // Check if the current word fits on the line
        if (fitsInLine(word, max_line_length, current_total_length) && current_total_length == 0) {
            // First word on the line, no extra space before it
            appendWord(&words, &word_count, word);
            current_total_length += strlen(word);
        } else if (fitsInLine(word, max_line_length, current_total_length)) {
            // Subsequent words, account for space before each word
            appendWord(&words, &word_count, word);
            current_total_length += strlen(word) + 1;  // +1 accounts for the space between words
        } else if (strchr(word, '-') != NULL) {
            // Call processHyphenatedWord correctly
            int finish = processHyphenatedWord(word, &words, &word_count, &current_total_length, max_line_length);
            if (finish == 1) {
                for (int i = 0; i < word_count; i++) {
                    free(words[i]);
                }
                free(words); // Clean up
                return;
            }
        } else {
            if (strlen(word) > max_line_length) {
                if (word_count > 0) {
                    printJustifiedLine(words, word_count, max_line_length, 1);
                    // Free the remaining words
                    for (int i = 0; i < word_count; i++) {
                        free(words[i]);
                    }
                    free(words); // Clean up
                    fclose(file);
                    printf("Error. The word processor can’t display the output.");
                    return;
                }
            }
            // If the word doesn't fit, print and justify the current line
            printJustifiedLine(words, word_count, max_line_length, 0);
            freeAndResetWords(&words, &word_count, &current_total_length); // Reset for the next line

            // Add the word to the new line
            appendWord(&words, &word_count, word);
            current_total_length = strlen(word); // Update current total length
        }
    }
    if (word_count > 0) {
        printJustifiedLine(words, word_count, max_line_length, 1);
        // Free the remaining words
        for (int i = 0; i < word_count; i++) {
            free(words[i]);
        }
        free(words); // Clean up
    }

    fclose(file);  // Close the file
}





int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <max_line_length> <filename>\n", argv[0]);
        return 1;
    }

    int max_line_length = atoi(argv[1]);
    const char *filename = argv[2];

    // Call the function to process the file and justify the text
    processFile(filename, max_line_length);

    return 0;
}