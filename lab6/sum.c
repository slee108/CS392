/*******************************************************************************
 * Name        : sum.c
 * Author      : Michael DelGaudio & Sunmin Lee
 * Date        : 06/18/2020
 * Description : sum file.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include "sum.h"

/**
 * TODO:
 * Takes in an array of integers and its length.
 * Returns the sum of integers in the array.
 */
int sum_array(int *array, const int length) {
	int result=0; 
	for(int i = 0; i < length; i++){
		result += array[i];
	}
	return result;
}