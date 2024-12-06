# Compiler
CC = gcc

# Compiler flags
CFLAGS = -g -Wall

# Source files
TCP_CLIENT = tcpclient.c
TCP_SERVER = tcpserver.c
UDP_CLIENT = udpclient.c
UDP_SERVER = udpserver.c

# Executable names
TCP_CLIENT_EXEC = tcpclient
TCP_SERVER_EXEC = tcpserver
UDP_CLIENT_EXEC = udpclient
UDP_SERVER_EXEC = udpserver

# Default target
all: $(TCP_CLIENT_EXEC) $(TCP_SERVER_EXEC) $(UDP_CLIENT_EXEC) $(UDP_SERVER_EXEC)

# Rules to build executables
$(TCP_CLIENT_EXEC): $(TCP_CLIENT)
	$(CC) $(CFLAGS) $< -o $@

$(TCP_SERVER_EXEC): $(TCP_SERVER)
	$(CC) $(CFLAGS) $< -o $@

$(UDP_CLIENT_EXEC): $(UDP_CLIENT)
	$(CC) $(CFLAGS) $< -o $@

$(UDP_SERVER_EXEC): $(UDP_SERVER)
	$(CC) $(CFLAGS) $< -o $@

# Clean up build files
clean:
	rm -f $(TCP_CLIENT_EXEC) $(TCP_SERVER_EXEC) $(UDP_CLIENT_EXEC) $(UDP_SERVER_EXEC)

.PHONY: all clean