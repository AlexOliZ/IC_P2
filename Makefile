bit_stream.o: ./bit_stream/bit_stream.cpp
			g++ bit_stream/main.cpp bit_stream/bit_stream.cpp -o bitstream

stream_header.o: bit_stream/bit_stream.h
			g++ bit_stream/bit_stream.h 

golomb_header.o: Golomb/golomb.h
			g++ Golomb/golomb.h

golomb.o:  Golomb/golomb.cpp
		g++ Golomb/main.cpp Golomb/golomb.cpp bit_stream/bit_stream.cpp -o golomb

lossless.o:  g++ lossless_predictive.cpp Golomb/golomb.cpp predictor.cpp bit_stream/bit_stream.cpp -lsndfile

lossless_header.o: lossless_predictive.h

predictor.o: g++ predictor.h 