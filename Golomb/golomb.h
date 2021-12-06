#ifndef H_GOLOMB
#define H_GOLOMB

#include <tuple>
#include <cmath>
#include <vector>
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

        void free_code();
        char* encode(uint n);
        char* signed_encode(int n);
        char* decode(uint q,uint r);
        int get_m();
        void set_m(int new_m);
        uint get_unarySize();
        uint get_remSize();


    private:
        uint m;
        char* code;
        uint unary_size = 0;
        uint rem_size = 0;

};
#endif