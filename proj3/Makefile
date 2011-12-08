

CXX = g++
CFLAGS = -O3 -Wall -std=c++0x -g
INCPATH = -I.
DEF = 
EXE = myresolver

OBJS = service.o  \
       socket.o \

HEAD = socket.hpp  \
       service.hpp \
       common.hpp  \


all: $(EXE)

myresolver: proj3_myresolver.cpp $(OBJS) $(HEAD)
	$(CXX) $< -o $@ $(OBJS) $(CFLAGS) $(INCPATH)

$(OBJS): %.o: %.cpp $(HEAD)
	$(CXX) -c $(INCPATH) $(CFLAGS)  $(DEF) -o $@ $< 

clean:
	rm -f $(EXE) *.o


