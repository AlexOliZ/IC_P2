#ifndef H_BITSTREAM
#define H_BITSTREAM

#include <iostream>
#include <fstream>
#include <bitset>

using namespace std;

class bit_stream {
    public:
        bit_stream(char *fname){
            pointer_read = -1;
            pointer_write = 7;
            filename = fname;
            byteCount = 0;
            inputfile = ifstream(filename, ios::binary);
            outputfile = ofstream(filename, ios::binary|ios_base::app);
            byte = 0;
            //inputfile.open(fname, fstream::in);
            //outputfile.open(fname, fstream::out);
            
        };
        bit_stream(char *fname,bool read_file,bool write_file){
            pointer_read = -1;
            pointer_write = 7;
            filename = fname;
            byteCount = 0;
            if(read_file)
                inputfile = ifstream(filename, ios::binary);
            if(write_file)
                outputfile = ofstream(filename, ios::binary|ios_base::app);
            byte = 0;
            //inputfile.open(fname, fstream::in);
            //outputfile.open(fname, fstream::out);
            
        };
        bit_stream();
        
        //void open_file(char* filename);
        //void close_file(ofstream myfile);
        void open_file_write();
        void close_file_write();
        void open_file_read();
        void close_file_read();
        bool end_of_file();
        void write_byte();

        void writeBit(uint8_t val);
        void writeBits(int val, uint n);
        void writeBits(char* val, uint n);
        uint8_t readBit();
        void readBit(uint8_t* bit);
        char* readBits(uint n);
        void readBits(char* bits, uint n);
        void writeChar(char val);
        void writeChars(char* val, uint n);
        char readChar();
        void readChar(char* c);
        
    private:
        uint8_t  byte = 0;
        //uint8_t byte_read = 0;
        //uint8_t byte_write = 0;
        char* filename;
        ofstream outputfile;
        ifstream inputfile;
        int pointer_read;
        int pointer_write;
        int byteCount;
};

#endif