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
            entropy = 0;
        };

        SF_INFO predictive_encode(char* outfile);
        void predictive_decode(char* infile,SF_INFO num);
        void setM(uint m);
        double getEntropy();
        void dispHistogram();
        
    private:
        bool calc_hist;
        int m;
        char* filename;
        map <double, int> histogram_residual;
        double entropy;

};

#endif