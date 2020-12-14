.PHONY: run all clean test print

EXEC=els
COMPILER=gcc
SRC=./src
OBJ=./obj
OBJS=$(addprefix $(OBJ)/,$(patsubst %.c,%.o,$(notdir $(wildcard src/*.c))))
INCFLAG=-I./inc

run: ./$(EXEC)
	./$(EXEC)

./$(EXEC): $(OBJS) 
	$(COMPILER) $(INCFLAG) -o $@ $^

$(OBJ)/%.o: $(SRC)/%.c
	$(COMPILER) $(INCFLAG) -c -o $@ $<

clean:
	rm $(OBJ)/*

print:
	echo $(OBJS)
