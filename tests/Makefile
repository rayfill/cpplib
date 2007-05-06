# リンクに必要なライブラリがwindowsとposixで異なるため、
# 環境変数から参照しています。
# 環境変数LIBSに必要なlibraryを追加してください。
# posixの場合特に必要ありません。windowsの場合は LIBS=-lws2_32を指定してください


#INCLUDE_PATH += -Ic:/mingw/include/stlport
LIBS += -lcppunit

CXXFLAGS += -g -Wall -W -Wsign-compare -Wunused -W \
	-fno-default-inline $(CXXOPTION) $(INCLUDE_PATH) -I.. \
	-D_WIN32_WINNT=0x0501

#CXXFLAGS= -pedantic $(NOCLEANSCXXFLAGS)

TARGET= UnitTest

OBJS= TestMain.o \
	TreeTest.o \
	IPTest.o \
	RerunnableThreadManagerTest.o \
	ReaderWriterLockTest.o \
	BarrierSyncTest.o \
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
	ExifTest.o \
	ShiftJISTest.o \
	TreeTest.o \
	BitmapTest.o \
	ImageTest.o \
#	IPTest.o \
#	GraphicsTest.o \
# 	EventTest.o \
#	ThreadPoolTest.o \
#	SamplingScannerTest.o \
#	FontGrapherTest.o \

DEPENDFILES=$(OBJS:.o=.d) 

.PHONY: clean %.d makedepend cleandepend remake

all: $(TARGET)

makedepend: cleandepend $(DEPENDFILES)

test: all
	./$(TARGET)

document: ../Doxyfile
	cd .. && doxygen

$(TARGET): $(OBJS) Makefile
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS) 


%.d: %.cpp
	$(CXX)  -I.. -MM -MT $(<:.cpp=.o) $< > $@

clean:
	rm -f $(OBJS) $(TARGET).exe

cleandepend:
	rm -f *.d

remake: cleandepend makedepend

include $(DEPENDFILES)
