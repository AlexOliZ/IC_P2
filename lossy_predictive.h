#ifndef H_LOSSY
#define H_LOSSY

#include <iostream>
#include "predictor.h"
#include "Golomb/golomb.h"
#include "bit_stream/bit_stream.h"
#include <sndfile.h>


using namespace std;

class lossy_predictive {
    public:
        lossy_predictive(char* fname){
            filename = fname;
        };

        void lossypredictive_encode(char* outfile);
        void lossypredictive_decode(char* infile);
        void setM(uint m);
        int quantize(int sample,int nbits);
        
    private:
        int m;
        char* filename;
};

#endif