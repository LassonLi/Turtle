#include <stdio.h>
#include <stdlib.h>

#define MAX_ROWS 33
#define MAX_COLS 51

int main(int argc, char *argv[])
{
    FILE *file1, *file2;
    int ch1, ch2;
    int row = 1, col = 1;

    // Check for correct number of arguments
    if (argc != 3) {
        printf("Usage: %s <file1> <file2>\n", argv[0]);
        return 1;
    }

    // Open first file
    file1 = fopen(argv[1], "r");
    if (file1 == NULL) {
        perror("Error opening first file");
        return 1;
    }

    // Open second file
    file2 = fopen(argv[2], "r");
    if (file2 == NULL) {
        perror("Error opening second file");
        fclose(file1); // Close the first file before returning
        return 1;
    }

    // Compare the contents of the files within the specified range
    while (row <= MAX_ROWS && col <= MAX_COLS &&
           (ch1 = fgetc(file1)) != EOF && (ch2 = fgetc(file2)) != EOF) {
        
        if (ch1 != ch2) {
            printf("Files are different.\n");
            fclose(file1);
            fclose(file2);
            return 0;
        }

        // Increment row and column counters
        if (ch1 == '\n') {
        row++;
        col = 1;
        } else {
        col++;
        }

    // Check if column limit is reached, move to next row
    if (col > MAX_COLS) {
        row++;
        col = 1;
        // Skip to the end of the current line in both files
        while ((ch1 = fgetc(file1)) != '\n' && ch1 != EOF) { }
        while ((ch2 = fgetc(file2)) != '\n' && ch2 != EOF) { }
        }
    }

    // Check if we stopped because we reached the limits
    if (row > MAX_ROWS || (row == MAX_ROWS && col > MAX_COLS)) {
        printf("Files are the same within the first %d rows and %d columns.\n", MAX_ROWS, MAX_COLS);
    } 
    else {
        // Check if both files reached EOF
        if (feof(file1) && feof(file2)) {
            printf("Files are the same.\n");
        } 
        else {
            printf("Files are different.\n");
        }
    }
    printf("\n");

    // Close the files
    fclose(file1);
    fclose(file2);

    return 0;
}