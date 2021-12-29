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
        /**
         * @brief Construct a new lossy predictive object
         * 
         * @param fname soundfile to be read
         * @param hist flag to determine whether histograms are calculated or not
         * @param bits number of bits to quantize the audio
         */
        lossy_predictive(char* fname,bool hist, int bits){
            qtbits = bits;
            calc_hist = hist;
            filename = fname;
            m = 0;
            entropy_original = 0;
            entropy_residual = 0;
        };
        /**
         * @brief Reads the soundfile, calculates the residuals and then sends to the bitstream each sample coded in golomb coding.
         * 
         * @param outfile file to where the bitstream writes
         */
        void lossypredictive_encode(char* outfile);
        /**
         * @brief Decodes the binary file throgh the bitstream and then recontructs, with the predictor, the original values
         * In the end, writes in a new wav file the values retrieved in the previous step
         *          
         * @param infile binary file containing the residuals coded in golomb coding
         */
        void lossypredictive_decode(char* infile);
        /**
         * @brief function used to change directly the m used in the golomb module
         * 
         * @param m the value that will be set to the variable m
         */
        void setM(uint m);
        /**
         * @brief Takes a audio sample and converts it to the nearest quantize step
         * 
         * @param sample audio sample given
         * @param nbits number of bits to quantize the audio sample
         * @return Return ths audio sample quantized to nbits
         */
        int quantize(int sample,int nbits);
        /**
         * @brief Returns the Entropy value of the original values
         * 
         * 
         * @return Entropy calculated from the original file
         */
        double getEntropyOriginal();
        /**
         * @brief Returns the Entropy value of the residual values
         * 
         * 
         * @return Entropy calculated from the original file
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
        int qtbits;
        char* filename;
        map <int, int> histogram_residual;
        map <int, int> histogram_original;
        double entropy_residual;
        double entropy_original;
};

#endif