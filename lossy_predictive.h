#ifndef H_LOSSY
#define H_LOSSY

#include <iostream>
#include "predictor.h"
#include "Golomb/golomb.h"
#include "bit_stream/bit_stream.h"
#include <sndfile.h>
#include <map>


using namespace std;

class lossy_predictive {
    public:
        lossy_predictive(char* fname,bool hist){
            calc_hist = hist;
            filename = fname;
            m = 0;
            entropy = 0;
        };

        void lossypredictive_encode(char* outfile, int qtbits);
        void lossypredictive_decode(char* infile);
        void setM(uint m);
        int quantize(int sample,int nbits);
        double getEntropy();
        
    private:
        bool calc_hist;
        int m;
        char* filename;
        map <double, int> histogram_residual;
        double entropy;
};

#endif