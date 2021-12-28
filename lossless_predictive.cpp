#include "lossless_predictive.h"
//s#include "./bit_stream/bit_stream.h"


//g++ lossless_predictive.cpp Golomb/golomb.cpp predictor.cpp bit_stream/bit_stream.cpp -lsndfile

using namespace std;
static int* check_validity;

SF_INFO lossless_predictive::predictive_encode(char* outfile){

    int *buf,i,predictor_val,num_items,num,channels;
    uint average=0;
    predictor predictor_encoder(false);
    double pak=0;

    SF_INFO inFileInfo;
    SNDFILE* inFile;

    inFileInfo.format = 0;
    inFile = sf_open(filename, SFM_READ, &inFileInfo);
    channels = inFileInfo.channels;

    num_items = inFileInfo.frames*channels;

    /* Allocate space for the data to be read, then read it. */
    buf = (int *) malloc(num_items*sizeof(int));
    num = sf_read_int(inFile,buf,num_items);

 
    sf_close(inFile);
    cout << "sizeof " << sizeof(SF_INFO) << endl;
    for(i=0 ; i<num_items ; i++){
        buf[i] = predictor_encoder.residual(buf[i]);
        average += buf[i]>=0?2*buf[i]:-2*buf[i]-1;
    }
    //average = average/num_items;
    cout << "average: " << average << endl;
    this->m=(uint)ceil(-1/log2(average/(average+1.0)));
    cout << "M: " << this->m << endl;
    cout << "size: " << (64*num_items) << endl;

    

    golomb golomb_encoder(this->m,outfile);
    
    for(i=0 ; i<num_items ; i++)
    {
        //predictor_val = predictor_encoder.residual(buf[i]);
        if(this->calc_hist){
            if (this->histogram_residual.find(buf[i])!= this->histogram_residual.end()){ //if the element exists
            this->histogram_residual[buf[i]]++; //increase the number of elements
            }else{
                this->histogram_residual[buf[i]]=1;
            }
        }
        //cout << buf[i] << endl;
        golomb_encoder.signed_stream_encode(buf[i]);
    }
    golomb_encoder.close_stream_write();
    printf("Read %d items\n",num_items);
    free(buf);
    if(this->calc_hist){
        for(std::map<double,int>::iterator it = this->histogram_residual.begin(); it != this->histogram_residual.end(); ++it) {
        pak = (double)it->second/num_items;
        if(pak > 0)
            this->entropy -= (log(pak)/log(16)) *pak;
        }
    }
    //for(std::map<double,int>::iterator it = histogram_residual.begin(); it != histogram_residual.end(); ++it) {
    //    cout << (int)it->first << ' ' << it->second << endl;
    //}
    //cout << "zero: " << histogram_residual[0] << endl;
    return inFileInfo;
}

double lossless_predictive::getEntropy(){
    return this->entropy;
}

void lossless_predictive::predictive_decode(char* infile,SF_INFO info)
{
    predictor predictor_decoder(false);
    golomb golomb_decoder(this->m,infile);
    int num_items,*code;
    int count = 0;

    num_items = info.channels*info.frames;
    code = (int*)malloc(sizeof(int)*num_items);
    count = 0;
    while(1){
        code[count] = predictor_decoder.reconstruct(golomb_decoder.signed_stream_decode());
        count ++;
        if(count>=info.frames*info.channels){
            break;
        }
    }
    golomb_decoder.close_stream_read();

    const char* outfilename = "output.wav";
    SNDFILE* outFile = sf_open (outfilename, SFM_WRITE, &info);
    sf_write_int (outFile, code, num_items) ;
    sf_close(outFile) ;
    free(code);
}

int main(int argc, char* argv[])
{
    string file = "./wavfiles/sample01.wav";
    string binfile = "testfile.bin";
    lossless_predictive lossless((char*)file.data(),true);
    SF_INFO info = lossless.predictive_encode((char*)binfile.data());
    lossless.predictive_decode((char*)binfile.data(),info);
    cout << "entropy=" <<lossless.getEntropy() << endl;
    cout << "min_size: " << (int)(info.frames*info.channels*ceil(lossless.getEntropy())) << endl;
    return 0;
}