bit_stream.o: ./bit_stream/bit_stream.o
		g++ bit_stream/main.cpp bit_stream/bit_stream.cpp bit_stream/bit_stream.h -o bitstream_output

golomb.o:  ./Golomb/golomb.o
		g++ Golomb/main.cpp Golomb/golomb.cpp Golomb/golomb.h bit_stream/bit_stream.cpp bit_stream/bit_stream.h  -o golomb_output

lossless.o:  lossless.o
		g++ lossless_predictive.cpp Golomb/golomb.cpp predictor.cpp bit_stream/bit_stream.cpp -lsndfile -o lossless_output -std=c++17 `pkg-config --cflags --libs opencv4`

lossy.o: lossy.o
		g++ lossy_predictive.cpp Golomb/golomb.cpp predictor.cpp bit_stream/bit_stream.cpp -lsndfile -o lossy_output -std=c++17 `pkg-config --cflags --libs opencv4`