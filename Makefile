CC ?= gcc
CFLAGS = -g -Wall -Wno-deprecated
OS = $(shell uname)

ifeq ($(OS), Darwin)
	LIBS = -framework OpenGL -framework GLUT
else ifeq ($(OS), Linux)
	LIBS = -lglut -lGLU -lGL
else
	CC=x86_64-w64-mingw32-g++
	LIBS = -lglut32 -lglu32 -lopengl32
endif

TARGET = tetris
SRC = tetris.c
OBJS = $(SRC:.c=.o)

all: $(TARGET)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
