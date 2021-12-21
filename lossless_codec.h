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
        void RGB (Mat y, Mat u, Mat v, Mat &ImagemRBG);
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
        int ValorPixelDec (int erro, int valorPrevisto);
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
         */
        void preditor_JPEG_LS (Mat matriz,Mat &erMat, Mat &prev);
        /**
         * @brief 
         * 
         * @param erroY residual
         * @param erroV residual
         * @param erroU residual
         * @param m golomg parameter m
         * @param namefile 
         */
        void golombEnc(Mat erroY,Mat erroV,Mat erroU, int m, char* namefile);

        void golombDesc(Mat &DescY, Mat &DescV, Mat &DescU,int m,char* namefile);

        private:
            //int m;
            string filename;
};
//#endif
