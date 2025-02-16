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

# Input parameters for debuging
DI = infile		# DEBUG INPUT FILE
DS = 8080 		# DEBUG SERVER PORT
DC = 8081		# DEBUG CLIENT PORT
DN = localhost	# DEBUG SERVER NAME
DK = 4			# DEBUG SERVER KIDS


# ----------------------------------------------------
# 					Basic options

all: client server ## Create both Server and Client executables
	
client: $(OBJS_CLIENT)	## Create only client's executable
	$(CC) -o $(OUTCLIENT) $?

server: $(OBJS_SERVER)	## Create only server's executable
	$(CC) -o $(OUTSERVER) $?

clean:  ## Clean Object files and executables
	$(RM) $(OBJS) $(OUTSERVER) $(OUTCLIENT)
	$(RM) output.*.*

help:	## Print this help message and exit
	@echo "$(HELPMENU)" ; grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "   %-30s %s\n", $$1, $$2}'

count:	## Count number of lines
	wc -l $(SOURCE) $(HEADER) $(MAKEFILE_LIST)


# ------------------------------------------------
#			DEBUG RULES
#
debug: kill clean all run ## Clean, Make and Run both Client and Server for testing, giving predefined parameters

rc: ## Run Client for testing, giving predefined parameters
	@./remoteClient $(DN) $(DS) $(DC) $(DI) 

rs: ## Run Server for testing, giving predefined parameters
	@./remoteServer $(DS) $(DK)

run: ## Run both Client and Server for testing, giving predefined parameters
	@./remoteServer $(DS) $(DK) & ./remoteClient $(DN) $(DS) $(DC) $(DI) 

kill:	## Kill all Client and Server Processes
	@killall remoteServer && echo "\033[32mServer terminated successfully!\033[39m" || echo "\033[31mServer was not running.\033[39m"
	@killall remoteClient && echo "\033[32mClient terminated successfully!\033[39m" || echo "\033[31mClient was not running.\033[39m"

# ----------------------------------------------------
#					Object files

remoteServer.o: remoteServer.c
	$(CC) $(CFLAGS) $?

remoteClient.o: remoteClient.c
	$(CC) $(CFLAGS) $?
	