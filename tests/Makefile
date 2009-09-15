CPPFLAGS=-I..
LDFLAGS=-lboost_thread-gcc41-mt -lboost_unit_test_framework-gcc41-mt -ljpeg -lpthread

ifneq ("$(BOOST_PATH)","")
	CPPFLAGS+=-I$(BOOST_PATH)/include/boost
	LDFLAGS+=-L$(BOOST_PATH)/lib
endif

all: gil_threaded

gil_threaded: gil_threaded.o

clean:
	rm *.o gil_threaded
