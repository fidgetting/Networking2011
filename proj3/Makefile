

CXX = clang
LINK = g++
CFLAGS = -O3 -Wall -g
INCPATH = -I.
DEF = -DDEBUG
EXE = myresolver

OBJS = myresolver.o \

HEAD = common.hpp  \
       Makefile \


all: $(EXE)
$(EXE): $(OBJS) $(HEAD)
	$(LINK) -o $(EXE) $(OBJS) $(DEF) $(CFLAGS) $(INCPATH)

$(OBJS): %.o: %.cpp $(HEAD)
	$(CXX) -c $< $(DEF) $(CFLAGS) $(INCPATH) -o $@

clean:
	rm -f $(EXE) *.o


