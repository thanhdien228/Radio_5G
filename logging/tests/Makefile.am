check_PROGRAMS = mainLogExceptionTest mainLoggerTest
mainLogExceptionTest_SOURCES = \
	../src/logException.cc \
	logExceptionTest/mainLogExceptionTest.cc
mainLoggerTest_SOURCES = \
	../src/logger.cc \
	../src/logException.cc \
	loggerTest/mainLoggerTest.cc
AM_CPPFLAGS = \
	-I ../inc
mainLoggerTest_LDADD = -lgtest -lgtest_main
mainLogExceptionTest_LDADD = -lgtest -lgtest_main
TESTS = mainLogExceptionTest mainLoggerTest