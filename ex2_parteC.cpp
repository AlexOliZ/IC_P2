//g++ ex2_parteC.cpp ./bit_stream/bit_stream.cpp ./Golomb/golomb.cpp -o ex2 `pkg-config --cflags --libs opencv`
// ./ex2 <input image> <output filename> <quantization> (ex ./parteC lena.ppm copy.ppm 2)

#include <string>
#include "./bit_stream/bit_stream.h"
#include "./Golomb/golomb.h"
#include "lossy_coding.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

//convert the RGB image to YUV 
void lossy_coding::YUV (Mat image, Mat &y, Mat &u, Mat &v){

    for (int i=0;i<image.size().height;i++){ //row
        for(int j=0;j<image.size().width;j++){ //columns
            double b = image.at<Vec3b>(i,j)[2];
            double g = image.at<Vec3b>(i,j)[1];
            double r = image.at<Vec3b>(i,j)[0];

            r=r/255;
            g=g/255;
            b=b/255;
            int auxy = 16+65.481*r+128.553*g+24.966*b;
            int auxu, auxv;
            //reduce the matrices u and v by half
            if(i%2==0 && j%2==0){ //if columns and rows are even
                auxu = 128 - 37.797*r - 74.203*g + 112.0*b;
                auxv = 128+112.0*r-93.786*g-18.214*b;

                if(auxu<16) auxu=16;
                if(auxu>240) auxu=240;
                if(auxv<16) auxv=16;
                if(auxv>240) auxv=240;
                u.at<uchar>(i/2,j/2) = auxu;
                v.at<uchar>(i/2,j/2) = auxv;
            }

            if(auxy<16) auxy=16;
            if(auxy>235) auxy=235;
            y.at<uchar>(i,j) = auxy;
            
        }
    }
}


//convert the YUV image to RGB
void lossy_coding::RGB (Mat y, Mat u, Mat v, Mat &ImagemRBG){
 
    for (int i=0;i<ImagemRBG.size().height;++i){ //row
        for(int j=0;j<ImagemRBG.size().width;++j){ //columns    
            int y_= y.at<uchar>(i,j);
            int u_= u.at<uchar>(i/2,j/2);
            int v_= v.at<uchar>(i/2,j/2);
            double R = (1.164*(y_ - 16) + 1.596*(v_-128));
            double B = (1.164*(y_ - 16) + 2.018*(u_-128));
            double G = (1.164*(y_ - 16) - 0.813*(u_-128) - 0.391*(v_-128));
            if(R<0) R=0;
            if(B<0) B=0;
            if(G<0) G=0;
            if(R>255) R=255;
            if(B>255) B=255;
            if(G>255) G=255;
            ImagemRBG.at<Vec3b>(i,j)[0]= R;
            ImagemRBG.at<Vec3b>(i,j)[2]= B;
            ImagemRBG.at<Vec3b>(i,j)[1]= G;
        }
    }   
}



//returns the error of the prediction (residual)
int lossy_coding::erroEnc (int valorPixel, int valorPrevisto,int quantization){
    int residual = valorPixel-valorPrevisto;
    return residual + ((residual>>quantization)<<quantization);
}

//returns the final pixel value -> after encoding and decoding
int lossy_coding::ValorPixelDec (int erro, int valorPrevisto,int quantization){
    int residual = erro + valorPrevisto;
    return residual <<quantization;
}

//function to predict the next pixel using the previous ones 
int lossy_coding::preditor (int a, int b, int c){ 
    int x;
    if(c>= max(a,b)){
        x = min(a,b);
    }else if (c<=min(a,b)){
        x = max(a,b);
    }else{
        x = a+b-c;
    }
    return x;
}


