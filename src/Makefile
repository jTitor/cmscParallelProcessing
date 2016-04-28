CC = g++
EXE = Proj6
CPP_FILES := $(shell find . -name '*.cpp')
#$(wildcard *.cpp)
OBJDIR = obj
OBJ_FILES = $(CPP_FILES:%.cpp=%.o)#$(CPP_FILES:.cpp=.o)
CC_FLAGS = -w --std=c++0x -L./Libraries -lstdc++ -lm -lpthread -lrt #GL seems to have a very old version of libpng
OBJ_FLAGS = $(CC_FLAGS)

all: $(EXE)

$(EXE): $(OBJ_FILES)
	$(CC) $(CC_FLAGS) $(OBJ_FILES) -o $(EXE)

%.o: %.cpp
	$(CC) -c $(OBJ_FLAGS) $< -o $@

clean:
	rm -f $(OBJ_FILES) $(EXE)
