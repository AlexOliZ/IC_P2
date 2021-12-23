#ifndef H_PREDICTOR
#define H_PREDICTOR

#include <iostream>

using namespace std;

class predictor {
    public:
        predictor(bool lossy){
            this->lossy = lossy;
            this->num_inputs = 0;
            this->pointer = 0;
        };

        int predict();
        int residual(int sample);
        int reconstruct(int residual);
        void updateBufferConst(int quant);
        void showBuffer();
        void  updateBuffer(int sample);
        
    private:
        bool lossy;
        int num_inputs;
        short pointer;
        short buffer[3];
};

#endif