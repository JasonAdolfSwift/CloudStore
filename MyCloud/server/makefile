CFLAGS = -g
exe:server.o
	g++ -o exe server.o
server.o:server.cc ../public.h server.h processpool.h
	g++ -c server.cc server.h ../public.h processpool.h -g
clean:
	rm *.o *.gch
