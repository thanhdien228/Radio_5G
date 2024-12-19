ACLOCAL_AMFLAGS = -I m4
lib_LTLIBRARIES = libDataBase.la
libDataBase_la_SOURCES = \
	src/commandLine.cc \
	src/dataValue.cc \
	src/dbException.cc \
	src/fileManager.cc \
	src/inMemDatabase.cc
libDataBase_la_LDFLAGS = -lreadline
AM_CPPFLAGS = \
	-I ./inc \
	-I /usr/include/readline
	