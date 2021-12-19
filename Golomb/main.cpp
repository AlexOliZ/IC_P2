#include "golomb.h"
#include <string>

using namespace std;

int main(void)
{
    int n;
    int max_n = 15;
    int m = 5;
    int i,j;
    char*code;
    golomb g(m);
    int out =0;

    for(n=-max_n ; n<max_n ; n++){
        cout << "encode val -> " << n << endl;
        code = g.signed_encode(n);
        for(j=0 ; j< floor((g.get_unarySize()+g.get_remSize()+1)/8)+1 ; j++)
        {
            for(i=0 ; i<8 ; i++){
                if(i+8*j < g.get_unarySize()+g.get_remSize()+1) {
                    cout<< ((code[j]>>(i)) &0x01);
                }
            }
        }   
        cout<< endl;
        //cout << "size: " << floor((g.get_unarySize()+g.get_remSize())) << endl;
        //cout << "unary: " << floor((g.get_unarySize())) << endl;
        //cout << "rem: " << floor((g.get_remSize()))<< endl;

        out = g.signed_decode(code,g.get_remSize(),g.get_unarySize());
        cout << "decode val: "<< out <<endl;
        //g.free_code();
    }
    //for(i=0 ; i<8 ; i++)
    //    cout<< ((code[1]>>(i)) &0x01);
    //cout<< endl; 

    string file = "test_file.bin";
    golomb golomb_encoder(m,(char*)file.data());
    
    // 00100000
    
    golomb_encoder.signed_stream_encode(10);
    
    bit_stream stream((char*)file.data(),true,false);
    
    //cout << "unary_size: " << golomb_encoder.get_unarySize() << endl;
    //cout << "rem_size: " << golomb_encoder.get_remSize() << endl;

    /*
    int un_size = 0;
    while(stream.readBit() != 1)
        un_size++;
    cout << endl;
    cout << "un_size: " << un_size << endl;
    */
    
    code = stream.readBits(golomb_encoder.get_remSize() + golomb_encoder.get_unarySize() + 1);
    cout << "code: ";
    
    for(j=0 ; j< 1+(int)(golomb_encoder.get_remSize() + golomb_encoder.get_unarySize() + 1)/8 ; j++)
    {
        for(i=0 ; i<8 ; i++)
            if(i+8*j < golomb_encoder.get_unarySize()+golomb_encoder.get_remSize()+1)
                cout<< ((code[j]>>(i)) &0x01);   
    }   
    cout<< endl;
    cout << "VAL " <<(int)code[0]<< endl;
    stream.close_file_read();
    
    out = g.signed_decode((char*)code,golomb_encoder.get_remSize(),golomb_encoder.get_unarySize());
    cout << "decode: "<<out << endl;
    
    return 0;
}