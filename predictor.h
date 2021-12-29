#ifndef H_PREDICTOR
#define H_PREDICTOR

#include <iostream>

using namespace std;

class predictor {
    public:
        /**
         * @brief Construct a new predictor object
         * 
         * @param lossy flags to determine if the predictive using is losing or not
         */
        predictor(bool lossy){
            this->lossy = lossy;
            this->num_inputs = 0;
            this->pointer = 0;
        };
        /**
         * @brief Predicts the next value using a set of equations
         * 
         * @return The predicted value
         */
        int predict();
        /**
         * @brief Calculates the residual of the audio sample
         * 
         * @param sample Audio sample given
         * @return Returns the residual value of the given sample 
         */
        int residual(int residual);
        /**
         * @brief Reconstructs the original sample from the residual given
         * 
         * @param residual Residual given
         * @return int 
         */
        int reconstruct(int residual);
        /**
         * @brief updates the buffer, this is used for lossy only
         * 
         * @param quant The value to be given to the buffer
         */
        void updateBufferConst(int quant);
        /**
         * @brief Displays the contents of the buffer
         * 
         */
        void showBuffer();
        /**
         * @brief Updates the buffer with the new calculated value
         * 
         * @param sample 
         */
        void  updateBuffer(int sample);
        
    private:
        bool lossy;
        int num_inputs;
        short pointer;
        int buffer[3];
};

#endif