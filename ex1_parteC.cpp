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
void lossless_codec::RGB (Mat &y, Mat &u, Mat &v, Mat &ImagemRBG){
    
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
//retorna o erro da previsao
int lossless_codec::erroEnc (int valorPixel, int valorPrevisto){
    return valorPixel-valorPrevisto;
}
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
            int PixelAtual = matriz.at<int>(i,j);
            if (j==0 && i!=0){  // se estivermos na 1 coluna
                a = 0;
                b = matriz.at<int>(i-1,j);
                c = 0;
            }else if (i==0 && j!=0){  // se estivermos na 1 linha
                a = matriz.at<int>(i,j-1);
                b = 0;
                c = 0;
            }else if (i==0 && j==0){ //se tivermos na 1 coluna e na 1 linha
                a = 0;
                b = 0;
                c = 0;
            }else{
                a = matriz.at<int>(i,j-1);
                b = matriz.at<int>(i-1,j);
                c = matriz.at<int>(i-1,j-1);
            }
    
            int previsao = preditor(a,b,c);

            //printf("previsao -> %d\n", previsao);
            
            int erro = erroEnc (PixelAtual, previsao);
            erMat.at<int>(i,j)  = erro;
            prev.at<int>(i,j) = previsao;
            //printf("erro[%d][%d] -> %d\n",i,j,erMat.at<int>(i,j));
        
        }
    }  
    
}
void lossless_codec::golombEnc(int erro,int m, char* namefile){
    
    golomb golomb_encoder(m,namefile);
    //char* code;
    //code = golomb_encoder.encode(erro);
    //escrever no ficheiro -> mandas tudo para o ficheiro depois a ler e tens que diferenciar ... 
    //les 10 da matriz y (o size da matriz y)  e depois les da matriz v (size da matriz v)
    golomb_encoder.signed_stream_encode(erro);
   // golomb_encoder.close_stream();
    //printf("erro -> %d\n", erro);
    //escrever o codigo num ficheiro
    
    /*for(int j=0 ; j< floor((golomb_encoder.get_unarySize()+golomb_encoder.get_remSize())/10000+1) ; j++)
    {
        for(int i=9999 ; i>=0 ; i--){
        if(i+10000*j < golomb_encoder.get_unarySize()+golomb_encoder.get_remSize()) {
                cout<< ((code[j]>>(i)) &0x01);
            }
        }
    }   
    cout<< endl;*/

    //stream.writeChars(code,golomb_encoder.get_remSize()+golomb_encoder.get_unarySize());
    
    //imprimir num ficheiro o bit -> usar a bit_stream
    //stream.writeChars(bit,);  
}

void lossless_codec::golombDesc(Mat prevY, Mat prevV, Mat prevU, Mat erroY, Mat erroV, Mat erroU, Mat &ValorY, Mat &ValorV, Mat &ValorU){
    //ler de um ficheiro 
    for (int i=0;i<(erroY.size().height)-1;++i){ //row
        for(int j=0;j<(erroY.size().width);++j){ //columns
            //ler para o code
            int code=0; //vem do descodificador de golomb
            ValorY.at<int>(i,j) = ValorPixelDec(code, prevY.at<int>(i,j));
        }
    }

    for (int i=0;i<(erroV.size().height)-1;++i){ //row
        for(int j=0;j<(erroV.size().width);++j){ //columns
            //ler para o code
            int code=0; //vem do descodificador de golomb
            ValorV.at<int>(i,j)  = ValorPixelDec(code, prevY.at<int>(i,j));
        }
    }

    for (int i=0;i<(erroU.size().height)-1;++i){ //row
        for(int j=0;j<(erroU.size().width);++j){ //columns
            //ler para o code
            int code=0; //vem do descodificador de golomb
            ValorU.at<int>(i,j) = ValorPixelDec(code, prevY.at<int>(i,j));
        }
    }


    
}

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
    printf("1 pixel -> antes -> %d\n", input_image.at<Vec3b>(0,0)[2]);
    printf("2 pixel -> antes -> %d\n", input_image.at<Vec3b>(0,1)[2]);
    printf("3 pixel -> antes -> %d\n", input_image.at<Vec3b>(0,2)[2]);
    lossless.YUV(input_image,y,u,v);
    imshow("y",y);
    imshow("u",u);
    imshow("v",v);
    lossless.RGB(y,u,v, output_image);
    printf("1 pixel -> depiois -> %d\n", output_image.at<Vec3b>(0,0)[2]);
    printf("2 pixel -> depiois -> %d\n", output_image.at<Vec3b>(0,1)[2]);
    printf("3 pixel -> depiois -> %d\n", output_image.at<Vec3b>(0,2)[2]);
   
    