//assign a,b,c to call the function to predict the next pixel
void lossy_coding::preditor_JPEG_LS (Mat matriz,Mat &erMat, Mat &prev, int quantization){
    for (int i=0;i<(matriz.size().height);++i){ //row
        for(int j=0;j<(matriz.size().width);++j){ //columns
            int a,b,c;
            int PixelAtual = matriz.at<uchar>(i,j);
            if (j==0 && i!=0){  // if we are in the 1st column
                a = 0;
                b = matriz.at<uchar>(i-1,j);
                c = 0;
            }else if (i==0 && j!=0){    // if we are in the 1st row
                a = matriz.at<uchar>(i,j-1);
                b = 0;
                c = 0;
            }else if (i==0 && j==0){ //if we have in the first column and in the first row
                a = 0;
                b = 0;
                c = 0;
            }else{
                a = matriz.at<uchar>(i,j-1);
                b = matriz.at<uchar>(i-1,j);
                c = matriz.at<uchar>(i-1,j-1);
            }
    
            int previsao = preditor(a,b,c);
            //printf("previsao -> %d\n", previsao);

            int erro = erroEnc (PixelAtual, previsao,quantization);
            erMat.at<uchar>(i,j)  = erro;
            prev.at<uchar>(i,j) = previsao;
            //printf("erro[%d][%d] -> %d\n",i,j,erMat.at<uchar>(i,j));
        
        }
    }  
    
}

//encode the residual into a file
void lossy_coding::golombEnc(Mat erroY,Mat erroV,Mat erroU, int m, char* namefile){
    
    golomb golomb_encoder(m,namefile);
    
    for (int h =0;h<erroY.size().height ; h++){
        for(int c = 0;c<erroY.size().width;c++){
            int erro = erroY.at<uchar>(h,c);
            golomb_encoder.signed_stream_encode(erro);
        }
    }

    for (int h =0;h<erroV.size().height ; h++){
        for(int c = 0;c<erroV.size().width;c++){
            int erro = erroV.at<uchar>(h,c);
            golomb_encoder.signed_stream_encode(erro);
        }
    }

    for (int h =0;h<erroU.size().height ; h++){
        for(int c = 0;c<erroU.size().width;c++){
            int erro = erroU.at<uchar>(h,c);
            golomb_encoder.signed_stream_encode(erro);
        }
    }
    
    golomb_encoder.close_stream_write();
    
}

//read the encoded file
void lossy_coding::golombDesc(Mat &DescY, Mat &DescV, Mat &DescU,int m,char* namefile){
    golomb golomb_Decoder(m,namefile); 

     for (int h =0;h<DescY.size().height ; h++){
        for(int c = 0;c<DescY.size().width;c++){
            int desc = golomb_Decoder.signed_stream_decode();
            DescY.at<uchar>(h,c) = desc;
        }
    }

    for (int h =0;h<DescV.size().height ; h++){
        for(int c = 0;c<DescV.size().width;c++){
            int desc = golomb_Decoder.signed_stream_decode();
            DescV.at<uchar>(h,c) = desc;
        }
    }

    for (int h =0;h<DescU.size().height ; h++){
        for(int c = 0;c<DescU.size().width;c++){
            int desc = golomb_Decoder.signed_stream_decode();
            DescU.at<uchar>(h,c) = desc;
        }
    }
    
    golomb_Decoder.close_stream_read();
}


