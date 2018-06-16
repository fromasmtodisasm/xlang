INC=-I./include/
SRC=main.c interpreter.c exp.c lexer.c preprocessor.c fileutils.c
OBJ=main.o interpreter.o exp.o lexer.o preprocessor.o fileutils.o
PROGRAM=xlang
CC=/usr/bin/gcc
INSTALL_DIR=/usr/bin/

.PHONY: all install clean

all:  $(PROGRAM)

$(PROGRAM): $(OBJ)
	@echo Linking $(EXECUTABLE)...
	$(CC) $(LDLIBS) $(INC) $(OBJ) -o $(PROGRAM)
	
$(OBJ): ./%.o : ./%.c
	@echo Building $@ from $<...
	$(CC) $(INC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o
install:
	install $(PROGRAM) $(INSTALL_DIR)
