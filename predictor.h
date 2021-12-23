#ifndef H_PREDICTOR
#define H_PREDICTOR

#include <iostream>

using namespace std;

class predictor {
    public:
        predictor(bool lossy){
            this->lossy = lossy;
            num_inputs = 0;
            pointer = 0;
        };

        short predict();
        int residual(short sample);
        short reconstruct(int residual);
        void updateBufferConst(short quant);
        void showBuffer();
        void  updateBuffer(short sample);
        
    private:
        bool lossy;
        int num_inputs;
        short pointer;
        short buffer[3];
};

#endif