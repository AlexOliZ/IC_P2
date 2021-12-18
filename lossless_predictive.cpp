#include "lossless_predictive.h"
#include "./bit_stream/bit_stream.h"
#include "./Golomb/golomb.h"
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
//#include "matplotlibcpp.h"
#include <cmath>
#include <math.h>

//g++ lossless_predictive.cpp Golomb/golomb.cpp predictor.cpp bit_stream/bit_stream.cpp -lsndfile

using namespace std;
static const int test[] = {6,15,70,60,150,200,-100,-200,-70,0,0,0,5,6,60,88,60,30,10,0,0,-10};

void lossless_predictive::predictive_encode(char* outfile){

    char* code;
    int *buf,i,j,avg;
    int num_items,num;
    predictor predictor_encoder;

    SF_INFO inFileInfo;
    SNDFILE* inFile;

    inFileInfo.format = 0;
    inFile = sf_open(filename, SFM_READ, &inFileInfo);
    int channels = inFileInfo.channels;

    num_items = inFileInfo.frames*channels;

    /* Allocate space for the data to be read, then read it. */
    buf = (int *) malloc(num_items*sizeof(int));
    num = sf_read_int(inFile,buf,num_items);

    cout << num << endl;
    cout << channels << endl;
    sf_close(inFile);

    bit_stream stream(outfile,false,true);   

    // encode predictor
    // para ser lossy dar shift << N vezes para cortar N bits+
    // audio é stereo ou mono~
    
    int average = 0;
    for(i=0 ; i<20 ; i++)
        average += test[i];

    m=(int)ceil(-1/log2(average/(average+1.0)));
    golomb golomb_encoder(m);

    for(i=0 ; i<22 ; i++)
    {
        code = golomb_encoder.signed_encode(test[i]);
        stream.writeBits(code,1+golomb_encoder.get_remSize()+golomb_encoder.get_unarySize());
        for(int j=0 ; j*8< 1+golomb_encoder.get_remSize()+golomb_encoder.get_unarySize() ; j++){
            for(int i=0 ; i<8 ; i++)
            {
                if(i+8*j < 1+golomb_encoder.get_remSize()+golomb_encoder.get_unarySize())
                    cout << ((code[j] >> i) & 0x01) ;
            }
        }
        cout << endl;
        cout << "unary: " << golomb_encoder.get_unarySize() << endl;
        cout << "res: " << golomb_encoder.get_remSize() << endl;
        //free(code);
    }
    stream.close_file_write();
    //stream.close_file_read();
    printf("Read %d items\n",num);
}

void lossless_predictive::predictive_decode(char* infile)
{
    bit_stream stream(infile,true,false);
    predictor predictor_decoder;
    golomb golomb_decoder(m);
    uint32_t unary_size = 0;
    uint32_t r,k,b;
    uint32_t res_size = 0;
    uint32_t n;
    bool found_unary=false;
    char bit;
    char* encoded_code;
    int code;
    b=(int)(log2(m));
    k=pow(2,(b + 1))-m;
    
    // unary_size = m/n
    // n = unary_size*m
    // k = n-unary_size*m
    // r = b  se r < k|| r = b+1 se r >= k
    while(!stream.end_of_file()){
        bit = stream.readBit();
        unary_size+=1;
        cout << (bit&0x01);
        if(bit==1){
            
            n = unary_size*m;
            r = n%m;
            res_size = b;
            if(r>=k)
                res_size++;
            encoded_code = stream.readBits(res_size);
            code = golomb_decoder.signed_decode(encoded_code,res_size,unary_size-1);
            for(int j=0 ; j*8< unary_size+res_size ; j++){
                for(int i=0 ; i<8 ; i++)
                {
                    cout << ((encoded_code[j] >> i) & 0x01) ;
                }
            }
            cout << endl;
            //code = predictor_decoder.reconstruct(golomb_decoder.signed_decode(encoded_code,unary_size-1,res_size));
            cout << "code: " << code << endl;
            cout << "unary: " << unary_size << endl;
            cout << "res: " << res_size << endl;
            unary_size = 0;
        }
    }
    //cout << "code: " << code << endl;
    stream.close_file_read();
}

int main(int argc, char* argv[])
{
    
    string file = "./wavfiles/sample06.wav";
    string binfile = "testfile.bin";
    //cout << floor(0.9) << endl;
    //cout << (int)0.9 << endl;
    /*
    char *filename = new char[file.length()];
    strcpy(filename, file.c_str());

    int *buf;
    int num_items,num;

    SF_INFO inFileInfo;
    SNDFILE* inFile;

    inFileInfo.format = 0;
    inFile = sf_open((char*)file.data(), SFM_READ, &inFileInfo);
    int channels = inFileInfo.channels;

    num_items = inFileInfo.frames*channels;
    */
    /* Allocate space for the data to be read, then read it. */
    /*
    buf = (int *) malloc(num_items*sizeof(int));
    num = sf_read_int(inFile,buf,num_items);

    cout << num << endl;
    cout << channels << endl;
    sf_close(inFile);
    printf("Read %d items\n",num);
    */
    lossless_predictive lossless((char*)file.data());
    lossless.predictive_encode((char*)binfile.data());
    lossless.predictive_decode((char*)binfile.data());

    return 0;
}