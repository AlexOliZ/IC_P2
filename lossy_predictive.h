#ifndef H_LOSSY
#define H_LOSSY

#include <iostream>
#include "predictor.h"
#include "Golomb/golomb.h"
#include "bit_stream/bit_stream.h"
#include <sndfile.h>
#include <map>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

class lossy_predictive {
    public:
        lossy_predictive(char* fname,bool hist, int bits){
            qtbits = bits;
            calc_hist = hist;
            filename = fname;
            m = 0;
            entropy = 0;
        };

        void lossypredictive_encode(char* outfile);
        void lossypredictive_decode(char* infile);
        void setM(uint m);
        int quantize(int sample,int nbits);
        double getEntropy();
        void dispHistogram();
        
    private:
        bool calc_hist;
        int m,qtbits;
        char* filename;
        map <double, int> histogram_residual;
        double entropy;
};

#endif