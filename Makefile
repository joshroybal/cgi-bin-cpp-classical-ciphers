cppciphers.cgi: main.o cipher.o cgilib.o freqan.o hypertext.o
	g++ -s -o cppciphers.cgi main.o cipher.o cgilib.o freqan.o hypertext.o

main.o: main.cpp cipher.hpp cgilib.hpp freqan.hpp hypertext.hpp
	g++ -O2 -c main.cpp

cipher.o: cipher.cpp cipher.hpp
	g++ -O2 -c cipher.cpp

cgilib.o: cgilib.cpp cgilib.hpp
	g++ -O2 -c cgilib.cpp

freqan.o: freqan.cpp freqan.hpp cipher.hpp
	g++ -O2 -c freqan.cpp

hypertext.o: hypertext.cpp hypertext.hpp
	g++ -O2 -c hypertext.cpp

install:
	sudo cp cppciphers.cgi /srv/httpd/cgi-bin
	sudo chown apache:apache /srv/httpd/cgi-bin/cppciphers.cgi
	sudo cp cppciphers.html /srv/httpd/htdocs
	sudo chown apache:apache /srv/httpd/htdocs/cppciphers.html

clean:
	rm cppciphers.cgi && rm *.o
