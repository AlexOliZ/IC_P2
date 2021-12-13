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

void lossless_predictive::predictive_encode(predictor p,char* outfile){

    char* code;
    int *buf;
    int num_items,num;

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
    printf("Read %d items\n",num);

    bit_stream stream(outfile);    
    golomb golomb_encoder(m);

    // encode predictor
    // para ser lossy dar shift << N vezes para cortar N bits+
    for(int i=0 ; i<num ; i+= channels)
    {
        for (int j=0; j < channels; ++j){
            //cout << "val: " << buf[i+j] << endl;
            code = golomb_encoder.signed_encode(buf[i+j]);
            stream.writeChars(code,golomb_encoder.get_remSize()+golomb_encoder.get_unarySize());
            /*
            for(j=0 ; j< floor((golomb_encoder.get_unarySize()+golomb_encoder.get_remSize())/8+1) ; j++)
            {
                for(i=7 ; i>=0 ; i--){
                if(i+8*j < golomb_encoder.get_unarySize()+golomb_encoder.get_remSize()) {
                        cout<< ((code[j]>>(i)) &0x01);
                    }
                }
            }   
            cout<< endl;
            */
            free(code);
        }
    }
    sf_close(inFile);
    stream.close_file_write();
    printf("Read %d items\n",num);
}

void lossless_predictive::predictive_decode(predictor p,char* infile)
{



}

int main(int argc, char* argv[])
{
    
    if(argc < 2)
        cout << "missing args, ./a.out <m>" << endl;
    int m = atoi(argv[1]);
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
    lossless_predictive lossless((char*)file.data(),m);
    predictor p((char*)file.data());
    lossless.predictive_encode(p,(char*)binfile.data());

    return 0;
}