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
            rem_size=0;
            unary_size=0;
            //memset(code,0,sizeof(uint8_t)*(uint)ceil(log2(m)));
        };
        golomb(uint val,char* fname)
        {
            filename = fname;
            m = val;
            stream = bit_stream(fname,true,true);
            rem_size=0;
            unary_size=0;
        }
        golomb(){};
        //~golomb();
        
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
        
        void close_stream_write();
        void close_stream_read();
        bool end_of_file();

    private:
        uint m;
        uint unary_size;
        uint rem_size;
        char* filename;
        bit_stream stream;
};
#endif