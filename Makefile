WARNINGS = -Wall -Weffc++ -Wextra -Wsign-conversion -pedantic-errors
DEBUG = -ggdb -fno-omit-frame-pointer
OPTIMIZE = -O2
STANDARD = -std=c++2a
LIBS = `pkg-config --libs gio-2.0`
INCLUDES = `pkg-config --cflags gio-2.0`

SOURCES = main.cpp display-config-API.cpp timingFunctions.cpp
OBJECTS = $(SOURCES:.cpp=.o)

TARGET = TopbarDisplaySettings

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(WARNINGS) $(OPTIMIZE) $(STANDARD) $(OBJECTS) $(LIBS) -o $@

%.o: %.cpp
	$(CXX) $(WARNINGS) $(OPTIMIZE) $(STANDARD) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

install:
	echo "Installing is not supported"

run:
	./$(TARGET)

debug: CXXFLAGS += $(DEBUG)
debug: OPTIMIZE = -O0
debug: clean $(TARGET)
