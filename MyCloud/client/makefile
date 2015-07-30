CFLAGS = -g
cli:client.o clitest.o 
	g++ -o cli client.o clitest.o 
client.o:client.cpp public.h client.h
	g++ -c client.cpp client.h public.h -g
clitest.o:clitest.cpp public.h client.h
	g++ -c clitest.cpp public.h client.h -g
clean:
	rm *.o *.gch
