SRCS += myls.c
LIBNAME += lbmyls.so

all:
	gcc $(SRCS) -o $(LIBNAME) -shared -fPIC

install:
	cp $(LIBNAME) /usr/lib
	cp myls.h /usr/include

uninstall:
	rm -f /usr/lib/$(LIBNAME) /usr/include/myls.h

clean:
	rm -f $(LIBNAME) *.o