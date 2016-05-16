
CXXFLAGS+= -std=c++11 -Wall

#SOURCES = $(wildcard *.cpp)
SOURCES = slottedTest.cpp BufferManager.cpp BufferFrame.cpp SlottedPage.cpp SPSegment.cpp Tid.cpp Record.cpp
HEADERS = $(wildcard *.hpp)
OBJECTS = $(SOURCES:.cpp=.o)


SlottedPage: $(OBJECTS)
	g++ -Wall $(OBJECTS) -o SlottedPage

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

all:
	SlottedPage;

clean:
	rm -f slottedTest *.o