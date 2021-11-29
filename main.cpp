#include "bit_stream.h"
#include <string>

int main(int argc ,char *argv[])
{
    string file = "test_file.bin";
    bit_stream bitstream((char*)file.data());
    //char* file = argv[1];
    //bit_stream bitstream(file);

    //ofstream outputfile ((char*)file.data(), ios::binary);
    //uint bit = 1;
    //outputfile.write((char*)&bit,1);
    //outputfile.write((char*)&bit,1);
    //outputfile.write((char*)&bit,1);
    //outputfile.write((char*)&bit,1);
    //outputfile.close();

    bitstream.writeBit('1');
    bitstream.writeBit('1');
    bitstream.writeBit('0');
    bitstream.writeBit('1');
    bitstream.writeBit('1');
    bitstream.writeBit('0');
    bitstream.writeBit('1');
    bitstream.writeBit('0');
    
    // ler char
    /*
    uint32_t* val;
    bitstream.readBits(val,8);
    for(int i=0 ; i<8 ; i++)
    {
       cout << "value: " << (val+i) << endl;
    }
    */
    return 0;
}