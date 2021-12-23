// g++ parteC.cpp ./bit_stream/bit_stream.cpp ./Golomb/golomb.cpp -o parteC `pkg-config --cflags --libs opencv`
// ./parteC <input image> <output filename> (por ex ./parteC lena.ppm copy.ppm)
#include <string>
#include "./bit_stream/bit_stream.h"
#include "./Golomb/golomb.h"
#include "lossless_codec.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

//converter a imagem RGB para YUV 
void lossless_codec::YUV (Mat image, Mat &y, Mat &u, Mat &v){
//passar as matrizes como & e depois alterar aqui com & 

    for (int i=0;i<image.size().height;i++){ //row
        for(int j=0;j<image.size().width;j++){ //columns
            //Vec3b & val = image.at<Vec3b>(i,j);
            //color
            double b = image.at<Vec3b>(i,j)[2];//pixelPtr[i*image.cols*cn + j*cn + 0];   //B
            double g = image.at<Vec3b>(i,j)[1];//pixelPtr[i*image.cols*cn + j*cn + 1];   //G
            double r = image.at<Vec3b>(i,j)[0];//pixelPtr[i*image.cols*cn + j*cn + 2];   //R

            //converter para yuv
            //int auxy = 0.299*r + 0.587*g + 0.114*b;
            r=r/255;
            g=g/255;
            b=b/255;
            int auxy = 16+65.481*r+128.553*g+24.966*b;
            int auxu, auxv;
            if(i%2==0 && j%2==0){ //se as colunas e as linhas forem pares
                //auxu = -0.147*r - 0.289*g + 0.436*b;
                //auxv = 0.615*r - 0.515*g - 0.100*b;
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


//converter a imagem YUV  para RGB
void lossless_codec::RGB (Mat y, Mat u, Mat v, Mat &ImagemRBG){
    
    int cn = ImagemRBG.channels(); //da nos os canais das cores... da para sabermos que cores e que sao
    uint8_t* pixelPtr = (uint8_t*)ImagemRBG.data; //ler a imagem pixel a pixel

    for (int i=0;i<ImagemRBG.size().height;++i){ //row
        for(int j=0;j<ImagemRBG.size().width;++j){ //columns    
            //color
            int y_= y.at<uchar>(i,j);
            int u_= u.at<uchar>(i/2,j/2);
            int v_= v.at<uchar>(i/2,j/2);
            //printf("char -> %d\n", y_);
            //double R =  v_/0.877+y_;//R -> (int) (1.164*(y_ - 16) + 1.596*(v_-128));
            //double B = u_/0.492 + y_;//B -> (int) (1.164*(y_ - 16) + 2.018*(u_-128));
            //double G =  (y_-0.299*R-0.114*B)/0.587 ;//G -> (int)(1.164*(y_ - 16) - 0.813*(u_-128) - 0.391*(v_-128));
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


//fiz testes destes erros e funciona
//retorna o erro da previsao (residual)
int lossless_codec::erroEnc (int valorPixel, int valorPrevisto){
    return valorPixel-valorPrevisto;
}

//retorna o valor final do pixel -> depois da codificacao e descodificacao
int lossless_codec::ValorPixelDec (int erro, int valorPrevisto){
    return erro + valorPrevisto;
}

// funcao de previsao do proximo pixel usando os anteriores 
int lossless_codec::preditor (int a, int b, int c){ 
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


//atribui o a,b,c para chamar a funcao de prever o proximo pixel
void lossless_codec::preditor_JPEG_LS (Mat matriz,Mat &erMat, Mat &prev){
    for (int i=0;i<(matriz.size().height);++i){ //row
        for(int j=0;j<(matriz.size().width);++j){ //columns
            int a,b,c;
            int PixelAtual = matriz.at<uchar>(i,j);
            if (j==0 && i!=0){  // se estivermos na 1 coluna
                a = 0;
                b = matriz.at<uchar>(i-1,j);
                c = 0;
            }else if (i==0 && j!=0){  // se estivermos na 1 linha
                a = matriz.at<uchar>(i,j-1);
                b = 0;
                c = 0;
            }else if (i==0 && j==0){ //se tivermos na 1 coluna e na 1 linha
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
            
            int erro = erroEnc (PixelAtual, previsao);
            erMat.at<uchar>(i,j)  = erro;
            prev.at<uchar>(i,j) = previsao;
           // printf("erro[%d][%d] -> %d\n",i,j,erMat.at<uchar>(i,j));
        
        }
    }  
    
}

//codificar o residual para um ficheiro
void lossless_codec::golombEnc(Mat erroY,Mat erroV,Mat erroU, int m, char* namefile){
    
    golomb golomb_encoder(m,namefile);
    //char* code;
    //code = golomb_encoder.encode(erro);
    //escrever no ficheiro -> mandas tudo para o ficheiro depois a ler e tens que diferenciar ... 
    //les 10 da matriz y (o size da matriz y)  e depois les da matriz v (size da matriz v)
   // golomb_encoder.signed_stream_encode(erro);
   // golomb_encoder.close_stream_write();
    //printf("erro -> %d\n", erro);
    

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

//ler o ficheiro codificado
void lossless_codec::golombDesc(Mat &DescY, Mat &DescV, Mat &DescU,int m,char* namefile){
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
 /*
void lossless_codec::golombDesc(Mat prevY, Mat prevV, Mat prevU, Mat erroY, Mat erroV, Mat erroU, Mat &DescY, Mat &DescV, Mat &DescU){
    //ler de um ficheiro 
    


    
}*/

int main(int argc,char *argv[]) {
    
    if(argc < 3)
    {
        //o divisor do golomb, a imagem de entrada, e o ficheiro de saida onde tem as coisas codificadas do golomb
        cout << "Error: Should write <input image> <output filename> <imagem>" << endl; 
        return EXIT_FAILURE;
    } 

    //int m = atoi(argv[1]); //divisor do golomb
    string input_name = argv[1];
    char* outfile = argv[2];
    //string imagem = argv[3];
    FILE* input_f,*output_f;
    const char* path = "./imagensPPM/";

    //cout<<input_name;

    input_name = path + input_name;
    path = input_name.c_str();
    
    Mat input_image = imread(samples::findFile(path)); //read image
    if(input_image.empty())
    {
        printf("read input file error");
        return EXIT_FAILURE;
    } 
    Mat output_image = Mat::zeros(input_image.size(),input_image.type());
    lossless_codec lossless(outfile);
    //matrizes com os valores de cada pixel nas componentes y,v,u
    Mat y (input_image.size().height,input_image.size().width,CV_8UC1);
    Mat v (input_image.size().height/2,input_image.size().width/2,CV_8UC1);
    Mat u (input_image.size().height/2,input_image.size().width/2,CV_8UC1);
    //printf("1 pixel -> antes -> %d\n", input_image.at<Vec3b>(0,0)[2]);
    //printf("2 pixel -> antes -> %d\n", input_image.at<Vec3b>(0,1)[2]);
    //printf("3 pixel -> antes -> %d\n", input_image.at<Vec3b>(0,2)[2]);
    lossless.YUV(input_image,y,u,v);
    //imshow("y",y);
    //imshow("u",u);
    //imshow("v",v);
    //lossless.RGB(y,u,v, output_image);
   // printf("1 pixel -> depiois -> %d\n", output_image.at<Vec3b>(0,0)[2]);
   // printf("2 pixel -> depiois -> %d\n", output_image.at<Vec3b>(0,1)[2]);
   // printf("3 pixel -> depiois -> %d\n", output_image.at<Vec3b>(0,2)[2]);
   

    //matriz com os erros todos de y
    //precisas dos +3 para as pontas das imagens (a,b,c nas pontas acrescentas 0)
    Mat erroY (y.size().height,y.size().width,CV_8UC1); 
    Mat prevAuxY (y.size().height,y.size().width,CV_8UC1); 
    lossless.preditor_JPEG_LS(y,erroY,prevAuxY);
    Mat erroV (v.size().height,v.size().width,CV_8UC1); 
    Mat prevAuxV (v.size().height,v.size().width,CV_8UC1);
    lossless.preditor_JPEG_LS(v,erroV,prevAuxV);
    Mat erroU (u.size().height,u.size().width,CV_8UC1); 
    Mat prevAuxU (u.size().height,u.size().width,CV_8UC1);
    lossless.preditor_JPEG_LS(u,erroU,prevAuxU);
    
    //imprimir uma matriz de erro das componentes y,v e u 
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
  
    //codificador e descodificador nao dao bem :(
    double mean = ((erroV.size().height*erroV.size().width)*10);
    double alpha = mean/(mean+1.0);
    int m = (int) ceil(-1/log2(alpha));

    printf("1 erro -> antes -> %d\n", erroY.at<uchar>(0,0));
    printf("2 erro -> antes -> %d\n", erroY.at<uchar>(0,1));
    printf("3 erro -> antes -> %d\n", erroY.at<uchar>(0,2));
    lossless.golombEnc(erroY,erroV,erroU,m,outfile); //da nos o erro para a funcao ValorPixelDec

    //da nos o erro de cada pixel para cada componente
    Mat DescY (erroY.size().height,erroY.size().width,CV_8UC1); 
    Mat DescV (erroV.size().height,erroV.size().width,CV_8UC1); 
    Mat DescU (erroU.size().height,erroU.size().width,CV_8UC1); 
    lossless.golombDesc(DescY,DescV,DescU,m,outfile);
    printf("1 erro -> depiois -> %d\n", DescY.at<uchar>(0,0));
    printf("2 erro -> depiois -> %d\n", DescY.at<uchar>(0,1));
    printf("3 erro -> depiois -> %d\n", DescY.at<uchar>(0,2));
   

    //para termos o valor previsto do pixel para cada componente
    Mat erroAuxY (y.size().height,y.size().width,CV_8UC1); 
    Mat prevY (y.size().height,y.size().width,CV_8UC1); 
    lossless.preditor_JPEG_LS(y,erroAuxY,prevY);
    Mat erroAuxV (v.size().height,v.size().width,CV_8UC1); 
    Mat prevV (v.size().height,v.size().width,CV_8UC1);
    lossless.preditor_JPEG_LS(v,erroAuxV,prevV);
    Mat erroAuxU (u.size().height,u.size().width,CV_8UC1); 
    Mat prevU (u.size().height,u.size().width,CV_8UC1);
    lossless.preditor_JPEG_LS(u,erroAuxU,prevU);
    

    //onde tem o erro é DescY V e U
    //matrizes com o valor dos pixeis depois do processo de compressao
    Mat ValorY (erroY.size().height,erroY.size().width,CV_8UC1); 
    Mat ValorV (erroV.size().height,erroV.size().width,CV_8UC1); 
    Mat ValorU (erroU.size().height,erroU.size().width,CV_8UC1); 


    for (int i=0;i<(ValorY.size().height)-1;++i){ //row
        for(int j=0;j<(ValorY.size().width);++j){ //columns
            //ler para o code
            int code= DescY.at<uchar>(i,j); //vem do descodificador de golomb
            ValorY.at<uchar>(i,j) = lossless.ValorPixelDec(code, prevY.at<uchar>(i,j));
        }
    }
    //int lossless_codec::ValorPixelDec (int erro, int valorPrevisto){

    for (int i=0;i<(ValorV.size().height)-1;++i){ //row
        for(int j=0;j<(ValorV.size().width);++j){ //columns
            //ler para o code
            int code= DescV.at<uchar>(i,j);; //vem do descodificador de golomb
            ValorV.at<uchar>(i,j)  = lossless.ValorPixelDec(code, prevV.at<uchar>(i,j));
        }
    }

    for (int i=0;i<(ValorU.size().height)-1;++i){ //row
        for(int j=0;j<(ValorU.size().width);++j){ //columns
            //ler para o code
            int code= DescU.at<uchar>(i,j);; //vem do descodificador de golomb
            ValorU.at<uchar>(i,j) = lossless.ValorPixelDec(code, prevU.at<uchar>(i,j));
        }
    }
    printf("1 erro -> depiois preditor -> %d\n", ValorY.at<uchar>(0,0));
    printf("2 erro -> depiois preditor-> %d\n", ValorY.at<uchar>(0,1));
    printf("3 erro -> depiois preditor-> %d\n", ValorY.at<uchar>(0,2));
   

//int lossless_codec::ValorPixelDec (int erro, int valorPrevisto){ erro = golomb -> valor previsto = preditor



    //imprimir uma matriz -> é suposto da igual acho eu ---- por o codigo do descodificador do golomb na funcao e ver
    
   /* for (int i = 0;i<ValorY.size().height-1;i++){
        for (int j = 0;j<ValorY.size().width-1;j++){
            //printf("valor do pixel[%d][%d] -> %d\n",i,j,ValorY.at<uchar>(i,j));
        }
    }
    
    for (int i = 0;i<ValorV.size().height;i++){
        for (int j = 0;j<ValorV.size().width;j++){
            //printf("erro[%d][%d] -> %d\n",i,j,ValorV.at<uchar>(i,j));
            //printf("erro[%d][%d] -> %d\n",i,j,ValorU.at<uchar>(i,j));
        }
    }*/

    lossless.RGB(ValorY,ValorU,ValorV, output_image);




    //teste do erro -> funciona
    //int erpijorg = lossless.erroEnc (986452865,7);
    //int ver = lossless.ValorPixelDec(erpijorg,7);
    //printf("ola-< %d",ver);

    imshow("Input image",input_image); //show image
    
    //imwrite(output_name,output_image); //write image

    imshow("Copied image",output_image);
    waitKey();

}