LIB=./SQLAPI/lib
INC=./SQLAPI/include
SRC=./src
BIN=./bin
OBJ=./obj

OPT=-Wall -g -std=c++11

all: clean $(BIN)/main_exe

$(BIN)/main_exe :
	g++ $(SRC)/main.cpp -lsqlapi -Wl,--no-as-needed -ldl -o $(BIN)/main_exe -I$(INC) -L$(LIB) 

clean:
	rm -rf $(BIN)/*