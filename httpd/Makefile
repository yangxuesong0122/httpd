ROOT_PATH=$(shell pwd)
LDFLAGS=-lpthread#-static
FLAGS=#-D_DEBUG_#-g
CC=gcc
BIN=httpd

CGI_BIN=cgi_bin

SRC=$(shell ls *.c)
OBJ=$(SRC:.c=.o)

$(BIN):$(OBJ)
	@echo "Linking [$^] to [$@]"
	@$(CC) -g -o $@ $^ $(LDFLAGS) # -levent
	@echo "Linking done..."
%.o:%.c
	@echo "Compling [$<] to [$@]"
	@$(CC) -g -c $< $(FLAGS) 
	@echo "Compling done..."

.PHONY:clean
clean:
	rm -f *.o $(BIN)