int main(int argc,char *argv[]) {
    
    if(argc < 4)
    {
        cout << "Error: Should write <input image> <output filename> <quantization>" << endl; 
        return EXIT_FAILURE;
    } 

    string input_name = argv[1];
    char* outfile = argv[2];
    int quantization = atoi(argv[3]);
    FILE* input_f,*output_f;
    const char* path = "./imagensPPM/";

    input_name = path + input_name;
    path = input_name.c_str();
    
    Mat input_image = imread(samples::findFile(path)); //read image
    if(input_image.empty())
    {
        printf("read input file error");
        return EXIT_FAILURE;
    } 
    Mat output_image = Mat::zeros(input_image.size(),input_image.type());
    lossy_coding lossy(outfile);
    
    //matrices with the values of each pixel in the y,v,u components
    Mat y (input_image.size().height,input_image.size().width,CV_8UC1);
    Mat v (input_image.size().height/2,input_image.size().width/2,CV_8UC1);
    Mat u (input_image.size().height/2,input_image.size().width/2,CV_8UC1);
    lossy.YUV(input_image,y,u,v);
    
    //matrix with the errors of the components y,v and u
    Mat erroY (y.size().height,y.size().width,CV_8UC1); 
    Mat prevAuxY (y.size().height,y.size().width,CV_8UC1); 
    lossy.preditor_JPEG_LS(y,erroY,prevAuxY,quantization);
    Mat erroV (v.size().height,v.size().width,CV_8UC1); 
    Mat prevAuxV (v.size().height,v.size().width,CV_8UC1);
    lossy.preditor_JPEG_LS(v,erroV,prevAuxV,quantization);
    Mat erroU (u.size().height,u.size().width,CV_8UC1); 
    Mat prevAuxU (u.size().height,u.size().width,CV_8UC1);
    lossy.preditor_JPEG_LS(u,erroU,prevAuxU,quantization);
    
    //print an error matrix of the components y,v and u
    /*for (int i = 0;i<erroY.size().height-1;i++){
        for (int j = 0;j<erroY.size().width-1;j++){
            printf("erro[%d][%d] -> %d\n",i,j,erroY.at<uchar>(i,j));
        }
    }
    
    for (int i = 0;i<erroV.size().height;i++){
        for (int j = 0;j<erroV.size().width;j++){
            //printf("erro[%d][%d] -> %d\n",i,j,erroV.at<uchar>(i,j));
            printf("erro[%d][%d] -> %d\n",i,j,erroU.at<uchar>(i,j));
        }
    }*/
  
    double mean = ((erroV.size().height*erroV.size().width)*10);
    double alpha = mean/(mean+1.0);
    int m = (int) ceil(-1/log2(alpha));
    //encode the residual
    lossy.golombEnc(erroY,erroV,erroU,m,outfile); 

    //matrices DescY, DescV, DescU have the decoded residual of each component
    Mat DescY (erroY.size().height,erroY.size().width,CV_8UC1); 
    Mat DescV (erroV.size().height,erroV.size().width,CV_8UC1); 
    Mat DescU (erroU.size().height,erroU.size().width,CV_8UC1); 
    lossy.golombDesc(DescY,DescV,DescU,m,outfile);
   
    //predicted pixel value for each component
    Mat erroAuxY (y.size().height,y.size().width,CV_8UC1); 
    Mat prevY (y.size().height,y.size().width,CV_8UC1); 
    lossy.preditor_JPEG_LS(y,erroAuxY,prevY,quantization);
    Mat erroAuxV (v.size().height,v.size().width,CV_8UC1); 
    Mat prevV (v.size().height,v.size().width,CV_8UC1);
    lossy.preditor_JPEG_LS(v,erroAuxV,prevV,quantization);
    Mat erroAuxU (u.size().height,u.size().width,CV_8UC1); 
    Mat prevU (u.size().height,u.size().width,CV_8UC1);
    lossy.preditor_JPEG_LS(u,erroAuxU,prevU,quantization);
    
    //matrices with the pixel values after the compression process
    Mat ValorY (erroY.size().height,erroY.size().width,CV_8UC1); 
    Mat ValorV (erroV.size().height,erroV.size().width,CV_8UC1); 
    Mat ValorU (erroU.size().height,erroU.size().width,CV_8UC1); 

    //calculation of the pixel value after decoding for the Y, V and U component
    for (int i=0;i<(ValorY.size().height)-1;++i){ //row
        for(int j=0;j<(ValorY.size().width);++j){ //columns
            int code= DescY.at<uchar>(i,j); //comes from the golomb decoder
            ValorY.at<uchar>(i,j) = lossy.ValorPixelDec(code, prevY.at<uchar>(i,j),quantization);
        }
    }
    
    for (int i=0;i<(ValorV.size().height)-1;++i){ //row
        for(int j=0;j<(ValorV.size().width);++j){ //columns
            int code= DescV.at<uchar>(i,j); //comes from the golomb decoder
            ValorV.at<uchar>(i,j)  = lossy.ValorPixelDec(code, prevV.at<uchar>(i,j),quantization);
        }
    }

    for (int i=0;i<(ValorU.size().height)-1;++i){ //row
        for(int j=0;j<(ValorU.size().width);++j){ //columns
            int code= DescU.at<uchar>(i,j); //comes from the golomb decoder
            ValorU.at<uchar>(i,j) = lossy.ValorPixelDec(code, prevU.at<uchar>(i,j),quantization);
        }
    }
   
    lossy.RGB(ValorY,ValorU,ValorV, output_image);

    //show images
    imshow("Input image",input_image); 
    imshow("Final image",output_image);

    waitKey();

}