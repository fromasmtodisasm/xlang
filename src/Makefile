INC=-I./include/
SRC=main.c interpreter.c exp.c lexer.c
OBJ=main.o interpreter.o exp.o lexer.o
PROGRAM=xlang
CC=/usr/bin/gcc

all:  $(PROGRAM)

$(PROGRAM): $(OBJ)
	@echo Linking $(EXECUTABLE)...
	$(CC) $(LDLIBS) $(INC) $(OBJ) -o $(PROGRAM)
	
$(OBJ): ./%.o : ./%.c
	@echo Building $@ from $<...
	$(CC) $(INC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o
