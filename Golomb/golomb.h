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
            code = malloc((sizeof(uint8_t)*(uint)ceil(log2(m))));
            m=val;
            //memset(code,0,sizeof(uint8_t)*(uint)ceil(log2(m)));
        };

        char* signed_encode(uint n);
        char* signed_decode(uint q,uint r);
        int get_m();
        void set_m(int new_m);
        uint get_unarySize();
        uint get_remSize();


    private:
        uint m;
        void* code;
        uint unary_size = 0;
        uint rem_size = 0;

};
#endif