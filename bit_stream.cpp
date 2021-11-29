#include "bit_stream.h"
using namespace std;
/*
*   bit stream to read/write bits from/to file
*   methods -> write,read one bit
*   write/read nbits
*   writes to bin file
*   read/write strings in binary
*   
*
*/
//class bit_stream
//{
    /*
    void open_file(char* filename)
    {
        myfile.open(filename);

    }

    void close_file(ofstream myfile)
    {
        myfile.close();
    }
    */


    void bit_stream::writeBit(uint8_t val)
    {
        byte |= (val & 0x01) << pointer;
        if (pointer > 0) {
            pointer--;                    
            return;       
        }           
        //cout << filename << endl;     
        //fwrite(&byte, sizeof(byte), 1, fopen(filename,"wb"));
        //cout << (char)byte << endl;
        if(!outputfile.is_open())
            outputfile.open(filename, fstream::out);
        outputfile.write((char*)&byte, 1);
        pointer = 7;
        outputfile.close();
        //byte = 0;
    }
    
    void bit_stream::writeBits(uint32_t val, uint n)
    {
        uint bit;
        for (int i = n-1; i >= 0; i--) {
            bit = (val >> i%32) & 0x01;
            writeBit(bit);
        }
        //outputfile.close();
    }

    void bit_stream::writeBits(uint32_t* val, uint n)
    {
        for (uint i=0; i<n/32 ; i++)
        {
            if(i == n/32)
                writeBits(val[i],n%32);
            else
                writeBits(val[i],32);
        }
        //outputfile.close();
    }

    uint8_t bit_stream::readBit()
    {
        uint8_t bit;
        // introduzir informação no byte da stream
        if (pointer < 0) {
            inputfile.read((char*)&byte, 1); 
            pointer = 7;
        }
        // se tem informação no byte da stream
        if(!inputfile.is_open())
            inputfile.open(filename, fstream::out);
        bit = ((byte >> pointer) & 0x01);
        pointer--;
        return bit;
    }

    void bit_stream::readBit(uint8_t* bit)
    {
        // introduzir informação no byte da stream
        if(pointer < 0)
        {
            inputfile.read((char*)&byte,1);
            pointer = 7;
        }
        // se tem informação no byte da stream
        *bit = ((byte >> pointer) & 0x01);
        pointer --;
    }
    
    uint32_t bit_stream::readBits(uint n)
    {
        //n = n&0xFFFF; // para uint_32
        uint64_t value=0;
        for(int i=0 ; i<n ; i++)
        {
            value = value << 1 | readBit();
        }
        return value;
    }

    void bit_stream::readBits(uint32_t* bits, uint n)
    {
        for(uint i=0 ; i<n ; i++)
        {
            bits[i] = readBit();
        }
    }

    void bit_stream::writeChar(char val)
    {
        writeBits(val,sizeof(char)*8);
    }

    void bit_stream::writeChars(char* val, uint n)
    {
        for(int i=0 ; i<n ; i++)
        {
            *(val+i) = readBits(sizeof(char)*8);
        }
    }

    char bit_stream::readChar()
    {
        return readBits(sizeof(char)*8);
    }

    void bit_stream::readChar(char* c)
    {
        *c = readBits(sizeof(char)*8);
    }

    void bit_stream::readChars(char* c, uint n)
    {
        for(uint i=0 ; i<n ; i++)
        {
            *(c+i) = readBits(sizeof(char)*8);
        }
    }
//};
