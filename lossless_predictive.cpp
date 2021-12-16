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

void lossless_predictive::predictive_encode(predictor predictor_encoder,char* outfile){

    char* code;
    int *buf,i,j,avg;
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

    // encode predictor
    // para ser lossy dar shift << N vezes para cortar N bits+
    // audio é stereo ou mono~
    
    int average = 0;
    for(i=0 ; i<num ; i++)
        average += buf[i];

    m=(int)ceil(-1/log2(average/(average+1.0)));
    golomb golomb_encoder(m);

    for(i=0 ; i<num ; i++)
    {
        code = golomb_encoder.signed_encode(predictor_encoder.residual(buf[i]));
        stream.writeChars(code,golomb_encoder.get_remSize()+golomb_encoder.get_unarySize());
        free(code);
    }
    sf_close(inFile);
    stream.close_file_write();
    printf("Read %d items\n",num);
}

void lossless_predictive::predictive_decode(predictor predictor_encoder,char* infile)
{
    bit_stream stream(infile);
    uint32_t unary_size = 0;
    uint32_t res_size = 0;

    


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
    predictor p((char*)file.data());
    lossless.predictive_encode(p,(char*)binfile.data());

    return 0;
}