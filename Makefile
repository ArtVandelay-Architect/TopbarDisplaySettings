all: TopbarDisplaySettings


WARNINGS = -Wall -Weffc++ -Wextra -Wsign-conversion -pedantic-errors
DEBUG = -ggdb -fno-omit-frame-pointer
OPTIMIZE = -O2
STANDARD = -std=c++2a
LIBS = `pkg-config --libs gio-2.0`
INCLUDES = `pkg-config --cflags gio-2.0`

SOURCES = main.cpp display-config-API.cpp
OBJECTS = $(SOURCES:.cpp=.o)

TopbarDisplaySettings: $(OBJECTS)
	$(CXX) $(WARNINGS) $(DEBUG) $(OPTIMIZE) $(STANDARD) $(OBJECTS) $(LIBS)  -o $@ 

%.o: %.cpp
	$(CXX) $(WARNINGS) $(DEBUG) $(OPTIMIZE) $(STANDARD) $(INCLUDES) -c $< -o $@

clean:
	rm -f TopbarDisplaySettings $(OBJECTS)

# Builder will call this to install the application before running.
install:
	echo "Installing is not supported"

# Builder uses this target to run your application.
run:
	./TopbarDisplaySettings
