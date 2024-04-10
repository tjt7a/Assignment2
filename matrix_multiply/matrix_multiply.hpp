#ifndef _MATRIX_MULTIPLY_H_
#define _MATRIX_MULTIPLY_H_

// Includes
#include <iostream>
#include <hls_stream.h>
#include <hls_vector.h>
#include <stdlib.h>

const int size = 4096;

extern "C" {
        void matrix_multiply(uint64_t* input1, uint64_t* input2, uint32_t* output, int vSize);
}

#endif
