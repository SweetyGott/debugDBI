
SlottedPage: slottedTest.o BufferManager.o BufferFrame.o SlottedPage.o SPSegment.o Tid.o Record.o
	g++ -o SlottedPage slottedTest.o BufferManager.o BufferFrame.o SlottedPage.o SPSegment.o Tid.o Record.o

slottedTest.o: slottedTest.cpp
	g++ -c -std=c++11 slottedTest.cpp

BufferManager.o: BufferManager.cpp BufferFrame.hpp
	g++ -c -std=c++11 BufferManager.cpp

BufferFrame.o: BufferFrame.cpp
	g++ -c -std=c++11 BufferFrame.cpp
	
SlottedPage.o: SlottedPage.hpp SlottedPage.cpp
	g++ -c -std=c++11 SlottedPage.hpp SlottedPage.cpp
	
SPSegment.o: SPSegment.hpp SPSegment.cpp
	g++ -c -std=c++11 SPSegment.hpp SPSegment.cpp

Tid.o: Tid.hpp
	g++ -c -std=c++11 Tid.hpp

Record.o: Record.hpp Record.cpp
	g++ -c -std=c++11 Record.hpp Record.cpp
	
all:
	SlottedPage;

clean:
	rm -f slottedTest slottedTest.o



