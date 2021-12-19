bit_stream.o: ./bit_stream/bit_stream.o
			g++ -c bit_stream/bit_stream.cpp bit_stream/bit_stream.h -o bitstream_output

stream_header.o: bit_stream/bit_stream.h
			g++ -c bit_stream/bit_stream.h 

golomb_header.o: Golomb/golomb.h
			g++ -c Golomb/golomb.h bit_stream/bit_stream.o

golomb.o:  ./Golomb/golomb.o
		g++ Golomb/main.cpp Golomb/golomb.cpp Golomb/golomb.h bit_stream/bit_stream.cpp bit_stream/bit_stream.h  -o golomb_output

lossless.o:  lossless.o
		g++ lossless_predictive.cpp Golomb/golomb.cpp predictor.cpp bit_stream/bit_stream.cpp -lsndfile -o lossless_output

lossless_header.o: lossless_predictive.h

predictor.o: g++ predictor.h 