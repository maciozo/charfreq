#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <locale.h>

typedef struct
{
    wchar_t character;
    long int frequency;
} charstat_t;

long int getCharIndex(wchar_t character, charstat_t *characters, long int numberOfChars)
{
    unsigned int i;
    for (i = 0; i < numberOfChars; i++)
    {
        if (characters[i].character == character)
        {
            return (i);
        }
    }
    return (-1);
}

int compare(const void *a, const void *b)
{
    long int difference;
    difference = (((charstat_t*) a)->frequency) - (((charstat_t*) b)->frequency);
    return ((int) difference);
}

void sort(charstat_t **characters, long int numberOfChars, charstat_t **sorted)
{
    /* Make a copy of the array */
    qsort(characters, numberOfChars, sizeof(charstat_t), compare);
}

void printList(FILE *outputFile, charstat_t **characters, long int numberOfChars)
{
    long int i;
    wchar_t line[64];
    char bom[] = "\xFF\xFE";
    fwrite(bom, 1, 2, outputFile);
    for (i = 0; i < numberOfChars; i++)
    {
        swprintf(line, 64, L"%lc - %l\n", characters[i]->character, characters[i]->frequency);
        fwrite(line, sizeof(wchar_t), wcslen(line), outputFile);
    }
    return;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Usage: charfreq <input> <output>\n");
    }

    FILE *inputFile;
    FILE *outputFile;
    
    setlocale(LC_CTYPE, "");

    struct stat stbuf;
    long int inputSize;

    wchar_t *inputData;
    charstat_t *characters;
    charstat_t *sorted;
    long int i;
    long int numberOfChars = 0;
    long int charIndex;

    inputFile = fopen(argv[1], "r"); /* Open the input file in read mode */
    if (inputFile == NULL)
    {
        perror("Failed to open input file");
        return(1);
    }

    fstat(fileno(inputFile), &stbuf); /* POSIX stat on the input file */
    inputSize = stbuf.st_size; /* Get the size of the input file */

    inputData = (wchar_t*) malloc(inputSize); /* Allocate memory required to store the input file */
    fwide(inputFile, 1); /* Set the orientation of the file stream to wide */

    /* Read data from the input file and store it in the inputData array */
    fread(inputData, sizeof(wchar_t), inputSize / sizeof(wchar_t), inputFile);
    
    /* Close the file, since we don't need it anymore */
    fclose(inputFile);
    
    outputFile = fopen(argv[2], "wb"); /* Open the output file in write mode */
    if (outputFile == NULL)
    {
        perror("Failed to open output file");
        return(1);
    }
    fclose(outputFile);
    outputFile = fopen(argv[2], "ab");
    
    characters = (charstat_t*) malloc(sizeof(charstat_t));

    /* Counting how often each character appears */
    for (i = 0; i < (inputSize / sizeof(wchar_t)); i++)
    {
        charIndex = getCharIndex(inputData[i], characters, numberOfChars);
        if (charIndex == -1)
        {
            /* The character has not appeared before */
            numberOfChars++;
            
            /* Add another space to the characters array */
            characters = (charstat_t*) realloc(characters, sizeof(charstat_t) * numberOfChars);
            
            characters[numberOfChars - 1].character = inputData[i];
            characters[numberOfChars - 1].frequency = 1;
        }
        
        else
        {
            /* The character has appeared before */
            characters[charIndex].frequency++;
        }
    }
    
    sorted = (charstat_t*) malloc(sizeof(charstat_t) * numberOfChars);
    sort(&characters, numberOfChars, &sorted);
    printList(outputFile, &characters, numberOfChars);
    return (0);
}
