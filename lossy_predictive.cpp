#include "lossy_predictive.h"
//#include "./bit_stream/bit_stream.h"


//g++ lossy_predictive.cpp Golomb/golomb.cpp predictor.cpp bit_stream/bit_stream.cpp -lsndfile

using namespace std;
static const int test[] = {6,15,70,60,50,0,-10,-20,-7,0,0,0,5,6,6,8,6,3,10,0,0,-10};

void lossy_predictive::lossypredictive_encode(char* outfile){

    char* code;
    int *buf,i,j,avg;
    int num_items,num;
    predictor predictor_encoder(true);

    SF_INFO inFileInfo;
    SNDFILE* inFile;

    inFileInfo.format = 0;
    inFile = sf_open(filename, SFM_READ, &inFileInfo);
    int channels = inFileInfo.channels;

    num_items = inFileInfo.frames*channels;

    /* Allocate space for the data to be read, then read it. */
    buf = (int *) malloc(num_items*sizeof(int));
    num = sf_read_int(inFile,buf,num_items);

    sf_close(inFile);
        
    int average = 0;
    for(i=0 ; i<20 ; i++)
        average += test[i];

    m=(int)ceil(-1/log2(average/(average+1.0)));
    golomb golomb_encoder(5,outfile);
    int avgsample = 0;
    for(i=0 ; i<num_items ; i+=channels)
    {
        avgsample = (buf[i]+buf[i+1])/2;
        int quant_value = quantize(predictor_encoder.residual(avgsample), 8);
        predictor_encoder.updateBufferConst(quant_value);
        golomb_encoder.signed_stream_encode(quant_value);
    }
    golomb_encoder.close_stream_write();
    //stream.close_file_read();
    printf("Read %d items\n",num);
}

int lossy_predictive::quantize(int sample,int nbits){
    int delta = (2147483647 - (-2147483648)) / (pow(2,nbits)-1);
    int qtsample = delta*floor(sample/delta+0.5);
    return qtsample; 
}
//nao implementado
void lossy_predictive::lossypredictive_decode(char* infile)
{
    //bit_stream stream(infile,true,false);
    predictor predictor_decoder(true);
    golomb golomb_decoder(5,infile);
    int code;
    cout << "------------------------------------" << endl;
    // unary_size = m/n
    // n = unary_size*m
    // k = n-unary_size*m
    // r = b  se r < k|| r = b+1 se r >= k
    int count = 0;
    while(!golomb_decoder.end_of_file()){
        code = golomb_decoder.signed_stream_decode();
        cout << endl;
        cout << "code: " << code << endl;      
        cout << "unary_size: " << golomb_decoder.get_unarySize() << endl;
        cout << "rem_size: " << golomb_decoder.get_remSize() << endl;
        count ++;
        if(count >= 22)
            break;
    }
    golomb_decoder.close_stream_read();
    //cout << "code: " << code << endl;
    //stream.close_file_read();
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
    lossy_predictive lossy((char*)file.data());
    lossy.lossypredictive_encode((char*)binfile.data());
    lossy.lossypredictive_decode((char*)binfile.data());

    return 0;
}