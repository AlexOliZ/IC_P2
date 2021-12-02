#ifndef H_BITSTREAM
#define H_BITSTREAM

#include <iostream>
#include <fstream>
#include <bitset>
#include <cassert>

using namespace std;

class bit_stream {
    public:
        bit_stream(char *fname){
            pointer_read = -1;
            pointer_write = 7;
            pointer = 7;
            filename = fname;
            byteCount = 0;
            inputfile = ifstream(fname, ios::binary);
            outputfile = ofstream(fname, ios::binary);
            byte = '0';
            //inputfile.open(fname, fstream::in);
            //outputfile.open(fname, fstream::out);
            
        };
        //~bitstream();
        //void open_file(char* filename);
        //void close_file(ofstream myfile);
        void writeBit(uint8_t val);
        void writeBits(uint32_t val, uint n);
        void writeBits(uint32_t* val, uint n);
        uint8_t readBit();
        void readBit(uint8_t* bit);
        uint32_t readBits(uint n);
        void readBits(uint32_t* bits, uint n);
        void writeChar(char val);
        void writeChars(char* val, uint n);
        char readChar();
        void readChar(char* c);
        void readChars(char* c, uint n);
        
    private:
        uint8_t  byte = 0;
        //uint8_t byte_read = 0;
        //uint8_t byte_write = 0;
        char* filename;
        ofstream outputfile;
        ifstream inputfile;
        int pointer_read;
        int pointer_write;
        int pointer;
        int byteCount;
        bool writeMode;
};

#endif