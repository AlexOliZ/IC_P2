// g++ parteC.cpp ./bit_stream/bit_stream.cpp ./Golomb/golomb.cpp -o parteC `pkg-config --cflags --libs opencv`
// ./parteC <m> <input image> <output filename> (por ex ./ex4 5 lena.ppm copy.ppm)
#include <string>
#include "./bit_stream/bit_stream.h"
#include "./Golomb/golomb.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

//converter a imagem RGB para YUV 
void YUV (Mat image, Mat &y, Mat &u, Mat &v){
//passar as matrizes como & e depois alterar aqui com & 

    //Mat Auxy (image.size().height,image.size().width,CV_8UC1);
    Mat Auxu (image.size().height,image.size().width,CV_8UC1);
    Mat Auxv (image.size().height,image.size().width,CV_8UC1);
    cv::Vec3b val;
    int cn = image.channels(); //da nos os canais das cores... da para sabermos que cores e que sao
    uint8_t* pixelPtr = (uint8_t*)image.data; //ler a imagem pixel a pixel

    for (int i=0;i<image.size().height;++i){ //row
        for(int j=0;j<image.size().width;++j){ //columns
            Vec3b & val = image.at<Vec3b>(i,j);
            //color
            val[0] = pixelPtr[i*image.cols*cn + j*cn + 0];   //B
            val[1] = pixelPtr[i*image.cols*cn + j*cn + 1];   //G
            val[2] = pixelPtr[i*image.cols*cn + j*cn + 2];   //R
            
            //converter para yuv
            y.at<Vec3b>(i,j) = 0.299*val[2] + 0.587*val[1] + 0.114*val[0];
            Auxu.at<Vec3b>(i,j) = -0.147*val[2] - 0.289*val[1] + 0.436*val[0];
            Auxv.at<Vec3b>(i,j) = 0.615*val[2] - 0.515*val[1] - 0.100*val[0];
        }
    }

    //retirar as linhas impares do u e do v
    //reduzir as matrizes u e v a metade das colunas e das linhas
    //de 100:100 tem que passar a 50:50 por ex
    for (int i = 0 ; i<u.size().height; ++i){
        for (int j=0;j<u.size().width;++j){
            if(i%2==0 && j%2==0){ //se as colunas e as linhas forem pares
                u.at<Vec3b>(i,j) = Auxu.at<Vec3b>(i,j);
                v.at<Vec3b>(i,j) = Auxv.at<Vec3b>(i,j);
            }
        }
    }
}

//retorna o erro da previsao
int erroEnc (int valor, int valorPrevisto){
    return valor-valorPrevisto;
}

// funcao de previsao do proximo pixel usando os anteriores 
int preditor (int a, int b, int c){ 
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
void preditor_JPEG_LS (Mat matriz,Mat &erMat){
    
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
            
            int erro = erroEnc (PixelAtual, previsao);
            //erro tem +3 espacos que as outras matrizes por causa das pontas
            erMat.at<int>(i,j)  = erro;
            //printf("erro[%d][%d] -> %d\n",i,j,erMat.at<int>(i,j));
        
        }
    }  
    
}

