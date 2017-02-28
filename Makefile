PROJECT=httpevent
SRC=$(wildcard *.cpp src/*.cpp)
OBJ=$(patsubst %.cpp,%.o,$(SRC))
LUA=luajit
CC=g++
CXXFLAGS=-std=c++11 -O3 -Wall -I./include `pkg-config --cflags $(LUA) libevent_openssl openssl`
LDLIBS+=`pkg-config --libs $(LUA) libevent_openssl openssl` -lpthread
LDLIBS+=-lPocoUtil -lPocoCrypto -lPocoNet -lPocoFoundation


INSTALL_DIR=/usr/local/bin
CONFIG_DIR=/etc/httpevent
LN_DIR=/usr/bin
RUN_DIR=/var/httpevent

all:$(PROJECT)

$(PROJECT):$(OBJ)
	$(CC) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

demo:
	cd plugin && make
demo-clean:
	cd plugin && make clean
demo-install:
	cd plugin && make install


clean:
	rm -f $(PROJECT) $(OBJ)

install:
	mkdir -pv   $(INSTALL_DIR)\
		    $(CONFIG_DIR) \
		    $(RUN_DIR)    \
		    $(RUN_DIR)/log \
		    $(RUN_DIR)/tpl \
		    $(RUN_DIR)/mod \
		    $(RUN_DIR)/www \
		    $(RUN_DIR)/www/upload \
		    $(RUN_DIR)/www/assets  \
		    $(RUN_DIR)/cert \
		    $(RUN_DIR)/lua
	install $(PROJECT) $(INSTALL_DIR)
	cp -R  etc/* $(CONFIG_DIR)
	cp -R www/* $(RUN_DIR)/www
	cp -R www/assets/* $(RUN_DIR)/www/assets
	install script/*.sh $(LN_DIR)
	cp script/httpevent.service /etc/systemd/system
	cp cert/* $(RUN_DIR)/cert
	ln -fs $(INSTALL_DIR)/$(PROJECT) $(LN_DIR)
	cp include/httpevent.hpp /usr/local/include
	mkdir -p /usr/local/include/httpevent
	cp -R include/httpevent/*  /usr/local/include/httpevent
	
	
uninstall:
	