#ifndef H_GOLOMB
#define H_GOLOMB

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
        }
        golomb(){};
        //~golomb();
        
        void free_code();
        char* encode(uint n);
        char* signed_encode(int n);
        //char* decode(uint q,uint r);
        //uint decode(char* code);
        int get_m();
        void set_m(int new_m);
        uint get_unarySize();
        uint get_remSize();
        uint decode(char* code,uint remainder, uint unary_size);
        int signed_decode(char*code,uint remainder_size,uint unary_size);
        void stream_encode(uint n);
        void signed_stream_encode(uint n);
        uint stream_decode();
        int signed_stream_decode();

    private:
        uint m;
        char* code;
        uint unary_size = 0;
        uint rem_size = 0;
        char* filename;

};
#endif