CXX=/usr/bin/g++ 
CXXFLAGS= -g -mno-cygwin -O0 -Wall -Wunused \
	-fno-default-inline -fmessage-length=78 -I..
#CXXFLAGS= -pedantic $(NOCLEANSCXXFLAGS)

LIBS= -lwsock32 -lcppunit -lgdi32
TARGET= UnitTest
OBJS= TestMain.o \
	ScannerTest.o \
	SieveTest.o \
	LZSSTest.o \
	StateMachineTest.o \
	FSMTest.o \
	Base64Test.o \
	AlgebraTheoryTest.o \
	NumberTheoryTest.o \
	AsymmetricKeyTest.o \
	StackableScannerTest.o \
	RegexTest.o \
	RSATest.o \
	MPITest.o \
	SmartPointerTest.o \
	RationalTest.o \
	FontGrapherTest.o \
	TranscodeTest.o \
	XMLParserTest.o \
	GeometryTest.o \
	GraphicsTest.o \
	BitmapTest.o \
	EndianTest.o CBCTest.o \
	AESTest.o MD5Test.o SHA1Test.o ExifTest.o \
	ImageTest.o SamplingScannerTest.o \
	BitStreamTest.o HuffmanTest.o MemoryStreamTest.o \
	IPTest.o NotifyTest.o RangeTest.o SingletonTest.o \
	SingletonTestFunc1.o SingletonTestFunc2.o ThreadTest.o \
	JulianDateTest.o \
	EventTest.o \
	ThreadPoolTest.o \

DEPSOBJS= TestMain.d \
	ScannerTest.d \
	SieveTest.d \
	LZSSTest.d \
	ThreadPoolTest.d \
	StateMachineTest.d \
	FSMTest.d \
	Base64Test.d \
	AlgebraTheoryTest.d \
	NumberTheoryTest.d \
	AsymmetricKeyTest.d \
	StackableScannerTest.d \
	RegexTest.d \
	RSATest.d \
	MPITest.d \
	SmartPointerTest.d \
	RationalTest.d \
	FontGrapherTest.d \
	TranscodeTest.d \
	XMLParserTest.d \
	GeometryTest.d \
	GraphicsTest.d \
	BitmapTest.d \
	EndianTest.d CBCTest.d \
	AESTest.d MD5Test.d SHA1Test.d ExifTest.d \
	ImageTest.d SamplingScannerTest.d \
	BitStreamTest.d HuffmanTest.d MemoryStreamTest.d \
	IPTest.d NotifyTest.d RangeTest.d SingletonTest.d \
	SingletonTestFunc1.d SingletonTestFunc2.d ThreadTest.d EventTest.d \
	JulianDateTest.d 

.PHONY: clean %.d makedepend cleandepend

all: $(TARGET)

makedepend: cleandepend $(DEPSOBJS)

test: all
	./UnitTest

document: ../Doxyfile
	cd .. && doxygen

$(TARGET): $(OBJS) Makefile
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS) 

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

%.d: %.cpp
	$(CXX) -mno-cygwin -I.. -MM -MT $(<:.cpp=.o) $< >> makefile.depend

clean:
	rm -f $(OBJS) *.exe

cleandepend:
	rm -f makefile.depend

-include makefile.dpend
