CCCOM=clang++ -std=c++1y
OPTIMIZATIONS= -O2
DEBUGFLAGS=-DDEBUG -DMATRIXBOUNDS -DITENSOR_USE_AT -DBOUNDS -g -O0 -Wall -Wno-long-long
INCLUDEFLAGS=-I..

#Define source files ----------

SOURCES = test.cc
SOURCES+= index_test.cc
SOURCES+= itensor_test.cc


#Define Flags ----------
CCFLAGS= -I. $(INCLUDEFLAGS) $(OPTIMIZATIONS)
CCGFLAGS= -I. $(INCLUDEFLAGS) $(DEBUGFLAGS)
LIBFLAGS= -framework Accelerate -L../itensor -litensor
LIBGFLAGS= -framework Accelerate -L../itensor -litensor-g

OBJECTS=$(patsubst %.cc,%.o, $(SOURCES))
GOBJECTS=$(patsubst %.cc,.debug_objs/%.o, $(SOURCES))

#Rules ------------------

%.o: %.cc
	$(CCCOM) -c $(CCFLAGS) -o $@ $<

.debug_objs/%.o: %.cc
	$(CCCOM) -c $(CCGFLAGS) -o $@ $<

#Targets -----------------

run: test
	./test

test: $(OBJECTS)
	$(CCCOM) $(CCFLAGS) $(OBJECTS) -o test $(LIBFLAGS)

test-g: mkdebugdir $(GOBJECTS)
	$(CCCOM) $(CCGFLAGS) $(GOBJECTS) -o test-g $(LIBGFLAGS)

mkdebugdir:
	mkdir -p .debug_objs

clean:
	rm -fr *.o .debug_objs test test-g
