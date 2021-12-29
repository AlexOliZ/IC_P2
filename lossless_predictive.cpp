#include "lossless_predictive.h"
#include <filesystem>
#include <chrono>

using namespace std::chrono;
  
//g++ lossless_predictive.cpp Golomb/golomb.cpp predictor.cpp bit_stream/bit_stream.cpp -lsndfile

using namespace std;

SF_INFO lossless_predictive::predictive_encode(char* outfile){
    auto start = high_resolution_clock::now();
    int i,predictor_val,num_items,channels,count=0;
    short *buf;
    predictor predictor_encoder(false);
    double pak=0;

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
        buf[i] = (short)predictor_encoder.residual((int)buf[i]);
        m += buf[i]>=0 ? 2*buf[i] : -2*buf[i]-1;
    }
    
    int average = m/num_items;
    for(i=0 ; i<num_items ; i++){
        if((buf[i]>=0 ? 2*buf[i] : -2*buf[i]-1) < average)
            count++;
    }

    // para testar com a average
    // m=m/num_items
    m=m/(count);
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

    golomb golomb_encoder(this->m,outfile);
    for(i=0 ; i<num_items ; i++)
    {
        golomb_encoder.signed_stream_encode(buf[i]);
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
    
    return inFileInfo;
}

void lossless_predictive::dispHistogram(){
    if(this->calc_hist){ 
        int hist_w = 600; int hist_h = 600; 
        Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(255, 255, 255)); 
        int c = 0;
        int count = 0;
        for(std::map<int,int>::iterator it = histogram_residual.begin(); it != histogram_residual.end(); ++it) {
            if(count % 95 == 0){
                line(histImage, Point(c, hist_h), Point(c, hist_h-it->second),Scalar(0,0,0), 2,8,0);
                c++;
            }     
            count++;
        }
        // display histogram
        imshow("Residual Histogram", histImage);
        waitKey(0);
    }
}

double lossless_predictive::getEntropyResidual(){
    return this->entropy_residual;
}
double lossless_predictive::getEntropyOriginal(){
    return this->entropy_original;
}
void lossless_predictive::predictive_decode(char* infile,SF_INFO info)
{
    auto start = high_resolution_clock::now();
    predictor predictor_decoder(false);
    golomb golomb_decoder(this->m,infile);
    int num_items;
    short *code;
    int count = 0;

    num_items = info.channels*info.frames;
    code = (short*)malloc(sizeof(short)*num_items);
    count = 0;
    while(1){
        code[count] = predictor_decoder.reconstruct(golomb_decoder.signed_stream_decode());
        count ++;
        if(count>=info.frames*info.channels){
            break;
        }
    }
    golomb_decoder.close_stream_read();

    const char* outfilename = "lossless.wav";
    SNDFILE* outFile = sf_open (outfilename, SFM_WRITE, &info);
    sf_write_short (outFile, code, num_items) ;
    sf_close(outFile) ;
    free(code);
    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    cout << "decompression duration: "<< (double)duration.count()/1000000 << endl;
}

int main(int argc, char* argv[])
{
    string binfile = "lossless_file.bin";
    if(argc != 3){
        cout << "Incorrect argument list, use is: ./lossyaudio <nomeficheiro> <hist?>"<<endl;
        return;
    }
    
    std::string filename = argv[1];
    const char* path = "./wavfiles/";
    filename = path + filename;

    bool calculate_hist;
    if(atoi(argv[2]) == 0){
        calculate_hist = false;
    }else{
        calculate_hist = true;
    }
    
    /* para limpar o ficheiro */
    std::ofstream ofs;
    ofs.open(binfile, std::ofstream::out | std::ofstream::trunc);
    ofs.close();

    lossless_predictive lossless((char*)filename.data(),calculate_hist);
    SF_INFO info = lossless.predictive_encode((char*)binfile.data());
    lossless.predictive_decode((char*)binfile.data(),info);
    
    cout << "Residual Entropy=" << lossless.getEntropyResidual() << endl;
    cout << "Original Entropy=" << lossless.getEntropyOriginal() << endl;
    cout << "compression="<< (double)std::filesystem::file_size(binfile)/std::filesystem::file_size(filename) <<endl;
    if(calculate_hist)
        lossless.dispHistogram();
    return 0;
}