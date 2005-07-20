CXX=g++
NOCLEANSCXXFLAGS= -pg -g3 -Wall -Wunused \
	-fno-default-inline -fmessage-length=78
CXXFLAGS= -pedantic $(NOCLEANSCXXFLAGS)

LIBS= -lwsock32 -lcppunit -lgdi32
TARGET= UnitTest
OBJS= TestMain.o \
	RSATest.o \
# 	MPITest.o \
# 	AlgebraTheoryTest.o \
# 	NumberTheoryTest.o \
# 	AsymmetricKeyTest.o \
# 	SmartPointerTest.o \
# 	RationalTest.o \
# 	FontGrapherTest.o \
# 	TranscodeTest.o \
# 	XMLParserTest.o \
# 	GeometryTest.o \
# 	GraphicsTest.o \
# 	BitmapTest.o \
# 	EndianTest.o CBCTest.o \
#	AESTest.o MD5Test.o SHA1Test.o ExifTest.o \
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
	$(CXX) $(CXXFLAGS) -c $<

MPITest.o: MPITest.cpp ../math/MPI.hpp
	$(CXX) $(CXXFLAGS) -c $<

NumberTheoryTest.o: NumberTheoryTest.cpp ../math/MPI.hpp ../math/theory/NumberTheory.hpp
	$(CXX) $(CXXFLAGS) -c $<

$(TARGET): $(OBJS) Makefile
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) *.exe
