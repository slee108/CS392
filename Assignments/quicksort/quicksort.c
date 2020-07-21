/*******************************************************************************
 * Name        : quicksort.c
 * Author      : Michael DelGaudio & Sunmin Lee
 * Date        : 5/27/2020
 * Description : Quicksort implementation.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "quicksort.h"

/* Static (private to this file) function prototypes. */
static void swap(void *a, void *b, size_t size);
static int lomuto(void *array, int left, int right, size_t elem_sz,
                  int (*comp)(const void *, const void *));
static void quicksort_helper(void *array, int left, int right, size_t elem_sz,
                             int (*comp)(const void *, const void *));

/**
 * Compares two integers passed in as void pointers and returns an integer
 * representing their ordering.
 * First casts the void pointers to int pointers.
 * Returns:
 * -- 0 if the integers are equal
 * -- a positive number if the first integer is greater
 * -- a negative if the second integer is greater
 */
int int_cmp(const void *a, const void *b)
{
    if (*(int *)a == *(int *)b)
    {
        return 0;
    }
    else if (*(int *)a > *(int *)b)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

/**
 * Compares two doubles passed in as void pointers and returns an integer
 * representing their ordering.
 * First casts the void pointers to double pointers.
 * Returns:
 * -- 0 if the doubles are equal
 * -- 1 if the first double is greater
 * -- -1 if the second double is greater
 */
//RESOURCE CITED: https://www.reddit.com/r/C_Programming/comments/4thsn7/comparing_doubles_in_c/
int dbl_cmp(const void *a, const void *b)
{
    //We may have to use DBL_EPSILON due to precision points for equality
    double x = *(double *)a;
    double y = *(double *)b;
    double z = fabs(x - y);
    if (z < __DBL_EPSILON__)
    {
        return 0;
    }
    // if (*(double *)a == *(double *)b)
    // {
    //     return 0;
    // }
    else if (*(double *)a > *(double *)b)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

/**
 * Compares two char arrays passed in as void pointers and returns an integer
 * representing their ordering.
 * First casts the void pointers to char* pointers (i.e. char**).
 * Returns the result of calling strcmp on them.
 */
int str_cmp(const void *a, const void *b)
{
    return strcmp(*(char **)a, *(char **)b);
}

/**
 * Swaps the values in two pointers.
 *
 * Casts the void pointers to character types and works with them as char
 * pointers for the remainder of the function.
 * Swaps one byte at a time, until all 'size' bytes have been swapped.
 * For example, if ints are passed in, size will be 4. Therefore, this function
 * swaps 4 bytes in a and b character pointers.
 */
static void swap(void *a, void *b, size_t size)
{
    char *fst = (char *)a;
    char *snd = (char *)b;
    for (size_t len = 0; len < size; len++)
    {
        char temp = *(fst + len);
        *(fst + len) = *(snd + len);
        *(snd + len) = temp;
    }
}

/**
 * Partitions array around a pivot, utilizing the swap function.
 * Each time the function runs, the pivot is placed into the correct index of
 * the array in sorted order. All elements less than the pivot should
 * be to its left, and all elements greater than or equal to the pivot should be
 * to its right.
 * The function pointer is dereferenced when it is used.
 * Indexing into void *array does not work. All work must be performed with
 * pointer arithmetic.
 */
static int lomuto(void *array, int left, int right, size_t elem_sz,
                  int (*comp)(const void *, const void *))
{
    //Should we utilize parens for PEMDAS?
    char *arr = (char *)array;
    char *pivot = arr + (left * elem_sz);
    int i = left;
    for (int j = left + 1; j <= right; j++)
    {
        if (comp(arr + j * elem_sz, pivot) < 0)
        {
            i++;
            swap(arr + i * elem_sz, arr + j * elem_sz, elem_sz);
        }
    }
    swap(arr + i * elem_sz, pivot, elem_sz);
    return i;
}

/**
 * Sorts with lomuto partitioning, with recursive calls on each side of the
 * pivot.
 * This is the function that does the work, since it takes in both left and
 * right index values.
 */
static void quicksort_helper(void *array, int left, int right, size_t elem_sz,
                             int (*comp)(const void *, const void *))
{
    if (left < right)
    {
        int pivot = lomuto(array, left, right, elem_sz, comp);
        quicksort_helper(array, left, pivot - 1, elem_sz, comp);
        quicksort_helper(array, pivot + 1, right, elem_sz, comp);
    }
}

/**
 * Quicksort function exposed to the user.
 * Calls quicksort_helper with left = 0 and right = len - 1.
 */
void quicksort(void *array, size_t len, size_t elem_sz,
               int (*comp)(const void *, const void *))
{
    quicksort_helper(array, 0, len - 1, elem_sz, comp);
}
