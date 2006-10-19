CXX=C:/MinGW/bin/g++ 
CXXFLAGS= -g -Wall -Wno-sign-compare -Wno-unused -Wno-uninitialized \
	-fno-default-inline -fmessage-length=78 -Ic:/mingw/include/stlport -I..
#CXXFLAGS= -pedantic $(NOCLEANSCXXFLAGS)

LIBS= -lwsock32 -lgdi32 -LC:/MinGW/lib -lcppunit -lstlport_debug
TARGET= UnitTest
OBJS= TestMain.o \
 	ScannerTest.o \
 	LexicalCastTest.o \
# 	XMLParserTest.o \
# 	HTTPResultTest.o \
# 	URLTest.o \
# 	HTTPPropertyTest.o \
# 	PropertyTest.o \
# 	StateMachineTest.o \
# 	FSMTest.o \
# 	Base64Test.o \
# 	StackableScannerTest.o \
# 	RegexTest.o \
# 	SmartPointerTest.o \
# 	RationalTest.o \
# 	FontGrapherTest.o \
# 	TranscodeTest.o \
# 	GeometryTest.o \
# 	GraphicsTest.o \
# 	BitmapTest.o \
# 	EndianTest.o \
# 	ThreadTest.o \
# 	ThreadPoolTest.o \
# 	ImageTest.o SamplingScannerTest.o \
# 	BitStreamTest.o HuffmanTest.o MemoryStreamTest.o \
# 	NotifyTest.o RangeTest.o SingletonTest.o \
# 	SingletonTestFunc1.o SingletonTestFunc2.o \
# 	LZSSTest.o \
# 	EventTest.o \
# 	JulianDateTest.o \
# 	CBCTest.o \
# 	AsymmetricKeyTest.o \
# 	SieveTest.o \
# 	RSATest.o \
# 	AlgebraTheoryTest.o \
# 	NumberTheoryTest.o \
# 	MPITest.o \
# 	AESTest.o MD5Test.o SHA1Test.o ExifTest.o \
# 	IPTest.o \
#	ShiftJISTest.o

DEPSOBJS= TestMain.d \
	ShiftJISTest.d \
	HTTPResultTest.d \
	URLTest.d \
	LexicalCastTest.d \
	HTTPPropertyTest.d \
	PropertyTest.d \
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


%.d: %.cpp
	$(CXX) -mno-cygwin -I.. -MM -MT $(<:.cpp=.o) $< >> makefile.depend

clean:
	rm -f $(OBJS) *.exe

cleandepend:
	rm -f makefile.depend

-include makefile.dpend
