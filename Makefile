CC = gcc
CFLAGS = -Wall -O2
LIBS = -lbcm2835 -lm

TARGET = galaga
OBJS = main.o st7789.o input.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

main.o: main.c st7789.h input.h
	$(CC) $(CFLAGS) -c main.c

st7789.o: st7789.c st7789.h
	$(CC) $(CFLAGS) -c st7789.c

input.o: input.c input.h
	$(CC) $(CFLAGS) -c input.c

clean:
	rm -f $(OBJS) $(TARGET)