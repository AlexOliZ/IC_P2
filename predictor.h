#ifndef H_PREDICTOR
#define H_PREDICTOR

#include <iostream>

using namespace std;

class predictor {
    public:
        predictor(char *fname){
            num_inputs = 0;
            pointer = 0;
            filename = fname;
        };

        short predict();
        int residual(short sample);
        short reconstruct(int residual);
        void updateBufferConst(short quant);
        void showBuffer();
        void  updateBuffer(short sample);
        
    private:
        int num_inputs;
        short pointer;
        short buffer[3] = {0,0,0};
        char* filename;
};

#endif