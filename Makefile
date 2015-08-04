
CPP=mpic++

split_type: split_type.cpp
	$(CPP) -o $@ $<

clean:
	rm -f split_type
