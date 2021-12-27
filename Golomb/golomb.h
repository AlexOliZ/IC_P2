#ifndef H_GOLOM
#define H_GOLOM

#include <tuple>
#include <cmath>
#include <iostream>
#include <string.h>
#include "../bit_stream/bit_stream.h"

using namespace std;

class golomb
{
    public:
        /**
         * @brief 
         * 
         * @param val value of M.
        */
        golomb(uint val){
            m=val;
            rem_size=0;
            unary_size=0;
        };
        /**
         * @brief 
         * 
         * @param val value of M.
         * @param fname name of the file to encode and decode.
        */
        golomb(uint val,char* fname)
        {
            filename = fname;
            m = val;
            stream = bit_stream(fname,true,true);
            rem_size=0;
            unary_size=0;
        }
        golomb(){};

        /**
         * @brief 
         * @return M used by the golomb encoding/decoding algorithm.
        */
        int get_m();

        /**
         * @brief Function used to change the value of M.
        */
        void set_m(int new_m);

        /**
         * @brief 
         * @return unary size of the last code.
         */
        uint get_unarySize();

        /**
         * @brief 
         * @return remainder size of the last code.
         */
        uint get_remSize();

        /**
         * @brief 
         * @param n value to encode.
         * @return golomb code.
         */
        char* encode(uint n);

         /**
         * @brief 
         * @param n signed value to encode.
         * @return golomb code.
         */
        char* signed_encode(int n);

         /**
         * @brief 
         * @param code golomb code that will be decoded.
         * @param remainder reminader size.
         * @param unary_size unary size.
         * @return value decoded.
         */
        uint decode(char* code,uint remainder, uint unary_size);

        /**
         * @brief 
         * @param code golomb code of signed value that will be decoded.
         * @param remainder reminader size.
         * @param unary_size unary size.
         * @return value decoded.
         */
        int signed_decode(char*code,uint remainder_size,uint unary_size);

        /**
         * @brief the code will be writen in the file provided in the constructor using the bit_stream.
         * @param n value to encode.
         */
        void stream_encode(uint n);

        /**
         * @brief the code will be writen in the file provided in the constructor using the bit_stream.
         * @param n signed value to encode.
         */
        void signed_stream_encode(int n);

        /**
         * @brief the code will be read from file provided in the constructor using the bit_stream.
         * @return value decoded.
         */
        uint stream_decode();

         /**
         * @brief the code will be read from file provided in the constructor using the bit_stream.
         * @return signed value decoded.
         */
        int signed_stream_decode();
        
         /**
         * @brief closes the ofstrem.
         */
        void close_stream_write();

        /**
         * @brief closes the ifstrem.
         */
        void close_stream_read();

        /**
         * @brief 
         * @return checks if the bit_stream reached the end of the file.
         */
        bool end_of_file();

    private:
        uint m;
        uint unary_size;
        uint rem_size;
        char* filename;
        bit_stream stream;
};
#endif