#include "bit_stream.h"
#include <string>
// g++ bit_stream/main.cpp bit_stream/bit_stream.cpp

int main(int argc ,char *argv[])
{
    string file = "bitstream_file.bin";
    bit_stream bitstream_write((char*)file.data(),false,true);

    bitstream_write.writeBit(0);
    bitstream_write.writeBit(1);
    bitstream_write.writeBit(1);
    bitstream_write.writeBit(1); 
    bitstream_write.writeBit(0);
    bitstream_write.writeBit(0);
    bitstream_write.writeBit(0);
    bitstream_write.writeBit(1);

    bitstream_write.writeBit(0);
    bitstream_write.writeBit(1);
    bitstream_write.writeBit(1);
    bitstream_write.writeBit(1); 
    bitstream_write.writeBit(0);
    bitstream_write.writeBit(0);
    bitstream_write.writeBit(0);
    bitstream_write.writeBit(1);
    
    /*
    bitstream.writeBit(1);
    bitstream.writeBit(0);
    bitstream.writeBit(0);
    bitstream.writeBit(0);
    bitstream.writeBit(1);
    bitstream.writeBit(1);
    bitstream.writeBit(1);
    bitstream.writeBit(0);
    */

    bitstream_write.writeBits('q',8);
    bitstream_write.close_file_write();
    
    // ler char
    uint32_t* val=0;
    uint8_t aux = 0;
    bit_stream bitstream_read((char*)file.data(),true,false);
    char* code = bitstream_read.readBits(24);
    
    while(!bitstream_read.end_of_file())
    {
        cout << (int)bitstream_read.readBit();
    }
    cout << endl;

    bitstream_read.close_file_read();
    for(int j=0 ; j<3 ; j++){
        for(int i=0 ; i<8 ; i++)
        {
            cout << ((code[j] >> i) & 0x01) << endl;
        }
        cout << code[j] << endl;
    }
    
    return 0;
}