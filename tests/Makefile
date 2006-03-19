CXX=/usr/bin/g++ 
NOCLEANSCXXFLAGS= -g -mno-cygwin -Wall -Wunused \
	-fno-default-inline -fmessage-length=78 -I..
CXXFLAGS= -pedantic $(NOCLEANSCXXFLAGS)

LIBS= -lwsock32 -lcppunit -lgdi32
TARGET= UnitTest
OBJS= TestMain.o \
	ScannerTest.o \
	SieveTest.o \
	LZSSTest.o \
	StateMachineTest.o \
	FSMTest.o \
	Base64Test.o \
# 	AlgebraTheoryTest.o \
# 	NumberTheoryTest.o \
# 	AsymmetricKeyTest.o \
# 	StackableScannerTest.o \
# 	RegexTest.o \
# 	RSATest.o \
# 	MPITest.o \
# 	SmartPointerTest.o \
# 	RationalTest.o \
# 	FontGrapherTest.o \
# 	TranscodeTest.o \
# 	XMLParserTest.o \
# 	GeometryTest.o \
# 	GraphicsTest.o \
# 	BitmapTest.o \
# 	EndianTest.o CBCTest.o \
# 	AESTest.o MD5Test.o SHA1Test.o ExifTest.o \
# 	ImageTest.o SamplingScannerTest.o \
# 	BitStreamTest.o HuffmanTest.o MemoryStreamTest.o \
# 	IPTest.o NotifyTest.o RangeTest.o SingletonTest.o \
# 	SingletonTestFunc1.o SingletonTestFunc2.o ThreadTest.o EventTest.o \
# 	JulianDateTest.o 

all: $(TARGET)

test: all
	./UnitTest

EndianTest.o: EndianTest.cpp ../IO/Endian.hpp
	$(CXX) $(NOCLEANSCXXFLAGS) -c $<

RSATest.o: RSATest.cpp ../Cryptography/RSA.hpp ../math/Theory/NumberTheory.hpp
	$(CXX) $(NOCLEANSCXXFLAGS) -c $<

SHA1Test.o: SHA1Test.cpp
	$(CXX) $(NOCLEANSCXXFLAGS) -c $<

MD5Test.o: MD5Test.cpp
	$(CXX) $(NOCLEANSCXXFLAGS) -c $<

ExifTest.o: ExifTest.cpp
	$(CXX) $(NOCLEANSCXXFLAGS) -c $<

SieveTest.o: SieveTest.cpp
	$(CXX) $(NOCLEANSCXXFLAGS) -c $<

MPITest.o: MPITest.cpp
	$(CXX) $(NOCLEANSCXXFLAGS) -c $<

AlgebraTheoryTest.o: AlgebraTheoryTest.cpp
	$(CXX) $(NOCLEANSCXXFLAGS) -c $<

NumberTheoryTest.o: NumberTheoryTest.cpp
	$(CXX) $(NOCLEANSCXXFLAGS) -c $<

AsymmetricKeyTest.o: AsymmetricKeyTest.cpp
	$(CXX) $(NOCLEANSCXXFLAGS) -c $<

$(TARGET): $(OBJS) Makefile
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS) 

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) *.exe