int main(int argc,char *argv[]) {
    
    if(argc < 4)
    {
        //o divisor do golomb, a imagem de entrada, e o ficheiro de saida onde tem as coisas codificadas do golomb
        cout << "Error: Should write <m> <input image> <output filename>" << endl; 
        return EXIT_FAILURE;
    } 

    int m = atoi(argv[1]); //divisor do golomb
    string input_name = argv[2];
    char* outfile = argv[3];
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
    //matrizes com os valores de cada pixel nas componentes y,v,u
    Mat y (input_image.size().height,input_image.size().width,CV_8UC1);
    Mat v (input_image.size().height/4,input_image.size().width/4,CV_8UC1);
    Mat u (input_image.size().height/4,input_image.size().width/4,CV_8UC1);
    YUV(input_image,y,u,v);

    //matriz com os erros todos de y
    //precisas dos +2 para as pontas das imagens (a,b,c nas pontas acrescentas 0)
    Mat erroY (y.size().height+3,y.size().width+3,CV_8UC1); 
    preditor_JPEG_LS(y,erroY);
    Mat erroV (v.size().height+3,v.size().width+3,CV_8UC1); 
    preditor_JPEG_LS(v,erroV);
    Mat erroU (u.size().height+3,u.size().width+3,CV_8UC1); 
    preditor_JPEG_LS(u,erroU);

    //printf("%d",v.size().height);
    //imprimir uma matriz 
    for (int i = 0;i<erroY.size().height-1;i++){
        for (int j = 0;j<erroY.size().width-1;j++){
            //printf("erro[%d][%d] -> %d\n",i,j,erroY.at<int>(i,j));
        }
    }
    
    for (int i = 0;i<erroV.size().height;i++){
        for (int j = 0;j<erroV.size().width;j++){
            //printf("erro[%d][%d] -> %d\n",i,j,erroV.at<int>(i,j));
            //printf("erro[%d][%d] -> %d\n",i,j,erroU.at<int>(i,j));
        }
    }
  
    //para o erro do y
    
    Mat codeY (erroY.size().height,erroY.size().width,CV_8UC1);
    Mat codeV (erroV.size().height,erroV.size().width,CV_8UC1);
    Mat codeU (erroU.size().height,erroU.size().width,CV_8UC1);
    char* code;
    golomb golomb_encoder(m);
    for (int i = 0;i<erroV.size().height-1;i++){
        for (int j = 0;j<erroV.size().width-1;j++){
            code = golomb_encoder.encode(erroV.at<int>(i,j));
            codeV.at<char>(i,j)= *code;   
        }
    }
    //print(codeY);

    for (int i = 0;i<codeV.size().height-1;i++){
        for (int j = 0;j<codeV.size().width-1;j++){
           
            printf("golom[%d][%d] -> %d\n",i,j,codeV.at<int>(i,j));

        }
    }
   
   


    /*golomb golomb_encoder(m);
    char* code;
    code = golomb_encoder.encode(erro);
    for(j=0 ; j< floor((golomb_encoder.get_unarySize()+golomb_encoder.get_remSize())/8+1) ; j++)
    {
        for(i=7 ; i>=0 ; i--){
        if(i+8*j < golomb_encoder.get_unarySize()+golomb_encoder.get_remSize()) {
                cout<< ((code[j]>>(i)) &0x01);
            }
        }
    }   
    cout<< endl;*/


     
    //printf("erro -> %d\n",erroy.at<int>(i,j));
    //uint erro = abs(Auxerro); //entropia
    // code = golomb_encoder.encode(erro);
    //stream.writeChar(code);
    //printf("erro = %d , codigo = %s\n" ,erro,code); 
    /*
    //char bit = g.encode(erro);
    char code = golomb_encoder.signed_encode(erro);
    stream.writeChars(code,golomb_encoder.get_remSize()+golomb_encoder.get_unarySize());
    
    //imprimir num ficheiro o bit -> usar a bit_stream
    stream.writeChars(bit,);  */
    //falta chamar o golomb para cada disto, e o bitStream para imprimir num ficheiro as coisas codificadas pelo golomb
                

   
    //create the matrix of the output image 
    //Mat output_image = Mat::zeros(input_image.size(),input_image.type());

    // pixel by pixel
   
    //Copy pixel by pixel
    /* for (int i=0;i<y.size().height;++i) //row
    {
        for(int j=0;j<y.size().width;++j) //columns
        {
            int aux = (y.at<int>(i,j));
            printf ("valores de y -> %d\n",aux);
            
        }
        
    }*/



    /*   
    int previsao = preditor(a,b,c);
    int Auxerro = erro (PixelAtual, previsao);
    uint erro = abs(Auxerro); //entropia
    //char bit = g.encode(erro);
    char code = golomb_encoder.signed_encode(erro);
    stream.writeChars(code,golomb_encoder.get_remSize()+golomb_encoder.get_unarySize());
    
    //imprimir num ficheiro o bit -> usar a bit_stream
    stream.writeChars(bit,);
    */

    //imshow("Input image",input_image); //show image
     
    //imwrite(output_name,output_image); //write image

    //imshow("Copied image",output_image);
    waitKey();

}