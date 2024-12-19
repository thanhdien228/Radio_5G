check_PROGRAMS = mainDataValueTest mainDbExceptionTest mainFileManagerTest mainInMemDBTest
TESTS = mainDataValueTest mainDbExceptionTest mainFileManagerTest mainInMemDBTest
mainDbExceptionTest_SOURCES = \
	../src/dbException.cc \
	dbExceptionTest/mainDbExceptionTest.cc
mainDataValueTest_SOURCES = \
	../src/dataValue.cc \
	../src/dbException.cc \
	../ dataValueTest/mainDataValueTest.cc
mainFileManagerTest_SOURCES = \
	../src/fileManager.cc \
	../src/dataValue.cc \
	../src/dbException.cc \
	fileManagerTest/mainFileManagerTest.cc
mainInMemDBTest_SOURCES = \
	../src/fileManager.cc \
	../src/dataValue.cc \
	../src/dbException.cc \
	../src/inMemDatabase.cc \
	inMemDatabaseTest/mainInMemDBTest.cc
AM_CPPFLAGS = \
	-I ../inc 
mainDataValueTest_LDADD = -lgtest -lgtest_main
mainDbExceptionTest_LDADD = -lgtest -lgtest_main
mainFileManagerTest_LDADD = -lgtest -lgtest_main
mainInMemDBTest_LDADD = -lgtest -lgtest_main