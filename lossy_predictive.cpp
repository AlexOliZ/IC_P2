#include "lossy_predictive.h"
#include <filesystem>
#include <chrono>

using namespace std::chrono;
//g++ lossy_predictive.cpp Golomb/golomb.cpp predictor.cpp bit_stream/bit_stream.cpp -lsndfile

using namespace std;

void lossy_predictive::lossypredictive_encode(char* outfile){
    auto start = high_resolution_clock::now();
    char* code;
    short *buf;
    int i,num_items,channels,quant_value,residual,count=0;
    predictor predictor_encoder(true);
    double pak = 0;

    SF_INFO inFileInfo;
    SNDFILE* inFile;

    inFileInfo.format = 0;
    inFile = sf_open(filename, SFM_READ, &inFileInfo);
    channels = inFileInfo.channels;

    num_items = inFileInfo.frames*channels;

    /* Allocate space for the data to be read, then read it. */
    buf = (short *) malloc(num_items*sizeof(short));
    sf_read_short(inFile,buf,num_items);
    sf_close(inFile);

    if(calc_hist){
        for(i=0 ; i<num_items ; i++){
            if (this->histogram_original.find(buf[i])!= this->histogram_original.end()){ //if the element exists
                this->histogram_original[buf[i]]++; //increase the number of elements
            }else{
                this->histogram_original[buf[i]]=1;
            }
        } 
    } 

    for(i=0 ; i<num_items ; i++){
        residual = (short)predictor_encoder.residual((int)buf[i]);
        quant_value = quantize((int)residual,qtbits);
        predictor_encoder.updateBufferConst((int)quant_value);
        buf[i] = (short)quant_value;
        m += buf[i]>=0?2*buf[i]:-2*buf[i]-1;
    }

    int average = m/num_items;
    for(i=0 ; i<num_items ; i++){
        if((buf[i]>=0 ? 2*buf[i] : -2*buf[i]-1) < average)
            count++;
    }
    
    // para testar com a average
    // m=m/num_items
    m=m/count;
    m = (uint)ceil(-1/log2(m/(m+1.0)));
    
    if(this->calc_hist){
        for(i=0 ; i<num_items ; i++)
        {
            if (this->histogram_residual.find(buf[i])!= this->histogram_residual.end()){ //if the element exists
                this->histogram_residual[buf[i]]++; //increase the number of elements
            }else{
                this->histogram_residual[buf[i]]=1;
            }
        }
    }
    
    golomb golomb_encoder(m,outfile);
    for(i=0 ; i<num_items ; i++)
    {
        golomb_encoder.signed_stream_encode((int)buf[i]);
    }
    
    golomb_encoder.close_stream_write();
    printf("Read %d items\n",num_items);
    free(buf);
    
    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    cout << "compression duration: "<< (double)duration.count()/1000000 << endl;

    if(this->calc_hist){
        for(std::map<int,int>::iterator it = this->histogram_original.begin(); it != this->histogram_original.end(); ++it) {
            pak = (double)it->second/num_items;
            if(pak > 0)
                this->entropy_original -= (log(pak)/log(16)) *pak;
        }
        for(std::map<int,int>::iterator it = this->histogram_residual.begin(); it != this->histogram_residual.end(); ++it) {
            pak = (double)it->second/num_items;
            if(pak > 0)
                this->entropy_residual -= (log(pak)/log(16)) *pak;
        }
    } 
}

int lossy_predictive::quantize(int sample,int nbits){
    int delta = (32767 - (-32768)) / (pow(2,nbits)-1);
    int qtsample = delta*floor(sample/delta+0.5);
    return qtsample; 
}

double lossy_predictive::getEntropyOriginal(){
    return this->entropy_original;
}
double lossy_predictive::getEntropyResidual(){
    return this->entropy_residual;
}

