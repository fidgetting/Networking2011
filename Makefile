

CXX = g++
CFLAGS = -O3 -Wall -std=c++0x
INCPATH = -I.
DEF = 
EXE = proj1_client proj1_server

OBJS = socket.o  \
       service.o \

HEAD = socket.hpp  \
       service.hpp \
       common.hpp  \


all: $(EXE)

proj1_client: proj1_client.cpp $(OBJS) $(HEAD)
	$(CXX) $< -o $@ $(OBJS) $(CFLAGS) $(INCPATH)

proj1_server: proj1_server.cpp $(OBJS) $(HEAD)
	$(CXX) $< -o $@ $(OBJS) $(CFLAGS) $(INCPATH)

$(OBJS): %.o: %.cpp $(HEAD)
	$(CXX) -c $(INCPATH) $(CFLAGS)  $(DEF) -o $@ $< 

clean:
	rm -f $(EXE) *.o


