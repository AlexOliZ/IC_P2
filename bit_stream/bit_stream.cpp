#include "bit_stream.h"
#include "math.h"
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
    bool bit_stream::end_of_file()
    {   
        return inputfile.eof();
    }

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
    
    void bit_stream::write_byte()
    {
        if(pointer_write < 7){
            //cout << "BYTE -> " << (int)byte << endl;
            //for(int i=0 ; i<8 ; i++)
            //    cout<< (int)((byte>>(i)) &0x01); 
            //byte = 0x0F;
            outputfile.write((char*)&byte,1);
            //cout << "byte: "<< (int)byte << endl;
            pointer_write = 0;
        }
    }

    void bit_stream::writeBit(uint8_t val)
    {
        byte |= (val & 0x01) << pointer_write;
        //cout << "bit: " << ((val & 0x01) << pointer_write) << ' ';
        if (pointer_write > 0) {
            pointer_write--;                    
            return;       
        }           
        
        //cout << endl;
        //for(int i=0 ; i<8 ; i++)
        //    cout<< (int)((byte>>(i)) &0x01);   
        //byte = 0x0F;
        outputfile.write((char*)&byte,1);
        pointer_write = 7;
        byte = 0;
    }
    
    void bit_stream::writeBits(int val, uint n)
    {   
        char bit;
        //cout << "val: " << val << endl;
        for(int i=n-1 ; i>=0 ; i--)
        {  
            bit = (val >> i%64) & 0x01;
            //bit = (val) & (0x01 << i);
            //cout << "bits: " << (int)bit << endl;
            writeBit(bit);
        }
    }

    void bit_stream::writeBits(char* val, uint n)
    {   
        int j;
        for(int i=0 ; i<(int)(n/8) ; i++)
        {
            for(j=0 ; j<8 && 8*i+j<n ; j++)
                writeBit((val[i] >> j) & 0x01);
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
        int i,j;
        if (pointer_read < 0) {
            inputfile.read((char*)&byte, 1); 
            //cout << "byte " << (int)byte << endl;
            //for(i=7 ; i>=0 ; i--){
            //    cout<< ((byte>>(i)) &0x01);
            //}
            //cout << endl;
            //cout << (int)byte << endl;
            pointer_read = 7;
        }
        //cout << "pointer: " << pointer_read << endl;
        val_byte = ((byte >> pointer_read) & 0x01); 
        pointer_read--;
        //cout << " read bit: " << (int)val_byte << endl;
        return val_byte;
    }

    void bit_stream::readBit(uint8_t* val)
    {
        // introduzir informação no byte da stream
        if(pointer_read < 0)
        {
            inputfile.read((char*)&byte,1);
            pointer_read = 7;
        }
        // se tem informação no byte da stream
        *val = ((byte >> pointer_read) & 0x01);
        pointer_read --;
    }
    
    char* bit_stream::readBits(uint n)
    {
        int i,j;
        char* value = (char*)malloc((uint)(n/8) + 1);
        for(j=(n-1)/8; j>=0; j--){
            for (i=7; i>=0; i--) {
                if(i+8*j < n)
                    value[j] |= (readBit()<<i);
                //cout<< ((value[j]>>(i)) &0x01);
            }
        }
        //cout << endl;
        //cout << "VALUE -> " << (int)value[0] << endl;
        return value;
    }
    
    void bit_stream::readBits(char* bits, uint n)
    {
        for(int j = 0; j*8 < n; j++){
            for (uint i = 0; i < 8; i++) {
                if(i+8*j < n)
                    bits[j] = (readBit()<<i); 
            }
        }
    }

    void bit_stream::writeChar(char val)
    {
        writeBits(val,sizeof(char)*8);
    }

    void bit_stream::writeChars(char* val, uint n)
    {
        int j;
        for(int i=0 ; i<(int)(n/8) ; i++)
        {
            for(j=0 ; j<8 ; j++)
                writeBit(val[i]);
        }
    }

    char bit_stream::readChar()
    {
        return readBits(sizeof(char)*8)[0];
    }

    void bit_stream::readChar(char* c)
    {
        c = readBits(sizeof(char)*8);
    }
//};
