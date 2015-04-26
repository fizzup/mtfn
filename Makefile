all: libmtfn.a mtfn test

clean:
	rm -f *.o mtfn libmtfn.a test_output.txt

test: test_output.txt
	diff test_output.txt test_reference.txt

test_output.txt: mtfn
	./mtfn test_input.txt > test_output.txt

mtfn: libmtfn.a test_metaphone.o
	g++ -o mtfn test_metaphone.o libmtfn.a

libmtfn.a: mtfn.o
	ar rcs libmtfn.a mtfn.o

mtfn.o: mtfn.cpp mtfn.h
	g++ -g -c -Wall -o mtfn.o mtfn.cpp 

test_metaphone.o: test_metaphone.cpp mtfn.h
	g++ -g -c -Wall -o test_metaphone.o test_metaphone.cpp 
