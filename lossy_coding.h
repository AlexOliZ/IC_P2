#include <string>
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;
class  lossy_coding{
    public:
         lossy_coding(string fname){
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
        int ValorPixelDec (int erro, int valorPrevisto, int quantization);
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
         * @param prev matrix with the calculated predictor
         * @param quantization quantization parameter set as program argument
         */
        void preditor_JPEG_LS (Mat matriz,Mat &erMat, Mat &prev, int quantization);
        /**
         * @brief 
         * 
         * @param erro residual
         * @param m golomg parameter m
         */
        void golombEnc(int erro,int m);

        void golombDesc(Mat prevY, Mat prevV, Mat prevU, Mat erroY, Mat erroV, Mat erroU, Mat &ValorY, Mat &ValorV, Mat &ValorU, int quantization);

        private:
            //int m;
            string filename;
};
//#endif
