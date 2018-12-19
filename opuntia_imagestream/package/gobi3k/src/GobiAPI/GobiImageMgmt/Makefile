INCLUDES=-I ../Core -I ../Shared -lrt -L. -lCore
CXXFLAGS=-m32 -march=i386 -Wunused-variable -w
LDFLAGS=-melf_i386
DEBUGFLAGS=-g3 -D DEBUG
QMICOREFLAGS=-D IMG_SUPPORT
SOURCES=GobiImageMgmtExports.cpp \
        GobiImageMgmt.cpp \
        ../Shared/GobiQMICore.cpp \
        ../Shared/GobiQMICoreImg.cpp \
        ../Shared/GobiQDLCore.cpp \
        ../Shared/GobiMBNMgmt.cpp
OUTPUT=GobiImageMgmt.so

all: clean $(OUTPUT)

$(OUTPUT): $(SOURCES) libCore.a
	$(CXX) $(CXXFLAGS) $(QMICOREFLAGS) -shared $(SOURCES) $(INCLUDES) -o $(OUTPUT) 

# Must set ANDROID_PATH variable
Android: 
	$(MAKE) $(MFLAGS) \
      PATH=$(PATH):$(ANDROID_PATH) \
      CXX=arm-linux-androideabi-g++ \
      LD=arm-linux-androideabi-ld \
      AR=arm-linux-androideabi-ar \
      CXXFLAGS=-Wunused-variable \
      LDFLAGS=-marmelf_linux_eabi \
      INCLUDES="-I ../Core -I ../Shared -L. -lCore" \
      all

install: $(OUTPUT)
	mkdir -p $(DESTDIR)/opt/Qualcomm/Gobi/
	cp -f $(OUTPUT) $(DESTDIR)/opt/Qualcomm/Gobi/

clean:
	rm -f $(OUTPUT) \
         libCore.a \
         QMIDB.o \
         BitPacker.o \
         BitParser.o \
         Comm.o \
         CoreDatabase.o \
         CoreUtilities.o \
         CRC.o \
         DataPacker.o \
         DataParser.o \
         DB2NavTree.o \
         DB2TextFile.o \
         DB2Utilities.o \
         Event.o \
         HDLC.o \
         HDLCProtocolServer.o \
         MemoryMappedFile.o \
         ProtocolBuffer.o \
         ProtocolEntityNav.o \
         ProtocolLog.o \
         ProtocolNotification.o \
         ProtocolRequest.o \
         ProtocolServer.o \
         QDLBuffers.o \
         QDLProtocolServer.o \
         QMIBuffers.o \
         QMIProtocolServer.o \
         SharedBuffer.o

debug: 
	$(MAKE) $(MFLAGS) CXXFLAGS="$(CXXFLAGS) $(DEBUGFLAGS)" all

libCore.a: QMIDB.o \
           BitPacker.o \
           BitParser.o \
           Comm.o \
           CoreDatabase.o \
           CoreUtilities.o \
           CRC.o \
           DataPacker.o \
           DataParser.o \
           DB2NavTree.o \
           DB2TextFile.o \
           DB2Utilities.o \
           Event.o \
           HDLC.o \
           HDLCProtocolServer.o \
           MemoryMappedFile.o \
           ProtocolBuffer.o \
           ProtocolEntityNav.o \
           ProtocolLog.o \
           ProtocolNotification.o \
           ProtocolRequest.o \
           ProtocolServer.o \
           QDLBuffers.o \
           QDLProtocolServer.o \
           QMIBuffers.o \
           QMIProtocolServer.o \
           SharedBuffer.o
	$(AR) rcs libCore.a \
          QMIDB.o \
          BitPacker.o \
          BitParser.o \
          Comm.o \
          CoreDatabase.o \
          CoreUtilities.o \
          CRC.o \
          DataPacker.o \
          DataParser.o \
          DB2NavTree.o \
          DB2TextFile.o \
          DB2Utilities.o \
          Event.o \
          HDLC.o \
          HDLCProtocolServer.o \
          MemoryMappedFile.o \
          ProtocolBuffer.o \
          ProtocolEntityNav.o \
          ProtocolLog.o \
          ProtocolNotification.o \
          ProtocolRequest.o \
          ProtocolServer.o \
          QDLBuffers.o \
          QDLProtocolServer.o \
          QMIBuffers.o \
          QMIProtocolServer.o \
          SharedBuffer.o

QMIDB.o: ../Database/QMI/Entity.txt \
         ../Database/QMI/EnumEntry.txt \
         ../Database/QMI/Enum.txt \
         ../Database/QMI/Field.txt \
         ../Database/QMI/Struct.txt
	cp ../Database/QMI ./ -rf
	$(LD) $(LDFLAGS) -r -b binary -o QMIDB.o \
	              QMI/Entity.txt \
	              QMI/EnumEntry.txt \
	              QMI/Enum.txt \
	              QMI/Field.txt \
	              QMI/Struct.txt
	rm QMI -rf

BitPacker.o: ../Core/BitPacker.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/BitPacker.cpp

BitParser.o: ../Core/BitParser.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/BitParser.cpp

Comm.o: ../Core/Comm.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/Comm.cpp

CoreDatabase.o: ../Core/CoreDatabase.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/CoreDatabase.cpp

CoreUtilities.o: ../Core/CoreUtilities.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/CoreUtilities.cpp

CRC.o: ../Core/CRC.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/CRC.cpp

DataPacker.o: ../Core/DataPacker.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/DataPacker.cpp

DataParser.o: ../Core/DataParser.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/DataParser.cpp

DB2NavTree.o: ../Core/DB2NavTree.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/DB2NavTree.cpp

DB2TextFile.o: ../Core/DB2TextFile.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/DB2TextFile.cpp

DB2Utilities.o: ../Core/DB2Utilities.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/DB2Utilities.cpp

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

ProtocolEntityNav.o: ../Core/ProtocolEntityNav.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/ProtocolEntityNav.cpp

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

QMIBuffers.o: ../Core/QMIBuffers.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/QMIBuffers.cpp

QMIProtocolServer.o: ../Core/QMIProtocolServer.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/QMIProtocolServer.cpp

SharedBuffer.o: ../Core/SharedBuffer.cpp
	$(CXX) $(CXXFLAGS) -c ../Core/SharedBuffer.cpp
	
