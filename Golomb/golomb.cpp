#include "golomb.h"
using namespace std;

// unary || 1 || remainder

uint golomb::get_unarySize(){ return unary_size; }
uint golomb::get_remSize(){ return rem_size; }
int golomb::get_m(){ return m; }
void golomb::set_m(int new_m){ m = new_m; }
void golomb::close_stream_write(){ stream.write_byte(); stream.close_file_write(); }
void golomb::close_stream_read(){ stream.close_file_read(); }
bool golomb::end_of_file(){ return stream.end_of_file(); }

char* golomb::encode(uint n)
{
    this->unary_size = (uint)((n)/this->m);
    uint b=ceil(log2(this->m));
    uint r = n -unary_size*this->m;
    uint k=pow(2,(b))-this->m;
    int i=0,j=0;
    char* code = (char*)malloc(((int)((b+1+this->unary_size+1)/8) +1));
    
    if(r<k){      
        this->rem_size=b;
        for(j=0 ; 8*j<this->rem_size ; j++)
            for(i=0 ; i+8*j<this->rem_size ; i++)
            {
                code[j] |= r &(0x01<<(i));
            }                    
    }else{
        this->rem_size=b+1;
        r += k;
        for(j=0 ; 8*j<this->rem_size ; j++)
            for(i=0 ; i+8*j<this->rem_size ; i++)
            {
                code[j] |= r &(0x01<<(i));
            }
    }

    code[(int)((this->rem_size)/8)] |= 0x01 << (this->rem_size-((int)(this->rem_size)/8)*8);
    return code;
}


void golomb::stream_encode(uint n)
{
    this->unary_size = (uint)((n)/this->m);
    uint b=ceil(log2(this->m));
    uint r = n -this->unary_size*this->m;
    uint k=pow(2,b)-this->m;
    int i=0,j=0;
    
    this->stream.writeBits(0,this->unary_size);
    this->stream.writeBit(1);

    if(r<k){      
        this->rem_size=b-1;
    }else{
        this->rem_size=b;
        r += k;
    }
    
    this->stream.writeBits(r,this->rem_size);
}

char* golomb::signed_encode(int n)
{
    return encode(n>=0?2*n:-2*n-1);
}

void golomb::signed_stream_encode(int n)
{
    stream_encode((uint)(n>=0?2*n:-2*n-1));
}

uint golomb::decode(char* code,uint remainder_size, uint unary_size)
{
    int remainder = 0;
    int i,j;
    uint b=ceil(log2(this->m));
    uint k=pow(2,b)-this->m;

    for(j=0 ; j*8<remainder_size ; j++)
        for(i=0; i<8 & i+8*j < remainder_size; i++){
            remainder += (pow(2,i)*((code[j]>>(i)) &0x01));
        }
    if(b<remainder_size)
        remainder-=k;
    return (unary_size)*m+remainder;
}

uint golomb::stream_decode()
{   
    this->unary_size = 0;
    while(this->stream.readBit() != 1){
        this->unary_size ++;
    }
    
    this->rem_size=ceil(log2(this->m));
    uint k=pow(2,(this->rem_size))-this->m;
    
    int j,i;
    
    this->rem_size--;
    char* code = (char*)malloc((uint)(this->rem_size/8) + 1);
    this->stream.readBits(code,this->rem_size);
    uint remainder=0;
    
    for(j=0 ; 8*j < this->rem_size ; j++){
        for(i=0; i<8; i++){
            if(i+8*j < this->rem_size){
                remainder += (code[j]&(0x01 << (i))) << (8*j);
            }
        }
    }
    
    if(remainder >= k)
    {
        remainder = remainder << 1;
        remainder += this->stream.readBit() -k;
        rem_size +=1;
    } 

    free(code);
    return remainder+(this->m*this->unary_size);
}

int golomb::signed_decode(char*code,uint remainder_size,uint unary_size)
{
	uint res=decode(code,remainder_size,unary_size);
    return res%2 ? -ceil((res+1)/2) : res/2;
}

int golomb::signed_stream_decode()
{
    uint res=stream_decode();
    return res%2 ? -ceil((res+1)/2) : res/2;
}