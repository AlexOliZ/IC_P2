#include "golomb.h"
using namespace std;

void golomb::free_code(){ free(code); }
uint golomb::get_unarySize(){ return unary_size; }
uint golomb::get_remSize(){ return rem_size; }
int golomb::get_m(){ return m; }
void golomb::set_m(int new_m)
{
    m = new_m; 
}

char* golomb::encode(uint n)
{
    unary_size = (int)((n)/m);
    uint b=(int)(log2(m));
    uint r = n -unary_size*m;
    uint k=pow(2,(b + 1))-m;
    int i=0,j=0;
    char* code = (char*)malloc(((int)((b+1+unary_size+1)/8) +1));

    if(r<k){      
        rem_size=b;
        for(j=0 ; 8*j<rem_size ; j++)
            //code[j] |= r&(0x0FF<<(j<<3));
            for(i=0 ; i+8*j<rem_size ; i++)
            {
                code[j] |= r &(0x01<<(i));
                //cout<< "bin: " << (r &(0x01<<(i+8*j))) <<endl;
            }                    
    }else{
        rem_size=b+1;
        r += k;
        for(j=0 ; 8*j<rem_size ; j++)
            for(i=0 ; i+8*j<rem_size ; i++)
            {
                code[j] |= r &(0x01<<(i));
            }
    }

    code[(int)((rem_size)/8)] |= 0x01 << (rem_size-((int)(rem_size)/8)*8);
    //cout << "unary: " << unary_size << endl;
    //cout << "rem_size: " << rem_size << endl;
    return code;
}

void golomb::stream_encode(uint n)
{
    bit_stream stream(filename,false,true);
    unary_size = (int)((n)/m);
    uint b=(int)(log2(m));
    uint r = n -unary_size*m;
    uint k=pow(2,(b + 1))-m;
    int i=0,j=0;

    stream.writeBits(0,unary_size);
    stream.writeBit(1);

    if(r<k){      
        rem_size=b;
        stream.writeBits(r,rem_size);
    }else{
        rem_size=b+1;
        r += k;
        stream.writeBits(r,rem_size);
    }

    //cout << "unary: " << unary_size << endl;
    //cout << "rem_size: " << rem_size << endl;
    stream.write_byte();
    stream.close_file_write();
}

char* golomb::signed_encode(int n)
{
    return encode(n>=0?2*n:-2*n-1);
}

void golomb::signed_stream_encode(uint n)
{
    stream_encode(n>=0?2*n:-2*n-1);
}

uint golomb::decode(char* code,uint remainder_size, uint unary_size)
{
    int remainder = 0;
    int i,j;
    uint b=(int)(log2(m));
    uint k=pow(2,(b + 1))-m;

    for(j=0 ; j*8<remainder_size ; j++)
        for(i=0; i+8*j < remainder_size; i++){
            remainder += (pow(2,i)*((code[j]>>(i)) &0x01));
        }
    if(b<remainder_size)
        remainder-=k;
    return (unary_size)*m+remainder;
}

int golomb::signed_decode(char*code,uint remainder_size,uint unary_size){
	//odd -> negative 
	//even -> positive
	uint res=decode(code,remainder_size,unary_size);
	return res%2 ? ((int)res+1)/-2 : res/2;
}

uint stream_decode()
{
    return 0;
}

int signed_stream_decode()
{
    uint res=stream_decode();
	return res%2 ? ((int)res+1)/-2 : res/2;
}