TARGET = SYDEsup
TEMPLATE = app
CONFIG += console

#no Qt stuff at all (pure ANSI-C++)
QT -= core
QT -= gui

#add windows API libraries
LIBS += -lversion
LIBS += -lws2_32   #WinSock
LIBS += -lIphlpapi #IP helper API

#link all libraries statically
QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++

#output directory
DESTDIR = ../result

#version info:
RC_FILE = ../src/resources.rc

# drag in openSYDE Core:
opensyde_core_skip_modules += opensyde_core_skip_code_generation
# do not drag in the zipping/unzipping classes; we have a special handling for miniz.c due to compiler warnings
#  do we refer to those files manually
opensyde_core_skip_modules += opensyde_core_skip_imports
opensyde_core_skip_modules += opensyde_core_skip_protocol_logging
opensyde_core_skip_modules += opensyde_core_skip_zipping
include(../libs/opensyde_core/opensyde_core.pri)

SOURCES += ../src/C_SUPSuSequences.cpp \
           ../src/C_SYDEsup.cpp \
           ../src/main.cpp \
           ../libs/opensyde_core/C_OSCZipFile.cpp

#using our standard compiler warning switches we will get some (non-critical) warnings in miniz.c
#we do not want to modify that library (as it is not maintained by us)
#so we want to suppress those warnings
#to do that we define an additional compiler named "miniz" and assign miniz.c to be built with that compiler
#see https://stackoverflow.com/questions/27683777/how-to-specify-compiler-flag-to-a-single-source-file-with-qmake
# for more information
win32-g++ {
   SOURCES_MINIZ = ../libs/opensyde_core/miniz/miniz.c  #define sources to be built with additional compiler
   miniz.name = miniz
   miniz.input = SOURCES_MINIZ              #assign sources
   miniz.dependency_type = TYPE_C
   miniz.variable_out = OBJECTS
   miniz.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_IN_BASE}$${first(QMAKE_EXT_OBJ)}
   #invoke C compiler (as it's a C file)
   miniz.commands = $${QMAKE_CC} $(CFLAGS) $(INCPATH) -Wno-strict-aliasing -Wno-extra -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
   QMAKE_EXTRA_COMPILERS += miniz
}

HEADERS += ../src/C_SUPSuSequences.h \
           ../src/C_SYDEsup.h \
           ../libs/opensyde_core/C_OSCZipFile.h \
           ../libs/opensyde_core/miniz/miniz.h

PRECOMPILED_HEADER = ../src/precomp_headers.h

#regular (non-library) header paths (also lists the system ones so QtCreator parser sees them)
INCLUDEPATH += ../src \
               ../libs/opensyde_core/miniz

DISTFILES += \
    ../doc/about.txt \
    ../doc/build.txt \
    ../doc/userlist.txt
