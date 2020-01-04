DEBUG = true
CC = gcc

CFLAGS = -g -O2 -c

# Header files
HEADER = handling.h

# Executables
OUTSERVER = remoteServer
OUTCLIENT = remoteClient 

# Source files
SOURCE_CLIENT = remoteClient.c 
SOURCE_SERVER = remoteServer.c

SOURCE = $(SOURCE_CLIENT) $(SOURCE_SERVER)

# Object files
OBJS_CLIENT = $(SOURCE_CLIENT:.c=.o)
OBJS_SERVER = $(SOURCE_SERVER:.c=.o)
OBJS = $(OBJS_CLIENT) $(OBJS_SERVER)


# Help menu
USAGE = Usage: make [OPTION]
DESCRIPTION = 
HELPMENU = $(USAGE)\n$(DESCRIPTION)\nOPTIONS:


# ----------------------------------------------------
# 					Basic options

all: client server ## Create both Server and Client executables
	
client: $(OBJS_CLIENT)	## Create only client's executable
	$(CC) -o $(OUTCLIENT) $?

server: $(OBJS_SERVER)	## Create only server's executable
	$(CC) -o $(OUTSERVER) $?

clean:  ## Clean Object files and executables
	$(RM) $(OBJS) $(OUTSERVER) $(OUTCLIENT)

help:	## Print this help message and exit
	@echo "$(HELPMENU)" ; grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "   %-30s %s\n", $$1, $$2}'

count:	## Count number of lines
	wc -l $(SOURCE) $(HEADER)

rc: ## Run Client for testing, giving predefined parameters
	@./remoteClient server 1 5 infile

rs: ## Run Server for testing, giving predefined parameters
	@./remoteServer 1 5


# ----------------------------------------------------
#					Object files

remoteServer.o: remoteServer.c
	$(CC) $(CFLAGS) $?

remoteClient.o: remoteClient.c
	$(CC) $(CFLAGS) $?