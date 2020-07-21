/*******************************************************************************
 * Name        : lab2.c
 * Author      : Michael DelGaudio & Sunmin Lee
 * Date        : 05/28/2020
 * Description : Practice with pointers and strings.
 * Pledge : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#define BUFLEN 128

/* Function prototypes */
size_t my_strlen(char *src);
char *my_strcpy(char *dst, char *src);

/**
 * Accepts user input from standard in, copies it to a new string, and prints
 * out the copied string and string length. You are NOT ALLOWED to use any 
 * functions from <string.h>.
 */
int main()
{
    char src[BUFLEN];
    src[0] = '\0';
    char *copy;

    printf("Enter a string: ");

    if (scanf("%[^\n]", src) < 0)
    {
        fprintf(stderr, "Error: Failed to get a string from standard in.\n");
        return EXIT_FAILURE;
    }

    /* TODO: Implement the following features before the return. */

    /* 1. Using malloc, allocate enough space in 'copy' to fit 'src'.
          (man 3 malloc) */
    size_t length = my_strlen(src);
    if ((copy = (char *)malloc(length * sizeof(char))) == NULL)
    {
        fprintf(stderr, "Error: Null.\n");
        return EXIT_FAILURE;
    }
    /* TODO: These function calls should not crash your program if you handled
             NULLs correctly. Do not delete them! */
    my_strlen(NULL);
    if (my_strcpy(copy, NULL))
    {
        fprintf(stderr, "Error: This line should not have executed!\n");
    }

    /* 2. Call my_strcpy to make a copy of src, stored in copy. */
    my_strcpy(copy, src);
    /*
     * 3. Print out the following, matching the format verbatim:
     * Duplicated string: some_string
     * Length: some_length
     */
    size_t lengthOfCopy = my_strlen(copy);
    printf("Duplicated string: %s\n", copy);
    printf("Length: %zu\n", lengthOfCopy);
    /* 
     * 4. Deallocate (free) any memory allocated with malloc/calloc/realloc/etc.
     */
    free(copy);
    return EXIT_SUCCESS;
}

/**
 * This function calculates the length of a character array passed in,
 * returning the length as a size_t. Calculation must be done via pointer
 * arithmetic. No credit will be given for calling strlen() in <string.h>.
 *
 * src: character array, possibly NULL (which will be considered length 0)
 *
 * return: length of src as a size_t
 */
size_t my_strlen(char *src)
{
    size_t final = 0;
    if (src == NULL)
    {
        return final;
    }
    while (*src++ != '\0')
    {
        final++;
    }

    return final;
}

/**
 * This function copies a character array src into dest. The caller must ensure
 * that the length of dst is at least length of src plus one. This function then
 * returns a pointer to dst. Computation must be done via pointer arithmetic.
 * No credit will be given for calling memcpy() in <string.h>.
 *
 * dst: target character array
 * src: source character array, possibly NULL
 *
 * return: pointer to dst
 *         If src is NULL, return NULL.
 */
char *my_strcpy(char *dst, char *src)
{
    if (src == NULL)
    {
        return NULL;
    }
    char *final = dst;
    while (*src != '\0')
    {
        *dst++ = *src++;
    }
    *dst = '\0';
    return final;
}
