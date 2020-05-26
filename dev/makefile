.PHONY: run all clean test print

COMPILER=gcc
SRC=./src
OBJ=./obj
OBJS=$(addprefix $(OBJ)/,$(patsubst %.c,%.o,$(notdir $(wildcard *.c))))

run: ./test
	./test

./test: $(OBJS) 
	$(COMPILER) -o $@ $^

$(OBJ)/%.o: %.c
	$(COMPILER) -c -o $@ $<

clean:
	rm $(OBJ)/*

print:
	echo $(OBJS)
