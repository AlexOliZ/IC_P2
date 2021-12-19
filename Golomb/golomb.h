#ifndef H_GOLOM
#define H_GOLOM

#include <tuple>
#include <cmath>
#include <iostream>
#include <string.h>
#include "../bit_stream/bit_stream.h"

using namespace std;

class golomb
{
    public:
        golomb(uint val){
            m=val;
            //memset(code,0,sizeof(uint8_t)*(uint)ceil(log2(m)));
        };
        golomb(uint val,char* fname)
        {
            filename = fname;
            m = val;
            stream = bit_stream(fname,true,true);
        }
        golomb(){};
        //~golomb();
        
        void free_code();
        int get_m();
        void set_m(int new_m);
        uint get_unarySize();
        uint get_remSize();

        char* encode(uint n);
        char* signed_encode(int n);
        uint decode(char* code,uint remainder, uint unary_size);
        int signed_decode(char*code,uint remainder_size,uint unary_size);
        
        void stream_encode(uint n);
        void signed_stream_encode(int n);
        uint stream_decode();
        int signed_stream_decode();
        
        void close_stream();
        bool end_of_file();

    private:
        uint m;
        char* code;
        uint unary_size = 0;
        uint rem_size = 0;
        char* filename;
        bit_stream stream;
};
#endif