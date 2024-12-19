check_PROGRAMS = mainCarrier mainModulator
TESTS = mainCarrier mainModulator
mainCarrier_SOURCES = \
	../src/carrier.cc \
	carrierTest/mainCarrier.cc
mainModulator_SOURCES = \
	../src/modulator.cc \
	modulatorTest/mainModulator.cc
AM_CPPFLAGS = \
	-I ../inc/ \
	-I /usr/include/readline \
	-I ../../database/inc \
	-I ../../logging/inc 
mainCarrier_LDADD = \
	-lgtest \
	-lgtest_main \
	-lpthread \
	../../database/.libs/libDataBase.so \
	../../logging/.libs/libLogger.so
mainModulator_LDADD = \
	-lgtest \
	-lgtest_main \
	-lpthread \
	-lgmock \
	-lgmock_main \
	../../database/.libs/libDataBase.so \
	../../logging/.libs/libLogger.so