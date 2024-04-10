#include "matrix_multiply.hpp"

uint8_t dot_product(uint8_t in1[4], uint8_t in2[4]) {

    uint8_t result = 0;

    // Compute the SIMD product across the two vectors and sum partials
    for(int i = 0; i < 4; i++){
        result += in1[i] * in2[i];
    }

    // Finally return the result to the caller
    return result;
}


int main() {

    // Arrays for kernel input1, input2, and output
    //  and our expected output
    uint64_t in1[size], in2[size];
    uint32_t out[size], expectation[size];
    uint8_t A[8], B[8];

    // Because we're going to process 8 8-bit additions in parallel, we're going to pack 8 8-bit values per 64-bit input

    for(int i = 0; i < size; i++){
        uint64_t tmp1 = 0;
        uint64_t tmp2 = 0;
        uint32_t tmpe = 0;

        // Choose two random values (0-16), concatenate them into 64-bit words
        for (int j = 7; j >= 0; j--) {
            uint64_t random_value_1 = rand() % 16;            
            tmp1 |= (random_value_1 << (8 * j));
            A[7-j] = random_value_1;

            uint64_t random_value_2 = rand() % 16;
            tmp2 |= (random_value_2 << (8 * j));
            B[7-j] = random_value_2;

        }

        uint8_t A0[4] = {A[0], A[1], A[2], A[3]};
        uint8_t B0[4] = {B[0], B[1], B[2], B[3]};
        uint8_t A1[4] = {A[4], A[5], A[6], A[7]};
        uint8_t B1[4] = {B[4], B[5], B[6], B[7]};


        in1[i] = tmp1; 
        in2[i] = tmp2;
        out[i] = 0;

        // Each output is one 32-bit value
        expectation[i] = 0 | ((uint32_t)dot_product(A0, B0) << 24) | ((uint32_t)dot_product(A0, B1) << 16) | ((uint32_t)dot_product(A1, B0) << 8) | (uint32_t)dot_product(A1, B1);
    }

    // Call the vector_add kernel
    matrix_multiply(in1, in2, out, size);

    // Evaluate each byte returned from the kernel
    for (int i = 0; i < size; ++i) {

        std::cout << "[[ ";

        for(int j = 7; j >= 4; j--){
            uint8_t in1_tmp = (in1[i] >> (j * 8)) & 0xff;
            std::cout << (int)in1_tmp << " ";
        }

        std::cout << "][";

        for(int j = 3; j >= 0; j--){
            uint8_t in1_tmp = (in1[i] >> (j * 8)) & 0xff;
            std::cout << (int)in1_tmp << " ";
        }

        std::cout << "] X [[";

        for(int j = 7; j >= 4; j--){
            uint8_t in2_tmp = (in2[i] >> (j * 8)) & 0xff;
            std::cout << (int)in2_tmp << " ";
        }

        std::cout << "][";

        for(int j = 3; j >=0; j--){
            uint8_t in2_tmp = (in2[i] >> (j * 8)) & 0xff;
            std::cout << (int)in2_tmp << " ";
        }

        std::cout << "]]=" << std::endl;


        std::cout << "\tKernel: [[";

        for(int j = 3; j >= 2; j--){
            uint8_t out_tmp = (out[i] >> (j * 8)) & 0xff;
            std::cout << (int)out_tmp << " ";
        }

        std::cout << "][";

        for(int j = 1; j >= 0; j--){
            uint8_t out_tmp = (out[i] >> (j * 8)) & 0xff;
            std::cout << (int)out_tmp << " ";
        }

        std::cout << "]" << std::endl;

        std::cout << "\tExpectation: [[";

        for(int j = 3; j >= 2; j--){
            uint8_t out_tmp = (expectation[i] >> (j * 8)) & 0xff;
            std::cout << (int)out_tmp << " ";
        }

        std::cout << "][";

        for(int j = 1; j >= 0; j--){
            uint8_t out_tmp = (expectation[i] >> (j * 8)) & 0xff;
            std::cout << (int)out_tmp << " ";
        }

        std::cout << "]]" << std::endl;

        if (expectation[i] != out[i])
            return EXIT_FAILURE;
    }

    std::cout << "Test passed.\n";
    return EXIT_SUCCESS;
}
