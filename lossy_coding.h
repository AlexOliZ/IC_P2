#include <string>
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

class  lossy_coding{
    public:
         lossy_coding(string fname){
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
        void RGB (Mat y, Mat u, Mat v, Mat &ImagemRBG);
        /**
         * @brief 
         * 
         * @param valorPixel pixel value
         * @param valorPrevisto expected pixel value
         * @return int -> residual for each value of the predicted pixel
         */
        int erroEnc (int valorPixel, int valorPrevisto, int quantization);
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
         * @param erroY residual of the y parameter to be encoded 
         * @param erroV residual of the v parameter to be encoded 
         * @param erroU residual of the u parameter to be encoded 
         * @param m golomg parameter m
         * @param namefile name of the file to be write from golomb
         */
        void golombEnc(Mat erroY,Mat erroV,Mat erroU, int m, char* namefile);
        /**
         * @brief 
         * 
         * @param DescY decoded y matrix
         * @param DescV decoded v matrix
         * @param DescU decoded u matrix
         * @param m golomg parameter m
         * @param namefile name of the file to be read from golomb
         */
        void golombDesc(Mat &DescY, Mat &DescV, Mat &DescU,int m,char* namefile);
        private:
            string filename;
};
//#endif
