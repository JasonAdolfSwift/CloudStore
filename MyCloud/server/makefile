CFLAGS = -g
ser:server.o
	g++ -o ser server.o
server.o:server.cpp public.h server.h processpool.h
	g++ -c server.cpp server.h public.h processpool.h -g
clean:
	rm *.o *.gch
