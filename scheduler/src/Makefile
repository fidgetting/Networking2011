
BIN_DIR = ../bin
INC_DIR = ../include

CXX     = g++
CFLAGS  = -g -O3 -std=c++0x
INCPATH = -I$(INC_DIR)
EXE     = ../fo_scheduler
LIBS    = -lboost_program_options -lboost_filesystem -lboost_system

OBJS = $(BIN_DIR)/main.o        \
       $(BIN_DIR)/Scheduler.o   \
       $(BIN_DIR)/Logging.o     \
       $(BIN_DIR)/Event.o       \

HEAD = Makefile                 \
       $(INC_DIR)/Scheduler.hpp \
       $(INC_DIR)/Agent.hpp     \
       $(INC_DIR)/Logging.hpp   \
       $(INC_DIR)/Event.hpp     \

all: $(BIN_DIR) $(EXE)
$(EXE): $(OBJS) $(BIN_DIR)/*.o $(HEAD)
	$(CXX) -o $(EXE) $(OBJS) $(LIBS)

$(BIN_DIR):
	mkdir $(BIN_DIR)

$(OBJS): $(BIN_DIR)/%.o: %.cpp $(HEAD)
	@echo "    build" $<
	$(CXX) -c $(DEF) $(INCPATH) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXE) $(OBJS)

