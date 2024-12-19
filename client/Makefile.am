bin_PROGRAMS = clientMain
clientMain_SOURCES = clientMain.cc src/client.cc
AM_CPPFLAGS = \
	-I ./inc \
	-I /usr/include/readline \
	-I ../database/inc \
	-I ../logging/inc
clientMain_LDADD = ../database/.libs/libDataBase.so ../logging/.libs/libLogger.so 