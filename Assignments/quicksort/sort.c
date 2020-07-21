/*******************************************************************************
 * Name        : sort.c
 * Author      : Michael DelGaudio & Sunmin Lee
 * Date        : 5/29/20
 * Description : Uses quicksort to sort a file of either ints, doubles, or
 *               strings.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quicksort.h"

#define MAX_STRLEN 64 // Not including '\0'
#define MAX_ELEMENTS 1024

typedef enum
{
    STRING,
    INT,
    DOUBLE
} elem_t;

void display_usage(char *fileExecuted)
{

    printf(
        "Usage: %s [-i|-d] filename\n   -i: Specifies the file contains ints.\n   -d: Specifies the file contains doubles.\n   filename: The file to sort.\n   No flags defaults to sorting strings.\n",
        fileExecuted);
}

/**
 * Reads data from filename into an already allocated 2D array of chars.
 * Exits the entire program if the file cannot be opened.
 */
size_t read_data(char *filename, char **data)
{
    // Open the file.
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error: Cannot open '%s'. %s.\n", filename,
                strerror(errno));
        free(data);
        exit(EXIT_FAILURE);
    }

    // Read in the data.
    size_t index = 0;
    char str[MAX_STRLEN + 2];
    char *eoln;
    while (fgets(str, MAX_STRLEN + 2, fp) != NULL)
    {
        eoln = strchr(str, '\n');
        if (eoln == NULL)
        {
            str[MAX_STRLEN] = '\0';
        }
        else
        {
            *eoln = '\0';
        }
        // Ignore blank lines.
        if (strlen(str) != 0)
        {
            data[index] = (char *)malloc((MAX_STRLEN + 1) * sizeof(char));
            strcpy(data[index++], str);
        }
    }

    // Close the file before returning from the function.
    fclose(fp);

    return index;
}

/**
 * Basic structure of sort.c:
 *
 * Parses args with getopt.
 * Opens input file for reading.
 * Allocates space in a char** for at least MAX_ELEMENTS strings to be stored,
 * where MAX_ELEMENTS is 1024.
 * Reads in the file
 * - For each line, allocates space in each index of the char** to store the
 *   line.
 * Closes the file, after reading in all the lines.
 * Calls quicksort based on type (int, double, string) supplied on the command
 * line.
 * Frees all data.
 * Ensures there are no memory leaks with valgrind. 
 **/
int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        display_usage(argv[0]);
        return EXIT_FAILURE;
    }
    char opt;
    int i_flag = 0, d_flag = 0, total_flags = 0;
    size_t len;
    char **array = malloc(sizeof(char *) * (MAX_ELEMENTS + 1));

    while ((opt = getopt(argc, argv, ":id")) != -1)
    {
        switch (opt)
        {
        case 'i':
            // specifies the file contains ints
            if (total_flags != 0)
            {
                printf("Error: Too many flags specified.\n");
                free(array);
                return EXIT_FAILURE;
            }

            i_flag = 1;
            total_flags++;

            break;
        case 'd':
            // specifices the file contains doubles
            if (total_flags != 0)
            {
                printf("Error: Too many flags specified.\n");
                free(array);
                return EXIT_FAILURE;
            }

            d_flag = 1;
            total_flags++;

            break;
        case '?':
            printf("Error: Unknown option '-%c' received.\n", optopt);
            display_usage(argv[0]);
            free(array);
            return EXIT_FAILURE;
        }
    }

    //sort strings
    if (i_flag == 0 && d_flag == 0)
    {

        if (argc > 2)
        {
            printf("Error: Too many files specified.\n");
            free(array);
            return EXIT_FAILURE;
        }
        len = read_data(argv[1], array);

        // sorting strings
        quicksort(array, len, sizeof(char *), str_cmp);
        // swap(array, array+1, sizeof(char*));
        for (size_t j = 0; j < len; j++)
        {
            printf("%s\n", array[j]);
        }
        for (size_t i = 0; i < len; i++)
        {
            free(array[i]);
        }
        free(array);

        return EXIT_SUCCESS;
    }

    //sort ints
    if (i_flag == 1 && d_flag == 0)
    {
        if (argc <= 2)
        {
            printf("Error: No input file specified.\n");
            free(array);
            return EXIT_FAILURE;
        }
        else if (argc > 3)
        {
            printf("Error: Too many files specified.\n");
            free(array);
            return EXIT_FAILURE;
        }
        len = read_data(*(argv + 2), array);
        int *num_array = malloc(sizeof(int) * (MAX_ELEMENTS + 1));
        for (size_t j = 0; j < len; j++)
        {
            num_array[j] = atoi(array[j]);
        }

        quicksort(num_array, len, sizeof(int), int_cmp);
        for (size_t j = 0; j < len; j++)
        {
            printf("%d\n", num_array[j]);
        }
        for (size_t i = 0; i < len; i++)
        {
            free(array[i]);
        }
        free(array);
        free(num_array);
        return EXIT_SUCCESS;
    }

    //sort doubles
    if (i_flag == 0 && d_flag == 1)
    {
        if (argc <= 2)
        {
            printf("Error: No input file specified.\n");
            free(array);
            return EXIT_FAILURE;
        }
        else if (argc > 3)
        {
            printf("Error: Too many files specified.\n");
            free(array);
            return EXIT_FAILURE;
        }
        len = read_data(*(argv + 2), array);
        double *dbl_array = malloc(sizeof(double) * (MAX_ELEMENTS + 1));
        char *idk;
        for (size_t j = 0; j < len; j++)
        {
            dbl_array[j] = strtod(array[j], &idk);
        }
        quicksort(dbl_array, len, sizeof(double), dbl_cmp);
        for (size_t j = 0; j < len; j++)
        {
            printf("%f\n", dbl_array[j]);
        }
        for (size_t i = 0; i < len; i++)
        {
            free(array[i]);
        }
        free(array);
        free(dbl_array);
        return EXIT_SUCCESS;
    }
}
