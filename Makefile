
CPP=mpic++
CFLAGS=-std=c++11

split_type: split_type.cpp common.hpp
	$(CPP) $(CFLAGS) -o $@ $<

clean:
	rm -f split_type
