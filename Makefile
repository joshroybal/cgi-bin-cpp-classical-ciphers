all: cppciphers.cgi cppattack.cgi

cppciphers.cgi: main.o cipher.o cgilib.o freqan.o hypertext.o
	g++ -s -o cppciphers.cgi main.o cipher.o cgilib.o freqan.o hypertext.o

cppattack.cgi: attack.o cipher.o cgilib.o freqan.o hypertext.o
	g++ -s -o cppattack.cgi attack.o cipher.o cgilib.o freqan.o hypertext.o

main.o: main.cpp cipher.hpp cgilib.hpp freqan.hpp hypertext.hpp
	g++ -O2 -c main.cpp

attack.o: attack.cpp cipher.hpp cgilib.hpp freqan.hpp hypertext.hpp
	g++ -O2 -c attack.cpp

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
	sudo cp cppattack.cgi /srv/httpd/cgi-bin
	sudo chown apache:apache /srv/httpd/cgi-bin/cppciphers.cgi
	sudo chown apache:apache /srv/httpd/cgi-bin/cppattack.cgi
	sudo cp cppciphers.html /srv/httpd/htdocs
	sudo cp cppattack.html /srv/httpd/htdocs
	sudo chown apache:apache /srv/httpd/htdocs/cppciphers.html
	sudo chown apache:apache /srv/httpd/htdocs/cppattack.html
	sudo cp gradienttable.css /srv/httpd/htdocs/includes
	sudo chown apache:apache /srv/httpd/htdocs/includes/gradienttable.css

clean:
	rm cppciphers.cgi && rm cppattack.cgi && rm *.o
