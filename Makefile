CXXFLAGS =	-O0 -g -Wall -fmessage-length=0 -I/home/ymg/libevent/include -I/home/ymg/apache-log4cxx-0.10.0/include

OBJS =		RRMode.o master_thread.o worker_threads.o init_configure.o 

LIBS = -L/home/ymg/libevent/lib -levent  -L/home/ymg/apache-log4cxx-0.10.0/lib -llog4cxx

TARGET =	RRMode

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) -lpthread -lboost_system -lboost_filesystem -Wl,-rpath,/usr/local/lib

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
