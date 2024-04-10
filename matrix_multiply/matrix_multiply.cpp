#include "matrix_multiply.hpp"

/*
    Compute a vector-vector scalar (dot) product
            in1 --> Input Vector
            in2 --> Input Vector
            output --> Output Scalar
*/
uint8_t dot_product(
                    hls::vector<uint8_t, 4> in1,
                    hls::vector<uint8_t, 4> in2) {
    #pragma HLS pipeline II=1

    // First compute the SIMD products across the two vectors
    hls::vector<uint8_t, 4> product = in1 * in2;

    // Next compute the sum of the products in parallel
    uint8_t result = 0;
    for(int i = 0; i < 4; i++){
        #pragma HLS UNROLL
        result += product[i];
    }
    // Finally return the result to the caller
    return result;
}

// Read Data from Global Memory and write 8-wide vectors into Stream inStream
static void read_input(uint64_t* in, hls::stream<hls::vector<uint8_t, 8> >& inStream, int vSize) {
// Auto-pipeline is going to apply pipeline to this loop
mem_rd:
    for (int i = 0; i < vSize; i++) {
    #pragma HLS LOOP_TRIPCOUNT min = size max = size
    #pragma HLS pipeline II=1

        // Here we read 64-bit values from memory and represent them in a vector datatype
        uint64_t tmp_input = in[i];
        hls::vector<uint8_t, 8> tmp_vec;
        for(int j = 0; j < 8; j++){
            #pragma HLS UNROLL
            tmp_vec[j] = (tmp_input >> (8 * j)) & 0xFF;
        }
        inStream << tmp_vec;
    }
}


// Read Input data from inStream1 and inStream2,
// compute matrix multiplication and write the result into outStream
static void compute_matrix_multiplication(
    hls::stream<hls::vector<uint8_t, 8> >& inStream1,
    hls::stream<hls::vector<uint8_t, 8> >& inStream2,
    hls::stream<hls::vector<uint8_t, 4> >& outStream,
    int vSize) {
// Auto-pipeline is going to apply pipeline to this loop
execute:
    for (int i = 0; i < vSize; i++) {
    #pragma HLS pipeline II=1
    #pragma HLS LOOP_TRIPCOUNT min = size max = size
    
        hls::vector<uint8_t, 8> in1 = inStream1.read();
        hls::vector<uint8_t, 8> in2 = inStream2.read();

        // Select rows / columns
        hls::vector<uint8_t, 4> A0;
        A0[0] = in1[0];
        A0[1] = in1[1];
        A0[2] = in1[2];
        A0[3] = in1[3];
        hls::vector<uint8_t, 4> B0;
        B0[0] = in2[0];
        B0[1] = in2[1];
        B0[2] = in2[2];
        B0[3] = in2[3];
        hls::vector<uint8_t, 4> A1;
        A1[0] = in1[4];
        A1[1] = in1[5];
        A1[2] = in1[6];
        A1[3] = in1[7];
        hls::vector<uint8_t, 4> B1;
        B1[0] = in2[4];
        B1[1] = in2[5];
        B1[2] = in2[6];
        B1[3] = in2[7];

        // Call dot-product 4 times
        hls::vector<uint8_t, 4> out;
        out[0] = dot_product(A0, B0);
        out[1] = dot_product(A0, B1);
        out[2] = dot_product(A1, B0);
        out[3] = dot_product(A1, B1);
        
        outStream << out;
    }
}

// Read result from outStream and write the result to Global Memory
static void write_result(
    uint32_t* out,
    hls::stream<hls::vector<uint8_t, 4> >& outStream,
    int vSize) {
mem_wr:
    for (int i = 0; i < vSize; i++) {
    #pragma HLS LOOP_TRIPCOUNT min = size max = size
        // Blocking read command to inStream
        hls::vector<uint8_t, 4> tmp_vec = outStream.read();

        uint64_t tmp_result = 0;
        for(int j = 0; j < 4; j++){
            #pragma HLS UNROLL
            tmp_result |= (((uint64_t)tmp_vec[j]) << (8 * j));
        }

        out[i] = tmp_result;
    }
}

extern "C" {
    /*
        Single-Cycle 8-bit 4x2, 2x4 Matrix Multiplication Engine:
            in1   (input)  --> Input Matrix 
            in2   (input)  --> Input Matrix
            out  (output) --> Output Matrix
            vSize (input)  --> Size of Vector in Integer
    */
    void matrix_multiply(
        uint64_t* in1,
        uint64_t* in2,
        uint32_t* out,
        int vSize) {
        static hls::stream<hls::vector<uint8_t, 8> > 
            inStream1("input_stream_1");
        static hls::stream<hls::vector<uint8_t, 8> >
            inStream2("input_stream_2");
        static hls::stream<hls::vector<uint8_t, 4> > 
            outStream("output_stream");
        #pragma HLS INTERFACE m_axi port = in1 bundle = gmem0 depth = size
        #pragma HLS INTERFACE m_axi port = in2 bundle = gmem1 depth = size
        #pragma HLS INTERFACE m_axi port = out bundle = gmem0 depth = size
        #pragma HLS dataflow

        // dataflow pragma instructs compiler to run following 
        // four functions in parallel
        read_input(in1, inStream1, vSize);
        read_input(in2, inStream2, vSize);
        compute_matrix_multiplication(inStream1, inStream2, outStream, vSize);
        write_result(out, outStream, vSize);
    }
}
