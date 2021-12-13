#include "predictor.h"


short predictor::predict()
{
    if(num_inputs == 0){
        return 0;
    } else if (num_inputs == 1) {
        return buffer[pointer];
    } else if (num_inputs == 2) {
        return 2*buffer[pointer] - 2*buffer[pointer-1];
    } 
    return 3*buffer[pointer] - 3*buffer[pointer-1] + buffer[pointer-2];
}

int predictor::residual(short sample)
{
    short val = predict(); // buffer[pointer]
    updateBuffer(sample);
    return sample-val;   
}

short predictor::reconstruct(int residual)
{
    int val_res = predict() + residual;
    updateBuffer(val_res);
    return val_res;
}

void predictor::updateBufferConst(short quant)
{
    updateBuffer(predict()+quant);
}

void predictor::updateBuffer(short sample) {
    pointer = (pointer+1)&0x03;
    buffer[pointer] = sample;
    num_inputs++;
}

void predictor::showBuffer()
{
    cout << buffer[0] << ',' << buffer[1] << ',' <<  buffer[2] << endl;
}