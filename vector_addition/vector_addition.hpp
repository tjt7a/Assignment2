#ifndef _VECTOR_ADDITION_H_
#define _VECTOR_ADDITION_H_

// Includes
#include <iostream>
#include <hls_stream.h>
#include <hls_vector.h>
#include <stdlib.h>

const int size = 4096;

extern "C" {
        void vector_add(uint64_t* input1, uint64_t* input2, uint64_t* output, int vSize);
}

#endif
