#
# Makefile for ITensor library
#
################################

#include ../this_dir.mk
include ../options.mk

####################################

SOURCES=util/option.cc 
SOURCES+= index.cc 
SOURCES+= itdata/itdata_functions.cc 
SOURCES+= itensor.cc 
#SOURCES+= iqindex.cc 
#SOURCES+= iqtensor.cc 
#SOURCES+= condenser.cc
#SOURCES+= iqcombiner.cc 
#SOURCES+= svdalgs.cc 
#SOURCES+= mps.cc 
#SOURCES+= mpo.cc 
#SOURCES+= tevol.cc

HEADERS=util/option.h global.h real.h complex.h index.h error.h \
        indexset.h itdata/itdata.h itdata/itdense.h itdata/itdata_functions.h \
        itensor.h 
        #qn.h iqindex.h iqtdat.h iqtensor.h \
        condenser.h combiner.h qcounter.h iqcombiner.h \
        spectrum.h svdalgs.h mps.h mpo.h core.h observer.h DMRGObserver.h \
        sweeps.h stats.h model.h\
        hams/HubbardChain.h hams/Heisenberg.h hams/ExtendedHubbard.h \
        hams/TriHeisenberg.h hams/Ising.h hams/J1J2Chain.h \
        hams/tJChain.h \
        model/spinhalf.h model/spinone.h model/hubbard.h model/spinless.h\
        model/tj.h model/Z3.h\
        eigensolver.h localop.h localmpo.h localmposet.h \
        partition.h hambuilder.h localmpo_mps.h tevol.h dmrg.h bondgate.h\
        integrators.h idmrg.h

####################################

CCFLAGS=-I.. $(ITENSOR_INCLUDEFLAGS) $(OPTIMIZATIONS) -Wno-tautological-compare
CCGFLAGS=-I.. $(ITENSOR_INCLUDEFLAGS) $(DEBUGFLAGS) -Wno-tautological-compare

OBJECTS= $(patsubst %.cc,%.o, $(SOURCES))
GOBJECTS= $(patsubst %.cc,.debug_objs/%.o, $(SOURCES))

.SUFFIXES: .cc $(SUFFIXES)

%.o: %.cc
	$(CCCOM) -c $(CCFLAGS) -o $@ $<

.debug_objs/%.o: %.cc
	$(CCCOM) -c $(CCGFLAGS) -o $@ $<

install: installo installg

libitensor.a: $(OBJECTS)
	ar rv libitensor.a $(OBJECTS)
	ranlib libitensor.a

libitensor-g.a: mkdebugdir $(GOBJECTS)
	ar rv libitensor-g.a $(GOBJECTS)
	ranlib libitensor-g.a

installo: mklibdir libitensor.a
	cp libitensor.a $(ITENSOR_LIBDIR)
	ranlib $(ITENSOR_LIBDIR)/libitensor.a

installg: mklibdir libitensor-g.a
	cp libitensor-g.a $(ITENSOR_LIBDIR)
	ranlib $(ITENSOR_LIBDIR)/libitensor-g.a

mkdebugdir:
	mkdir -p .debug_objs
	mkdir -p .debug_objs/util
	mkdir -p .debug_objs/itdata

mklibdir:
	mkdir -p $(ITENSOR_LIBDIR)

clean:	
	rm -fr *.o .debug_objs libitensor.a libitensor-g.a

DEPHEADERS=real.h util/option.h
util/option.o: $(DEPHEADERS)
.debug_objs/util/option.o: $(DEPHEADERS)
DEPHEADERS+= global.h index.h
index.o: $(DEPHEADERS)
.debug_objs/index.o: $(DEPHEADERS)
DEPHEADERS+= indexset.h 
indexset.o: $(DEPHEADERS)
.debug_objs/indexset.o: $(DEPHEADERS)
DEPHEADERS+= itdata/itdata.h itdata/itdense.h itdata/itdata_functions.h itensor.h 
itensor.o: $(DEPHEADERS)
.debug_objs/itensor.o: $(DEPHEADERS)

#DEPHEADERS+= qn.h iqindex.h
#iqindex.o: $(DEPHEADERS)
#.debug_objs/iqindex.o: $(DEPHEADERS)
#DEPHEADERS+= iqtdat.h iqtensor.h qcounter.h
#iqtensor.o: $(DEPHEADERS)
#.debug_objs/iqtensor.o: $(DEPHEADERS)
#DEPHEADERS+= combiner.h condenser.h iqcombiner.h localmpo.h
#iqcombiner.o: $(DEPHEADERS)
#.debug_objs/iqcombiner.o: $(DEPHEADERS)
#DEPHEADERS+= spectrum.h svdalgs.h
#svdalgs.o: $(DEPHEADERS)
#.debug_objs/svdalgs.o: $(DEPHEADERS)
#DEPHEADERS+= mps.h
#mps.o: $(DEPHEADERS)
#.debug_objs/mps.o: $(DEPHEADERS)
#DEPHEADERS+= mpo.h
#mpo.o: $(DEPHEADERS)
#.debug_objs/mpo.o: $(DEPHEADERS)
#DEPHEADERS+= bondgate.h tevol.h integrators.h
#tevol.o: $(DEPHEADERS)
#.debug_objs/tevol.o: $(DEPHEADERS)
