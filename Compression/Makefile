all: compress

compress : main.C compress.C testVector.C
	g++ -Wall -g -O2 -o $@ main.C compress.C testVector.C

clean:
	rm -f *.o compress
