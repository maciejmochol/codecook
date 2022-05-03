CC=g++
CFLAGS=-g -Wall -D_REENTRANT -rdynamic
LIBPTHREAD=-lpthread
//LIBXML=-lxml -lz 


CC_OBJ= \
./src/al_log.o \
./src/alarmserv.o \
./src/blocking.o \
./src/cdtree.o \
./src/codecook.o \
./src/command.o \
./src/cstable.o \
./src/datastruct.o \
./src/database.o\
./src/cddb.o\
./src/berkeley_db.o\
./src/device.o \
./src/device_list.o \
./src/itc.o \
./src/logger.o \
./src/manager.o \
./src/meaner.o \
./src/netmanager.o \
./src/protocol.o \
./src/ptable.o \
./src/terminal.o \
./src/thread.o \
./src/user.o \
./src/xmlgen.o \
./src/grammar_cdtree.tab.o \
./src/scanner_cdtree.o \
./src/parser_cdtree.o \
./src/grammar_mathforms.tab.o \
./src/scanner_mathforms.o \
./src/parser_mathforms.o 

#./src/system_info.o \
#./src/driver_1.o

# MYLIB =  ../../../libSzarp/ipcVooyLib.o
MYLIB = -ldl -lcrypt

all: 
	@$(MAKE) -C ./src
	@$(MAKE) -C ./plugins
	@echo Linking..

	$(CC) $(CFLAGS) -o codecook $(CC_OBJ) $(LIBPTHREAD) $(MYLIB);
	@echo "Making plugins.."
	g++ -shared -Wl,-soname,libCodecook_SYSINFO.so -o libCodecook_SYSINFO.so ./plugins/system_info.o
	#strip libCodecook_SYSINFO.so
	g++ -shared -Wl,-soname,libCodecook_PRATERM.so -o libCodecook_PRATERM.so ./plugins/praterm.o


codecook: 
	@$(MAKE) -C ./src
	@$(MAKE) -C ./plugins

clean:
	@$(MAKE) -C ./src $@
	@$(MAKE) -C ./plugins $@
	rm -f codecook *.so
