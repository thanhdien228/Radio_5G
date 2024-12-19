check_PROGRAMS = mainAntennaTest
TESTS = mainAntennaTest
mainAntennaTest_SOURCES = \
	../src/antenna.cc \
	mainAntennaTest.cc
AM_CPPFLAGS = \
	-I ../inc \
	-I ../../server/inc \
	-I ../../logging/inc \
	-I ../../database/inc 
mainAntennaTest_LDADD = \
	-lgtest -lgtest_main \
	../../database/.libs/libDataBase.so \
	../../logging/.libs/libLogger.so