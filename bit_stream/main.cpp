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

    bitstream.writeBit(0);
    bitstream.writeBit(1);
    bitstream.writeBit(1);
    bitstream.writeBit(1);
    bitstream.writeBit(0);
    bitstream.writeBit(0);
    bitstream.writeBit(0);
    bitstream.writeBit(1);
    bitstream.writeBit(0);
    bitstream.writeBit(1);
    bitstream.writeBit(1);
    bitstream.writeBit(1);
    bitstream.writeBit(0);
    bitstream.writeBit(0);
    bitstream.writeBit(0);
    bitstream.writeBit(1);
    bitstream.writeBit(0);
    bitstream.writeBit(1);
    bitstream.writeBit(1);
    bitstream.writeBit(1);
    bitstream.writeBit(0);
    bitstream.writeBit(0);
    bitstream.writeBit(0);
    bitstream.writeBit(1);
    bitstream.close_file_write();
    //bitstream.writeBit('1');
    //bitstream.writeBit('1');
    //bitstream.writeBit('0');
    //bitstream.writeBit('1');
    //bitstream.writeBit('0');
    
    // ler char
    
    uint32_t* val=0;
    uint8_t aux = 0;
    uint8_t value = bitstream.readBits(8);
    bitstream.close_file_read();
    for(int i=0 ; i<8 ; i++)
    {
        aux |= ((value >> i) & 0x01);
        cout << ((value >> i) & 0x01) << endl;
    }
    cout << "value: " << ((char)value)  << endl;
    cout << "comp: " << (aux&&'a')<<endl;
    
    return 0;
}