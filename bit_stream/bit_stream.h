#ifndef H_BITSTREAM
#define H_BITSTREAM

#include <iostream>
#include <fstream>
#include <bitset>

using namespace std;

class bit_stream {
    public:
        /**
         * @brief 
         * 
         * @param fname name of the file the bit_stream will read or write.
        */
        bit_stream(char *fname){
            pointer_read = -1;
            pointer_write = 7;
            filename = fname;
            byteCount = 0;
            inputfile = ifstream(filename, ios::binary);
            outputfile = ofstream(filename, ios::binary|ios_base::app);
            wbyte = 0;      
            rbyte = 0;      
        };
        /**
         * @brief 
         * 
         * @param fname name of the file the bit_stream will read or write.
         * @param read_file enables or disables read functions.
         * @param write_file enables or disables write functions.
        */
        bit_stream(char *fname,bool read_file,bool write_file){
            pointer_read = -1;
            pointer_write = 7;
            filename = fname;
            byteCount = 0;
            wbyte = 0;      
            rbyte = 0;   
            if(read_file)
                inputfile = ifstream(filename, ios::binary);
            if(write_file)
                outputfile = ofstream(filename, ios::binary|ios_base::app);            
        };
        bit_stream(){};

        /**
         * @brief 
         * reopens the ofstream.
        */
        void open_file_write();
        
        /**
         * @brief 
         * closes the ofstream.
        */
        void close_file_write();
        
        /**
         * @brief 
         * reopens the ifstream.
        */
        void open_file_read();
        
        /**
         * @brief 
         * closes the ifstream.
        */
        void close_file_read();
        
        /**
         * @brief 
         * checks if ifstream reached the end of the file.
        */
        bool end_of_file();
        
        /**
         * @brief 
         * writes the bits saved in the bit_stream and closes the file.
        */
        void write_byte();

        /**
         * @brief function used to write 1 bit.
         * because we can only write 1 byte at a time the value will be writen.
         * after 8 bits have been inserted in the bit_stream.
         * @param  val bit that will be writen.
        */
        void writeBit(uint8_t val);
        
        /**
         * @brief function used to write N bits.
         * @param val bits that will be writen.
         * @param n number of bits that will be writen, maximum of 64 bits. 
        */
        void writeBits(int val, uint n);
        
        /**
         * @brief function used to write N bits using a char*.
         * @param val bits that will be writen.
         * @param n number of bits that will be writen. 
        */
        void writeBits(char* val, uint n);
        
        /**
         * @brief function used to read 1 bit.
         * @return bit read from the file.
        */
        uint8_t readBit();
        
        /**
         * @brief function used to read a bit using a pointer.
         * @param bit variable that will save the value read from the file.
        */
        void readBit(uint8_t* bit);
        
        /**
         * @brief function used to read N bits.
         * @param n number of bits that will be read from the file. 
         * @return N bits read from the file.
        */
        char* readBits(uint n);
        
        /**
         * @brief function used to read N bits using a char*.
         * @param bits pointer that will save the bits read from the file.
         * @param n number of bits that will be read from the file. 
        */
        void readBits(char* bits, uint n);
        
    private:
        uint8_t  wbyte;
        uint8_t  rbyte;
        char* filename;
        ofstream outputfile;
        ifstream inputfile;
        int pointer_read;
        int pointer_write;
        int byteCount;
};

#endif