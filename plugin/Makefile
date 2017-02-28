MOD=mod/demo.so

MODSRC=$(wildcard *.cpp)
MODOBJ=$(patsubst %.cpp,%.o,$(MODSRC))

LUA=luajit

CC=g++
CXXFLAGS+=-O3 -std=c++11 -fPIC -Wall  `pkg-config --cflags $(LUA) libevent_openssl openssl`
LDLIBS+=`pkg-config --libs $(LUA) libevent_openssl openssl`  -lpthread
LDLIBS+=-lPocoUtil -lPocoCrypto -lPocoNet -lPocoFoundation
LDFLAGS+=-shared



all:$(MOD)

$(MOD):$(MODOBJ)
	$(CC) -o $@ $^ $(CXXFLAGS) $(LDLIBS) $(LDFLAGS)

clean:
	rm -f  $(MODOBJ) $(MOD)


install:
	systemctl stop httpevent
	install etc/*.*  /etc/httpevent
	install mod/demo.so /var/httpevent/mod
	install  www/*.* /var/httpevent/www
	install  lua/*.* /var/httpevent/lua
	mkdir -pv /var/httpevent/lua/mod
	install  lua/mod/*.* /var/httpevent/lua/mod
	systemctl start httpevent

