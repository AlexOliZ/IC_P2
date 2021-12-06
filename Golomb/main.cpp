#include "golomb.h"

using namespace std;

int main(void)
{
    int n;
    int max_n = 20;
    int m = 4;
    int i,j;
    char*code;
    golomb g(m);

    for(n=0 ; n<max_n ; n++){
        cout << "N -> " << n << endl;
        code = g.encode(n);
        for(j=0 ; j< floor((g.get_unarySize()+g.get_remSize())/8+1) ; j++)
        {
            for(i=7 ; i>=0 ; i--)
                if(i+8*j < g.get_unarySize()+g.get_remSize())
                    cout<< ((code[j]>>(i)) &0x01);
        }   
        cout<< endl;
        cout << "size: " << floor((g.get_unarySize()+g.get_remSize())) << endl;
        cout << "unary: " << floor((g.get_unarySize())) << endl;
        cout << "rem: " << floor((g.get_remSize()))<< endl;
        //g.free_code();
    }
    //for(i=0 ; i<8 ; i++)
    //    cout<< ((code[1]>>(i)) &0x01);
    //cout<< endl; 

    return 0;
}