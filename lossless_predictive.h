#ifndef H_LOSSLESS
#define H_LOSSLESS

#include <iostream>
#include "predictor.h"
#include "Golomb/golomb.h"
#include "bit_stream/bit_stream.h"
#include <sndfile.h>


using namespace std;

class lossless_predictive {
    public:
        lossless_predictive(char* fname){
            //m=m_val;
            filename = fname;
        };

        void predictive_encode(char* outfile);
        void predictive_decode(char* infile);
        void setM(uint m);
        
    private:
        int m;
        char* filename;
};

#endif