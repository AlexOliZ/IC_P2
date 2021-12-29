#ifndef H_LOSSLESS
#define H_LOSSLESS

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

class lossless_predictive {
    public:
        lossless_predictive(char* fname,bool hist){
            calc_hist = hist;
            m = 0;
            filename = fname;
            entropy_residual = 0;
            entropy_original = 0;
        };

        SF_INFO predictive_encode(char* outfile);
        void predictive_decode(char* infile,SF_INFO num);
        void setM(uint m);
        double getEntropyOriginal();
        double getEntropyResidual();
        void dispHistogram();
        
    private:
        bool calc_hist;
        uint m;
        char* filename;
        map <int, int> histogram_original;
        map <int, int> histogram_residual;
        double entropy_residual;
        double entropy_original;

};

#endif