void lossy_predictive::dispHistogram(){
    int hist_w,hist_h,c,count;
    if(this->calc_hist){
        switch(this->qtbits){
            case 16:
                {
                    hist_w = 620; hist_h = 620; 
                    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(255, 255, 255)); 
                    c = 0;
                    count = 0;
                    for(std::map<int,int>::iterator it = histogram_residual.begin(); it != histogram_residual.end(); ++it) {
                        if(count % 35 == 0){
                            line(histImage, Point(c, hist_h), Point(c, hist_h-it->second),Scalar(0,0,0), 2,8,0);
                            c++;
                        }     
                        count++;
                    }
                    imshow("Histogram 16bits", histImage);
                    waitKey(0);
                    break;
                }
            case 8:
                { 
                    hist_w = 510; hist_h = 600; 
                    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(255, 255, 255)); 
                    c = 0;
                    count = 0;
                    for(std::map<int,int>::iterator it = histogram_residual.begin(); it != histogram_residual.end(); ++it) {
                        for(int k=0;k < 3; k++){
                            line(histImage, Point(c, hist_h), Point(c, hist_h-(it->second/500)),Scalar(0,0,0), 2,8,0);
                            c++;
                        } 
                        count++;
                    }         
                    // display histogram
                    imshow("Histogram 8bits", histImage);
                    waitKey(0);
                    break;
                }   
            case 4:
                {
                    hist_w = 600; hist_h = 600; 
                    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(255, 255, 255)); 
                    c = 0;
                    count = 0;
                    for(std::map<int,int>::iterator it = histogram_residual.begin(); it != histogram_residual.end(); ++it) {
                        for(int k=0;k < 40; k++){
                            line(histImage, Point(c, hist_h), Point(c, hist_h-(it->second/1500)),Scalar(0,0,0), 1,8,0);
                            c++;
                        }                         
                        count++;
                    }
                    // display histogram
                    imshow("Histogram 4bits", histImage);
                    waitKey(0);
                    break;            
                }
            default:
                {
                    cout << "Wrong number of bits" << endl;
                }
        }
    }   
}

void lossy_predictive::lossypredictive_decode(char* infile)
{
    auto start = high_resolution_clock::now();
    int channels,num_items;
    short *code;
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

    code = (short*)malloc(sizeof(short)*num_items);

    while(count < num_items){
        code[count] = predictor_decoder.reconstruct(golomb_decoder.signed_stream_decode());
        count++;
    }
    golomb_decoder.close_stream_read();

    const char* outfilename = "lossyoutput.wav";
    SNDFILE* outFile = sf_open (outfilename, SFM_WRITE, &inFileInfo);
    sf_write_short(outFile, code, num_items) ;
    sf_close(outFile) ;
    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    cout << "decompression duration: "<< (double)duration.count()/1000000 << endl;
}

int main(int argc, char* argv[])
{
    string binfile = "lossy_file.bin";
    if(argc != 4){
        cout << "Incorrect argument list, use is: ./lossyaudio <nomeficheiro> <nbitsqnt> <hist?>"<<endl;
        return 0;
    }
    int quant_bits = atoi(argv[2]);
    bool calculate_hist;

    std::string filename = argv[1];
    const char* path = "./wavfiles/";
    filename = path + filename;

    if(atoi(argv[3]) == 0){
        calculate_hist = false;
    }else{
        calculate_hist = true;
    }
    
    std::ofstream ofs;
    ofs.open(binfile, std::ofstream::out | std::ofstream::trunc);
    ofs.close();

    lossy_predictive lossy((char*)filename.data(),calculate_hist,quant_bits);
    lossy.lossypredictive_encode((char*)binfile.data());
    lossy.lossypredictive_decode((char*)binfile.data());
    
    cout << "compression="<< (double)std::filesystem::file_size(binfile)/std::filesystem::file_size(filename) <<endl;
    cout << "Residual Entropy=" << lossy.getEntropyResidual() << endl;
    cout << "Original Entropy=" << lossy.getEntropyOriginal() << endl;
    
    if(calculate_hist)
        lossy.dispHistogram();    
    return 0;
}