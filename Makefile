CC=cl
SRC=asciivid.cpp
TARGET=asciivid
INCPATH=.\include
LIBPATH=.\x64\vc16\lib

all: $(TARGET)

$(TARGET): $(SRC)	
	$(CC) /EHsc /I$(INCPATH) $(SRC) /link /LIBPATH:$(LIBPATH) opencv_world4100.lib