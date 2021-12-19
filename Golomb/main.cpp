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
    int test[] = {6,15,70,60,50,0,-10,-20,-7,0,0,0,5,6,6,8,6,3,10,0,0,-10};
    golomb golomb_encoder(m,(char*)file.data());
    for(int i=0 ; i<22 ; i++)
        golomb_encoder.signed_stream_encode(test[i]);
    
    golomb_encoder.close_stream();
    
    golomb golomb_decoder(m,(char*)file.data());
    for(int i=0 ; i<22 ; i++){
        out = golomb_decoder.signed_stream_decode();
        cout << "decode: "<< out << endl;
    }
    return 0;
}