/*
	Matrix Multiplication program to generate some kind of CPU processing
	to monitor data from tasks in the L4re uKOS.
*/

#include <cstdlib>
#include <base/printf.h>
#include <timer_session/connection.h>

int main(void) {
	int m, c, d, k, sum = 0;
	int first[100][100], second[100][100], multiply[100][100];

	// seed for the random number generator that gets the time and 
	// size of rows and columns for the matrix
	// we are using square matrices to start with.
	m = 100;

	// generate random elements for the matrix
	for (c = 0; c < m; c++) {
		for (d = 0; d < m; d++) {
			first[c][d] = rand() % 100;
			second[c][d] = rand() % 100;
		}
	}

	// MUTIPLY the matrices, finalllly
	for (c = 0; c < m; c++) {
		for (d = 0; d < m; d++) {
			for (k = 0; k < m; k++) {
				sum = sum + first[c][k]*second[k][d];
			}

			multiply[c][d] = sum;
			sum = 0;
		}
	}

	// Print the matrices
	// not required for our purposes, just here randomly

	PINF("Tumatmul Done.\n");

	for (c = 0; c < m; c++) {
		for (d = 0; d < m; d++)
			PINF("%d\t", multiply[c][d]);
		PINF("\n");
	}

}
