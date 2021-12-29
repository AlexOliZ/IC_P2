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
        /**
         * @brief Construct a new lossless predictive object
         * 
         * @param fname soundfile to be read
         * @param hist flag to determine whether histograms are calculated or not
         */
        lossless_predictive(char* fname,bool hist){
            calc_hist = hist;
            m = 0;
            filename = fname;
            entropy_residual = 0;
            entropy_original = 0;
        };
        /**
         * @brief Reads the soundfile, calculates the residuals and then sends to the bitstream each sample coded in golomb coding.
         * 
         * @param outfile file to where the bitstream writes
         * @return SF_INFO information related to the sound file
         */
        SF_INFO predictive_encode(char* outfile);
        /**
         * @brief Decodes the binary file throgh the bitstream and then recontructs, with the predictor, the original values
         * In the end, writes in a new wav file the values retrieved in the previous step
         * 
         * @param infile binary file containing the residuals coded in golomb coding
         * @param info information related to the sound file
         */
        void predictive_decode(char* infile,SF_INFO info);
        /**
         * @brief function used to change directly the m used in the golomb module
         * 
         * @param m the value that will be set to the variable m
         */
        void setM(uint m);
        /**
         * @brief Returns the Entropy value of the original values 
         * 
         * @return Entropy calculated
         */
        double getEntropyOriginal();
        /**
         * @brief Returns the Entropy value of the residual values
         * 
         * @return Entropy calculated
         */
        double getEntropyResidual();
        /**
         * @brief Function that displays the Histogram of the Residual values
         * 
         */
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