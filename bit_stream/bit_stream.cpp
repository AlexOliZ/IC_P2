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
    
    void bit_stream::open_file_write()
    {
        outputfile.open(filename);
    }

    void bit_stream::close_file_write()
    {
        outputfile.close();
    }

    void bit_stream::open_file_read()
    {
        inputfile.open(filename);

    }

    void bit_stream::close_file_read()
    {
        inputfile.close();
    }
    


    void bit_stream::writeBit(uint8_t val)
    {
        // vai ler o 1º bit e escrever na posiçao do pointer
        byte |= (val & 0x01) << pointer;
        cout << "val write: "<< val << endl;
        cout << "byte write: "<< byte << endl;
        cout << "pointer write: "<< pointer << endl; 
        if (pointer > 0) {
            pointer--;                    
            return;       
        }           
        //cout << filename << endl;     
        //fwrite(&byte, sizeof(byte), 1, fopen(filename,"wb"));
        //cout << (char)byte << endl;
        //if(!outputfile.is_open())
        //    outputfile.open(filename, fstream::out);
        outputfile.write((char*)&byte, 1);
        pointer = 7;
        //outputfile.close();
        byte = 0;
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
    /*
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
    */
    uint8_t bit_stream::readBit()
    {
        uint8_t val_byte=0;
        // introduzir informação no byte da stream
        if (pointer_read < 0) {
            inputfile.read((char*)&byte, 1); 
            cout << "value readfile: " << byte << endl;
            pointer_read = 7;
        }
        // se tem informação no byte da stream
        //if(!inputfile.is_open())
        //    inputfile.open(filename, fstream::out);
        //val_byte = ((byte >> pointer_read) & 0x01);
        //cout << "read bit: " << ((byte << pointer_read) & (0x01 << pointer_read)) << endl;
        cout << "read bit: " << ((byte >> pointer_read) & 0x01) << endl;
        val_byte = ((byte >> pointer_read) & 0x01); 
        //byte = byte >> 1;
        pointer_read--;
        return val_byte;
    }

    void bit_stream::readBit(uint8_t* val)
    {
        // introduzir informação no byte da stream
        if(pointer_read < 0)
        {
            inputfile.read((char*)&byte,1);
            pointer = 7;
        }
        // se tem informação no byte da stream
        *val = ((byte >> pointer_read) & 0x01);
        pointer_read --;
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