/*
    //matriz com os erros todos de y
    //precisas dos +3 para as pontas das imagens (a,b,c nas pontas acrescentas 0)
    Mat erroY (y.size().height+3,y.size().width+3,CV_8UC1); 
    Mat prevY (y.size().height+3,y.size().width+3,CV_8UC1); 
    lossless.preditor_JPEG_LS(y,erroY,prevY);
    Mat erroV (v.size().height+3,v.size().width+3,CV_8UC1); 
    Mat prevV (y.size().height+3,y.size().width+3,CV_8UC1);
    lossless.preditor_JPEG_LS(v,erroV,prevV);
    Mat erroU (u.size().height+3,u.size().width+3,CV_8UC1); 
    Mat prevU (y.size().height+3,y.size().width+3,CV_8UC1);
    lossless.preditor_JPEG_LS(u,erroU,prevU);
    */


    //imprimir uma matriz 
    /*for (int i = 0;i<erroY.size().height-1;i++){
        for (int j = 0;j<erroY.size().width-1;j++){
            printf("erro[%d][%d] -> %d\n",i,j,erroY.at<int>(i,j));
        }
    }
    
    for (int i = 0;i<erroV.size().height;i++){
        for (int j = 0;j<erroV.size().width;j++){
            //printf("erro[%d][%d] -> %d\n",i,j,erroV.at<int>(i,j));
            printf("erro[%d][%d] -> %d\n",i,j,erroU.at<int>(i,j));
        }
    }*/
  
    //char* code;
    //double mean = (codeY.size().height*codeY.size().width + codeV.size().height*codeV.size().width + codeU.size().height*codeU.size().width)/3;
  /*
    double mean = ((erroV.size().height*erroV.size().width)*10);
    double alpha = mean/(mean+1.0);
    int m = (int) ceil(-1/log2(alpha));
    
    for (int h =0;h<erroY.size().height ; h++){
        for(int c = 0;c<erroY.size().width;c++){
            lossless.golombEnc(erroY.at<int>(h,c),m,outfile);
        }
    }

    for (int h =0;h<erroV.size().height ; h++){
        for(int c = 0;c<erroV.size().width;c++){
            lossless.golombEnc(erroV.at<int>(h,c),m,outfile);
        }
    }

    for (int h =0;h<erroU.size().height ; h++){
        for(int c = 0;c<erroU.size().width;c++){
            lossless.golombEnc(erroU.at<int>(h,c),m,outfile);
        }
    }

    //matrizes com o valor do pixel em YUV 
    Mat ValorY (erroY.size().height+3,erroY.size().width+3,CV_8UC1); 
    Mat ValorV (erroV.size().height+3,erroV.size().width+3,CV_8UC1); 
    Mat ValorU (erroU.size().height+3,erroU.size().width+3,CV_8UC1); 
    lossless.golombDesc(prevY,prevV,prevU,erroY,erroV,erroU,ValorY,ValorV,ValorU);

    
    //imprimir uma matriz -> é suposto da igual acho eu ---- por o codigo do descodificador do golomb na funcao e ver
    for (int i = 0;i<ValorY.size().height-1;i++){
        for (int j = 0;j<ValorY.size().width-1;j++){
           //printf("ValorY[%d][%d] -> %d\n",i,j,ValorY.at<int>(i,j));
        }
    }


    //é preciso passarmos YUV par RGB e imprimir a imagem como antes???
*/
    
    //teste do erro -> funciona
    //int erpijorg = lossless.erroEnc (986452865,7);
    //int ver = lossless.ValorPixelDec(erpijorg,7);
    //printf("ola-< %d",ver);

    imshow("Input image",input_image); //show image
    
    //imwrite(output_name,output_image); //write image

    imshow("Copied image",output_image);
    waitKey();

}
