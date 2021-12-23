#include "predictor.h"


short predictor::predict()
{
    if(this->num_inputs == 0){
        return 0;
    } else if (this->num_inputs == 1) {
        return this->buffer[this->pointer];
    } else if (this->num_inputs == 2) {
        return 2*this->buffer[this->pointer] - 2*this->buffer[this->pointer-1];
    } 
    return 3*this->buffer[this->pointer] - 3*this->buffer[pointer-1] + this->buffer[pointer-2];
}

int predictor::residual(short sample)
{
    short val = predict(); // buffer[pointer]
    if(!lossy){
        updateBuffer(sample);
    }
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
    this->pointer = (pointer+1)%3;
    this->buffer[pointer] = sample;
    this->num_inputs++;
}

void predictor::showBuffer()
{
    cout << this->buffer[0] << ',' << this->buffer[1] << ',' <<  this->buffer[2] << endl;
}
//int main(void)
//{return 0;}