
BIN_DIR = ../bin
INC_DIR = .

CXX     = g++
CFLAGS  = -g -O3 -std=c++0x
INCPATH = -I$(INC_DIR)
EXE     = ../server
LIBS    = -lboost_system -lboost_program_options -lcryptopp

OBJS    = $(BIN_DIR)/main.o       \
          $(BIN_DIR)/Socket.o     \
          $(BIN_DIR)/threadpool.o \

HEAD    = Makefile                  \
          $(INC_DIR)/Socket.hpp     \
          $(INC_DIR)/Server.hpp     \
          $(INC_DIR)/threadpool.hpp \

all: $(EXE)
$(EXE): $(OBJS) $(BIN_DIR)/*.o $(HEAD)
	$(CXX) -o $(EXE) $(OBJS) $(LIBS)

$(OBJS): $(BIN_DIR)/%.o: %.cpp $(HEAD)
	@echo "    build" $<
	$(CXX) -c $(DEF) $(INCPATH) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXE) $(OBJS)


