DEBUG = true
CC = gcc

CFLAGS = -g -O2 -c

HEADER = handling.h

OUTSERVER = remoteServer
OUTCLIENT = remoteClient 

SOURCE_CLIENT = remoteClient.c 
SOURCE_SERVER = remoteServer.c

SOURCE = $(SOURCE_CLIENT) $(SOURCE_SERVER)

OBJS_CLIENT = $(SOURCE_CLIENT:.c=.o)
OBJS_SERVER = $(SOURCE_SERVER:.c=.o)
OBJS = $(OBJS_CLIENT) $(OBJS_SERVER)


USAGE = Usage: make [OPTION]
DESCRIPTION = 
HELPMENU = $(USAGE)\n$(DESCRIPTION)\nOPTIONS:


## ----------------------------------------------------
##
## 					Basic options
## 
## ----------------------------------------------------

all: client server ## Create Server and Client executables
	
client: $(OBJS_CLIENT)	## Create only client's executable
	$(CC) -o $(OUTCLIENT) $?

server: $(OBJS_SERVER)	## Create only server's executable
	$(CC) -o $(OUTSERVER) $?

clean:  ## Clean Object files and executables
	$(RM) $(OBJS) $(OUTSERVER) $(OUTCLIENT)

help:	## Print help message and exit
	@echo "$(HELPMENU)" ; grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "   %-30s %s\n", $$1, $$2}'

count:	## Count number of lines
	wc $(SOURCE) $(HEADER)



## ----------------------------------------------------
## 
##					Object files
##
## ----------------------------------------------------



remoteServer.o: remoteServer.c
	$(CC) $(CFLAGS) $?

remoteClient.o: remoteClient.c
	$(CC) $(CFLAGS) $?