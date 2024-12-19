bin_PROGRAMS = serverMain
serverMain_SOURCES = serverMain.cc src/server.cc src/carrier.cc src/modulator.cc ../antenna/src/antenna.cc 
AM_CPPFLAGS = \
	-I ./inc \
	-I /usr/include/readline \
	-I ../database/inc \
	-I ../logging/inc \
	-I ../visualizer/inc \
	-I ../antenna/inc
serverMain_LDADD = ../database/.libs/libDataBase.so ../logging/.libs/libLogger.so