INCLUDES=-I ../Shared -I ../Core -lrt -L. -lCore -lpthread
CXXFLAGS=-m32 -march=i386 -Wunused-variable -w
DEBUGFLAGS=-g3 -D DEBUG
SOURCES=Main.cpp \
        ../Shared/GobiQDLCore.cpp \
        ../Shared/GobiMBNMgmt.cpp
OUTPUT=GobiQDLService

all: clean $(OUTPUT)

$(OUTPUT): $(SOURCES) libCore.a
	$(CXX) $(CXXFLAGS) $(SOURCES) $(INCLUDES) -o $(OUTPUT)

# Must set ANROID_PATH variable
Android: 
	$(MAKE) $(MFLAGS) \
      PATH=$(PATH):$(ANDROID_PATH) \
      CXX=arm-linux-androideabi-g++ \
      AR=arm-linux-androideabi-ar \
      CXXFLAGS=-Wunused-variable \
      INCLUDES="-I ../Core -I ../Shared -L. -lCore" \
      all

debug: 
	$(MAKE) $(MFLAGS) CXXFLAGS="$(CXXFLAGS) $(DEBUGFLAGS)" all

install: $(OUTPUT)
	mkdir -p $(DESTDIR)/opt/Qualcomm/Gobi/GobiQDLService/
	cp -f $(OUTPUT) $(DESTDIR)/opt/Qualcomm/Gobi/GobiQDLService/
	mkdir -p $(DESTDIR)/etc/udev/rules.d/
	cp -f 99-GobiQDLService.rules $(DESTDIR)/etc/udev/rules.d/

clean:
	rm -f $(OUTPUT) \
         libCore.a \
         Comm.o \
         CoreUtilities.o \
         CRC.o \
         DB2TextFile.o \
         Event.o \
         HDLC.o \
         HDLCProtocolServer.o \
         MemoryMappedFile.o \
         ProtocolBuffer.o \
         ProtocolLog.o \
         ProtocolNotification.o \
         ProtocolRequest.o \
         ProtocolServer.o \
         QDLBuffers.o \
         QDLProtocolServer.o \
         SharedBuffer.o

libCore.a: Comm.o \
           CoreUtilities.o \
           CRC.o \
           Event.o \
           HDLC.o \
           HDLCProtocolServer.o \
           MemoryMappedFile.o \
           ProtocolBuffer.o \
           ProtocolLog.o \
           ProtocolNotification.o \
           ProtocolRequest.o \
           ProtocolServer.o \
           QDLBuffers.o \
           QDLProtocolServer.o \
           SharedBuffer.o
	$(AR) rcs libCore.a \
          Comm.o \
          CoreUtilities.o \
          CRC.o \
          Event.o \
          HDLC.o \
          HDLCProtocolServer.o \
          MemoryMappedFile.o \
          ProtocolBuffer.o \
          ProtocolLog.o \
          ProtocolNotification.o \
          ProtocolRequest.o \
          ProtocolServer.o \
          QDLBuffers.o \
          QDLProtocolServer.o \
          SharedBuffer.o

Comm.o: ../Core/Comm.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/Comm.cpp

CoreUtilities.o: ../Core/CoreUtilities.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/CoreUtilities.cpp

CRC.o: ../Core/CRC.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/CRC.cpp

Event.o: ../Core/Event.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/Event.cpp

HDLC.o: ../Core/HDLC.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/HDLC.cpp

HDLCProtocolServer.o: ../Core/HDLCProtocolServer.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/HDLCProtocolServer.cpp

MemoryMappedFile.o: ../Core/MemoryMappedFile.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/MemoryMappedFile.cpp

ProtocolBuffer.o: ../Core/ProtocolBuffer.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/ProtocolBuffer.cpp

ProtocolLog.o: ../Core/ProtocolLog.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/ProtocolLog.cpp

ProtocolNotification.o: ../Core/ProtocolNotification.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/ProtocolNotification.cpp

ProtocolRequest.o: ../Core/ProtocolRequest.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/ProtocolRequest.cpp

ProtocolServer.o: ../Core/ProtocolServer.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/ProtocolServer.cpp

QDLBuffers.o: ../Core/QDLBuffers.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/QDLBuffers.cpp

QDLProtocolServer.o: ../Core/QDLProtocolServer.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/QDLProtocolServer.cpp

SharedBuffer.o: ../Core/SharedBuffer.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/SharedBuffer.cpp

