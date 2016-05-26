#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Copy-2_of_Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/cell.o \
	${OBJECTDIR}/cellularautomata.o \
	${OBJECTDIR}/generate.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/margolus_base.o \
	${OBJECTDIR}/margolus_quasi.o \
	${OBJECTDIR}/margolus_quasi_x.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m32 -fopenmp
CXXFLAGS=-m32 -fopenmp

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/lib -lconfig++

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_CONF}/margolusca322

${CND_CONF}/margolusca322: ${OBJECTFILES}
	${MKDIR} -p ${CND_CONF}
	g++ -o ${CND_CONF}/margolusca322 ${OBJECTFILES} ${LDLIBSOPTIONS} -fopenmp -m32 -Wall

${OBJECTDIR}/cell.o: cell.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/cell.o cell.cpp

${OBJECTDIR}/cellularautomata.o: cellularautomata.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/cellularautomata.o cellularautomata.cpp

${OBJECTDIR}/generate.o: generate.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/generate.o generate.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/margolus_base.o: margolus_base.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/margolus_base.o margolus_base.cpp

${OBJECTDIR}/margolus_quasi.o: margolus_quasi.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/margolus_quasi.o margolus_quasi.cpp

${OBJECTDIR}/margolus_quasi_x.o: margolus_quasi_x.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/margolus_quasi_x.o margolus_quasi_x.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_CONF}/margolusca322

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
