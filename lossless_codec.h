#include <string>
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;
class lossless_codec{
    public:
        lossless_codec(string fname){
            //m=m_val;
            filename = fname;
        };
        /**
         * @brief 
         * 
         * @param image original image matrix
         * @param y address for the matrix y
         * @param u address for the matrix u
         * @param v address for the matrix v
         */
        void YUV(Mat image, Mat &y, Mat &u, Mat &v);
        /**
         * @brief 
         * 
         * @param valorPixel pixel value
         * @param valorPrevisto expected pixel value
         * @return int -> residual for each value of the predicted pixel
         */
        int erroEnc (int valorPixel, int valorPrevisto);
        /**
         * @brief 
         * 
         * @param erro residual
         * @param valorPrevisto expected pixel value
         * @return int -> pixel value for each value of the predicted pixel
         */
        int erroDec (int erro, int valorPrevisto);
        /**
         * @brief 
         * 
         * @param a parameter a to calculate the expected pixel value
         * @param b parameter b to calculate the expected pixel value
         * @param c parameter c to calculate the expected pixel value
         * @return int -> the predicted value of the next pixel
         */
        int preditor (int a, int b, int c);
        /**
         * @brief 
         * 
         * @param matriz matrix to calculate the calculated error
         * @param erMat matrix with the calculated error
         */
        void preditor_JPEG_LS (Mat matriz,Mat &erMat);
        /**
         * @brief 
         * 
         * @param erro residual
         * @param m golomg parameter m
         */
        void golombfuction(int erro,int m);
        private:
            //int m;
            string filename;
};
//#endif