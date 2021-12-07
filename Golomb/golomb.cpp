#include "golomb.h"
using namespace std;

// para um n inteiro positivo
// q = floor( (n-1)/m ) 
// r = n -q*m -1
// o codigo é codificado em ceilling( log2 b ) bits para os valores maiores

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
    unary_size =(uint)floor((n)/m);
    uint b=floor(log2(m));
    uint r= n -unary_size*m;
    uint k=pow(2,(b + 1))-m;
    int i=0,j=0;
    char* code = (char*)malloc((floor((b+1+unary_size+1)/8) +1));
    //cout << "size of rem -> " << (sizeof(char)*(floor((b+1+unary_size+1)/8) +1)) << endl;
    //cout << "size of rem -> " << sizeof(rem) << endl;
    //cout << "b: " << b << endl;
    //cout << "r: " << r << endl;
    //cout << "k: " << k << endl;
    //cout << "q: " << q << endl;

    if(r<k){      
        rem_size=b;
        for(j=0 ; j<sizeof(code)/8 && 8*j<b ; j++)
            //rem[j] |= r&(0x0FF<<j);
            for(i=0 ; i+8*j<b ; i++)
            {
                code[j] |= r &(0x01<<(i+8*j));
                //cout<< "bin: " << (r &(0x01<<(i+8*j))) <<endl;
            }

        //for(i=0 ; i<b ; i++)
        //    cout<< "check: " << ((rem[0]>>(i)) &0x01) <<endl;
                
        
    }else{
        rem_size=b+1;
        r += k;
        for(j=0 ; j<sizeof(code)/8 ; j++)
            for(i=0 ; i<8 ; i++)
            {
                if(i+8*j < b+1)
                {
                    code[j] |= r &(0x01<<(i+8*j));
                    //cout<< "else: " << (r &(0x01<<(i+8*j))) <<endl;
                }
            }
    }
    code[(int)floor(unary_size/8)] |= (0x01 << rem_size);
    unary_size++;
    return code;
}

char* golomb::signed_encode(int n)
{
    return encode(n>=0?2*n:-2*n-1);
}

/*char* golomb::decode(uint q,uint r)
{
    uint b=floor(log2(m));
    int j=0;
    int q_value=q-1;

    return 0;
}*/

uint golomb::decode(char* code){
    int i = 0;
    int num_q=unary_size-1;
    int remainder = 0;
    for(i = rem_size-1; i >=0; i--){
        //cout << "code["<<i<<"]="<< ((code[0]>>(i)) &0x01) <<endl;
        remainder += (pow(2,i)*((code[0]>>(i)) &0x01));
    }
    cout << "outcode" << num_q*m+remainder <<endl;
    return 0;
}

