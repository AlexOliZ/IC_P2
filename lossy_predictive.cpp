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
    for(i=0 ; i<num ; i++){
        average += buf[i];
    }
    //average = average/22;
    m=(int)ceil(-1/log2(average/(average+1.0)));
    cout << m << endl;
    golomb golomb_encoder(m,outfile);
    long avgsample = 0;
    for(i=0 ; i<num ; i++)
    {
        //avgsample = (buf[i]+buf[i+1])/2;
        int residual = predictor_encoder.residual(buf[i]);

        int quant_value = quantize(residual,4);
        //cout << quant_value<<endl;
        //cout << quant_value << endl;
        predictor_encoder.updateBufferConst(residual);
        golomb_encoder.signed_stream_encode(residual);

    }
    golomb_encoder.close_stream_write();
    printf("Read %d items\n",num);
}

int lossy_predictive::quantize(int sample,int nbits){
    int delta = (2147483647 - (-2147483648)) / (pow(2,nbits)-1);
    int qtsample = delta*floor(sample/delta+0.5);
    //cout << qtsample << endl;
    return qtsample; 
}
/*
int lossy_predictive::quantize(int sample,int nbits)
{
    return sample & ~((1 << nbits) - 1);
}*/
void lossy_predictive::lossypredictive_decode(char* infile)
{
    predictor predictor_decoder(true);
    golomb golomb_decoder(m,infile);

    SF_INFO inFileInfo;
    SNDFILE* inFile;

    inFileInfo.format = 0;
    inFile = sf_open(filename, SFM_READ, &inFileInfo);
    int channels = inFileInfo.channels;
    int num_items = inFileInfo.frames*channels;

    sf_close(inFile);

    int* code = (int*)malloc(sizeof(int)*num_items);
    int count = 0;
    cout << "-----------" <<endl;
    while(!golomb_decoder.end_of_file() && count < num_items){
        //golomb_decoder.set_m(m);
        code[count] = predictor_decoder.reconstruct(golomb_decoder.signed_stream_decode());
        //cout << "code[count]="<< code[count] << endl;
        count++;
        //cout << "count =" << count << endl;
    }
    golomb_decoder.close_stream_read();

    const char* outfilename = "lossyoutput.wav";
    SNDFILE* outFile = sf_open (outfilename, SFM_WRITE, &inFileInfo);
    sf_write_int (outFile, code, num_items) ;
    sf_close(outFile) ;
}

int main(int argc, char* argv[])
{
    
    string file = "./wavfiles/sample01.wav";
    string binfile = "testfile.bin";
   
    lossy_predictive lossy((char*)file.data());
    lossy.lossypredictive_encode((char*)binfile.data());
    lossy.lossypredictive_decode((char*)binfile.data());

    return 0;
}