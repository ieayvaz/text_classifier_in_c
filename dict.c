#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS 50000
#define MAX_WORD_LENGTH 50
#define MAX_LINE_LENGTH 1000

int isDuplicate(char **dictionary, int count, char *word) {
    for (int i = 0; i < count; i++) {
        if (strcmp(dictionary[i], word) == 0) {
            return 1; // Word is a duplicate
        }
    }
    return 0; // Word is not a duplicate
}

char **make_dictionary(char *content) {
    char **dictionary = malloc(sizeof(char *) * MAX_WORDS);

    int i = 0;
    char *token = strtok(content, " ");
    while (token != NULL && i < MAX_WORDS) {
        // Check if the word is already in the dictionary
        if (!isDuplicate(dictionary, i, token)) {
            dictionary[i] = strdup(token);
            i++;
        }
        token = strtok(NULL, " ");
    }

    return dictionary;
}

void free_dictionary(char **dictionary, int size) {
    for (int i = 0; i < size; i++) {
        free(dictionary[i]);
    }
    free(dictionary);
}

int *make_hotvector(char **dictionary, char **content){
    int *hotvec = malloc(sizeof(int)*MAX_WORDS);
    memset(hotvec, 0, MAX_WORDS * sizeof(int));

    int i = 0;
    while(content[i] != NULL && i < MAX_LINE_LENGTH){
      int j = 0;
      while(dictionary[j] != NULL && j < MAX_WORDS){
	if(strcmp(dictionary[j],content[i]) == 0){
	  hotvec[j] = 1;
	  break;
	}
	j++;
      }
      i++;
    }
    
    return hotvec;
}

char** splitStringIntoWords(const char* inputString,int *wordnum) {
    // Allocate memory for the array of strings
    char** words = (char**)malloc(MAX_LINE_LENGTH * sizeof(char*));
    
    if (words == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Initialize variables
    int wordCount = 0;
    const char* delimiter = " \t\n"; // Words are separated by space, tab, or newline

    // Tokenize the input string
    char* token = strtok((char*)inputString, delimiter);
    while (token != NULL && wordCount < MAX_WORDS) {
        // Allocate memory for each word
        words[wordCount] = (char*)malloc((strlen(token) + 1) * sizeof(char));
        
        if (words[wordCount] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        // Copy the word into the array
        strcpy(words[wordCount], token);

        // Move to the next word
        token = strtok(NULL, delimiter);
        wordCount++;
    }

    *wordnum = wordCount;
    
    return words;
}

void freeWordArray(char** words, int numWords) {
    // Free memory for each word
    for (int i = 0; i < numWords; i++) {
        free(words[i]);
    }

    // Free memory for the array of strings
    free(words);
}

int main() {
    FILE *file = fopen("merged_output.txt", "r");
    FILE *outfile = fopen("hotvectors.txt", "w+");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = malloc(file_size + 1);
    if (content == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    fread(content, 1, file_size, file);
    content[file_size] = '\0';

    char **dictionary = make_dictionary(content);
    fseek(file, 0, SEEK_SET);

    char line[MAX_LINE_LENGTH];
    while(fgets(line, MAX_LINE_LENGTH, file)){
	int wordnum = 0;
	char **words = splitStringIntoWords(line,&wordnum);

        int *hv = make_hotvector(dictionary,words);
	for(int i = 0;i < MAX_WORDS;i++){
	  fprintf(outfile,"%d ",hv[i]);
	}
	fprintf(outfile,"\n");
	free(hv);
	freeWordArray(words,wordnum);
    }

    // Cleanup
    free(content);
    free_dictionary(dictionary, MAX_WORDS);

    return 0;
}
