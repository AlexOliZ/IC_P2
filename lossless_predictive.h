#ifndef H_LOSSLESS
#define H_LOSSLESS

#include <iostream>
#include "predictor.h"
#include <sndfile.h>


using namespace std;

class lossless_predictive {
    public:
        lossless_predictive(char* fname ,uint m_val){
            m=m_val;
            filename = fname;
        };

        void predictive_encode(predictor p,char* outfile);
        void predictive_decode(predictor p,char* infile);
        void setM(uint m);
        
    private:
        int m;
        char* filename;
};

#endif