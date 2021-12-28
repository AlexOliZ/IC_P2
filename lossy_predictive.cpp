#include "lossy_predictive.h"

//g++ lossy_predictive.cpp Golomb/golomb.cpp predictor.cpp bit_stream/bit_stream.cpp -lsndfile

using namespace std;
void lossy_predictive::lossypredictive_encode(char* outfile,int qtbits){

    char* code;
    int *buf,i,num_items,num,channels,quant_value,residual;
    int average = 0;
    predictor predictor_encoder(true);
    double pak = 0;

    SF_INFO inFileInfo;
    SNDFILE* inFile;

    inFileInfo.format = 0;
    inFile = sf_open(filename, SFM_READ, &inFileInfo);
    channels = inFileInfo.channels;

    num_items = inFileInfo.frames*channels;

    buf = (int *) malloc(num_items*sizeof(int));
    num = sf_read_int(inFile,buf,num_items);

    sf_close(inFile);
        
    for(i=0 ; i<num_items ; i++){
        residual = predictor_encoder.residual(buf[i]);
        quant_value = quantize(residual,qtbits);
        predictor_encoder.updateBufferConst(quant_value);
        buf[i] = quant_value;
        average += buf[i]>=0?2*buf[i]:-2*buf[i]-1;
    }
    m=(int)ceil(-1/log2(average/(average+1.0)));

    golomb golomb_encoder(m,outfile);

    
    for(i=0 ; i<num ; i++)
    {
        if(this->calc_hist){
            if (this->histogram_residual.find(buf[i])!= this->histogram_residual.end()){ //if the element exists
            this->histogram_residual[buf[i]]++; //increase the number of elements
            }else{
                this->histogram_residual[buf[i]]=1;
            }
        }
        golomb_encoder.signed_stream_encode(buf[i]);
    }
    golomb_encoder.close_stream_write();
    printf("Read %d items\n",num);
    free(buf);
    if(this->calc_hist){
        for(std::map<double,int>::iterator it = this->histogram_residual.begin(); it != this->histogram_residual.end(); ++it) {
        pak = (double)it->second/num_items;
        if(pak > 0)
            this->entropy -= (log(pak)/log(16)) *pak;
        }
    }
}

int lossy_predictive::quantize(int sample,int nbits){
    int delta = (2147483647 - (-2147483648)) / (pow(2,nbits)-1);
    int qtsample = delta*floor(sample/delta+0.5);
    return qtsample; 
}

double lossy_predictive::getEntropy(){
    return this->entropy;
}

void lossy_predictive::lossypredictive_decode(char* infile)
{
    int channels,num_items, *code;
    int count = 0;
    predictor predictor_decoder(true);
    golomb golomb_decoder(this->m,infile);

    SF_INFO inFileInfo;
    SNDFILE* inFile;

    inFileInfo.format = 0;
    inFile = sf_open(filename, SFM_READ, &inFileInfo);
    channels = inFileInfo.channels;
    num_items = inFileInfo.frames*channels;

    sf_close(inFile);

    code = (int*)malloc(sizeof(int)*num_items);

    while(count < num_items){

        code[count] = predictor_decoder.reconstruct(golomb_decoder.signed_stream_decode());
        count++;
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
    if(argc != 3){
        cout << "Incorrect argument list, use is: ./lossyaudio <nbitsqnt> <hist?>"<<endl;
    }
    int quant_bits = atoi(argv[1]);
    bool calculate_hist;
    if(argv[2] == "false"){
        calculate_hist = false;
    }
    else{
        calculate_hist = true;
    }
    lossy_predictive lossy((char*)file.data(),calculate_hist);
    lossy.lossypredictive_encode((char*)binfile.data(),quant_bits);
    lossy.lossypredictive_decode((char*)binfile.data());
    cout << "entropy=" << lossy.getEntropy() << endl;
    return 0;
}