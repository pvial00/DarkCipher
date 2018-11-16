CC=gcc
CFLAGS=-O3 -std=c99
TARGET=dark
all: $(TARGET)

$(TARGET): darkcrypt.c
	$(CC) darkcrypt.c $(CFLAGS) -o $(TARGET)

clean:
	$(RM) $(TARGET)
