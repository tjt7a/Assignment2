#include "vector_addition.hpp"

int main() {

    // Arrays for input1, input2, and our expected output
    uint64_t in1[size], in2[size], out[size], expectation[size];

    // Because we're going to process 8 8-bit additions in parallel, we're going to pack 8 8-bit values per 64-bit input
    for(int i = 0; i < size; i++){
        uint64_t tmp1 = 0;
        uint64_t tmp2 = 0;
        uint64_t tmpe = 0;

        // Choose two random values (0-255), concatenate them into 64-bit words
        for (int j = 0; j < 8; j++) {
            uint64_t random_value_1 = rand() % 256;            
            tmp1 |= (random_value_1 << (8 * j));

            uint64_t random_value_2 = rand() % 256;
            tmp2 |= (random_value_2 << (8 * j));

            uint64_t random_sum = (random_value_1 + random_value_2) % 256;
            tmpe |= (random_sum << (8 * j));
        }

        in1[i] = tmp1; 
        in2[i] = tmp2; // Odd
        out[i] = 0;
        expectation[i] = tmpe;
    }

    // Call the vector_add kernel
    vector_add(in1, in2, out, size);

    // Evaluate each byte returned from the kernel
    for (int i = 0; i < size; ++i) {
        for(int j = 0; j < 8; j++){
            uint8_t in1_tmp = (in1[i] >> (j * 8)) & 0xff;
            uint8_t in2_tmp = (in2[i] >> (j * 8)) & 0xff;
            uint8_t out_tmp = (out[i] >> (j * 8)) & 0xff;
            uint8_t exp_tmp = (expectation[i] >> (j * 8)) & 0xff;
            std::cout << "[" << i << "][" << j << "] " << std::endl;
            std::cout << "\tin1: " << (int)in1_tmp << ", in2: " << (int)in2_tmp << std::endl;
            std::cout << "\tout: " << (int)out_tmp << ", expectation: " << (int)exp_tmp << std::endl;
        }
        if (expectation[i] != out[i])
            return EXIT_FAILURE;
    }

    std::cout << "Test passed.\n";
    return EXIT_SUCCESS;
}
