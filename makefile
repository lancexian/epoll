CC  = gcc
CXX = g++

CXXFLAGS = -Wall -O2 -g -std=c++11 	#编译器参数
CXXFLAGS += -I $(shell pwd)/include	#指定编译器头文件

LIB = -lpthread -L.

PRG = libtools.so
OBJ = main.o

EXE = demo
ALL:$(EXE)

$(EXE):main.o
	$(CXX) $(CFLAGS) $< -o $@

%.o:%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $^ -o $@

$(PRG):$(OBJ)
	$(CXX) -shared -o $@ $(OBJ) $(LIB)

.PHONY:clean
clean:
	@echo "Remove linked and compiled files....."
	rm -rf $(OBJ) $(EXE)
