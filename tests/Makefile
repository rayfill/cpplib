INCLUDE_PATH += -Ic:/mingw/include/stlport
LIBS += -lcppunit

CXXFLAGS += -g -Wall -W -Wsign-compare -Wunused -Weffc++ \
	-fno-default-inline $(CXXOPTION) $(INCLUDE_PATH) -I..
#CXXFLAGS= -pedantic $(NOCLEANSCXXFLAGS)

TARGET= UnitTest

OBJS= TestMain.o \
	TwinLockTest.o \
	RerunnableThreadTest.o \
	ThreadTest.o \
	MutexTest.o \
	CommandLineTest.o \
	ScannerTest.o \
	LexicalCastTest.o \
	HTTPResultTest.o \
	URLTest.o \
	HTTPPropertyTest.o \
	PropertyTest.o \
	StateMachineTest.o \
	FSMTest.o \
	Base64Test.o \
	StackableScannerTest.o \
	RegexTest.o \
	SmartPointerTest.o \
	RationalTest.o \
	TranscodeTest.o \
	GeometryTest.o \
	EndianTest.o \
	BitStreamTest.o HuffmanTest.o MemoryStreamTest.o \
	NotifyTest.o RangeTest.o \
	LZSSTest.o \
	CBCTest.o \
	AESTest.o MD5Test.o SHA1Test.o \
	IPTest.o \
	XMLParserTest.o \
	NumberTheoryTest.o \
	AlgebraTheoryTest.o \
	SieveTest.o \
	MPITest.o \
	AsymmetricKeyTest.o \
	RSATest.o \
	JulianDateTest.o \
	SingletonTest.o \
	SingletonTestFunc1.o SingletonTestFunc2.o \
#	ShiftJISTest.o
#	ExifTest.o \
#	SamplingScannerTest.o \
#	ImageTest.o \
#	ThreadPoolTest.o \
#	GraphicsTest.o \
#	FontGrapherTest.o \
#	BitmapTest.o \
#	EventTest.o \

DEPENDFILES=$(OBJS:.o=.d) 

.PHONY: clean %.d makedepend cleandepend remake

all: $(TARGET)

makedepend: cleandepend $(DEPENDFILES)

test: all
	./UnitTest

document: ../Doxyfile
	cd .. && doxygen

$(TARGET): $(OBJS) Makefile
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS) 


%.d: %.cpp
	$(CXX)  -I.. -MM -MT $(<:.cpp=.o) $< >> makefile.depend

clean:
	rm -f $(OBJS) *.exe

cleandepend:
	rm -f makefile.depend

remake: cleandepend makedepend

-include makefile.dpend
