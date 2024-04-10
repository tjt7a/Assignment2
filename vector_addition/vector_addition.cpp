#include "vector_addition.hpp"

// Read Data from Global Memory and write 8-wide vectors into Stream inStream
static void read_input(uint64_t* in, hls::stream<hls::vector<uint8_t, 8> >& inStream, int vSize) {
// Auto-pipeline is going to apply pipeline to this loop
mem_rd:
    for (int i = 0; i < vSize; i++) {
    #pragma HLS LOOP_TRIPCOUNT min = size max = size
    #pragma HLS pipeline II=1

        // Here we read 64-bit values from memory and represent them in a vector datatype
        // as 8x 8-bit values
        uint64_t tmp_input = in[i];
        hls::vector<uint8_t, 8> tmp_vec;
        for(int j = 0; j < 8; j++)
            tmp_vec[j] = (tmp_input >> (8 * j)) & 0xFF;
        inStream << tmp_vec; 
    }
}

// Read Input data from inStream and write the result into outStream
static void compute_addition(hls::stream<hls::vector<uint8_t, 8> >& inStream1,
                        hls::stream<hls::vector<uint8_t, 8> >& inStream2,
                        hls::stream<hls::vector<uint8_t, 8> >& outStream,
                        int vSize) {
// Auto-pipeline is going to apply pipeline to this loop
execute:
    for (int i = 0; i < vSize; i++) {
    #pragma HLS LOOP_TRIPCOUNT min = size max = size
    #pragma HLS pipeline II=1
        outStream << inStream1.read() + inStream2.read();
    }
}

// Read result from outStream and write the result to Global Memory
static void write_result(uint64_t* out, hls::stream<hls::vector<uint8_t, 8> >& outStream, int vSize) {
// Auto-pipeline is going to apply pipeline to this loop
mem_wr:
    for (int i = 0; i < vSize; i++) {
    #pragma HLS LOOP_TRIPCOUNT min = size max = size
    #pragma HLS pipeline II=1
        // Blocking read command to inStream
        hls::vector<uint8_t, 8> tmp_vec = outStream.read();
        
        uint64_t tmp_result = 0;
        for(int j = 0; j < 8; j++){
            tmp_result |= (((uint64_t)tmp_vec[j]) << (8 * j));
        }   
        
        out[i] = tmp_result;
    }
}

extern "C" {
    /*
        Vector Addition Kernel Implementation using dataflow
        Arguments:
            in1   (input)  --> Input Vector
            in2   (input)  --> Input Vector
            out  (output) --> Output Vector
            vSize (input)  --> Size of Vector in Integer
    */
    void vector_add(uint64_t* in1, uint64_t* in2, uint64_t* out, int vSize) {
        static hls::stream<hls::vector<uint8_t, 8> > inStream1("input_stream_1");
        static hls::stream<hls::vector<uint8_t, 8> > inStream2("input_stream_2");
        static hls::stream<hls::vector<uint8_t, 8> > outStream("output_stream");
        #pragma HLS INTERFACE m_axi port = in1 bundle = gmem0 depth = size
        #pragma HLS INTERFACE m_axi port = in2 bundle = gmem1 depth = size
        #pragma HLS INTERFACE m_axi port = out bundle = gmem0 depth = size
        #pragma HLS dataflow

        // dataflow pragma instruct compiler to run following three APIs in parallel
        read_input(in1, inStream1, vSize);
        read_input(in2, inStream2, vSize);
        compute_addition(inStream1, inStream2, outStream, vSize);
        write_result(out, outStream, vSize);
    }
}
