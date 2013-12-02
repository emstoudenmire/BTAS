include ../options.mk

#Define Flags ----------
CCFLAGS= -I. $(ITENSOR_INCLUDEFLAGS) $(CPPFLAGS) $(OPTIMIZATIONS)
CCGFLAGS= -I. $(ITENSOR_INCLUDEFLAGS) $(DEBUGFLAGS)
LIBFLAGS=-L$(ITENSOR_LIBDIR) $(ITENSOR_LIBFLAGS)
LIBGFLAGS=-L$(ITENSOR_LIBDIR) $(ITENSOR_LIBGFLAGS)

#Rules ------------------

%.o: %.C $(ITENSOR_LIBS) $(REL_TENSOR_HEADERS)
	$(CCCOM) -c $(CCFLAGS) -o $@ $<

.debug_objs/%.o: %.C $(ITENSOR_GLIBS) $(REL_TENSOR_HEADERS)
	$(CCCOM) -c $(CCGFLAGS) -o $@ $<

#Targets -----------------

test: test.o $(ITENSOR_LIBS) $(REL_TENSOR_HEADERS)
	$(CCCOM) $(CCFLAGS) test.o -o test $(LIBFLAGS)

test-g: mkdebugdir .debug_objs/test.o $(ITENSOR_GLIBS) $(REL_TENSOR_HEADERS)
	$(CCCOM) $(CCGFLAGS) .debug_objs/test.o -o test-g $(LIBGFLAGS)

test_nditer: test_nditer.o $(ITENSOR_LIBS) $(REL_TENSOR_HEADERS)
	$(CCCOM) $(CCFLAGS) test_nditer.o -o test_nditer $(LIBFLAGS)

test_nditer-g: mkdebugdir .debug_objs/test_nditer.o $(ITENSOR_GLIBS) $(REL_TENSOR_HEADERS)
	$(CCCOM) $(CCGFLAGS) .debug_objs/test_nditer.o -o test_nditer-g $(LIBGFLAGS)

itensor_test: itensor_test.o $(ITENSOR_LIBS) $(REL_TENSOR_HEADERS)
	$(CCCOM) $(CCFLAGS) itensor_test.o -o itensor_test $(LIBFLAGS)

itensor_test-g: mkdebugdir .debug_objs/itensor_test.o $(ITENSOR_GLIBS) $(REL_TENSOR_HEADERS)
	$(CCCOM) $(CCGFLAGS) .debug_objs/itensor_test.o -o itensor_test-g $(LIBGFLAGS)

mkdebugdir:
	mkdir -p .debug_objs

clean:
	rm -fr *.o .debug_objs test test-g itensor_test itensor_test-g test_nditer test_nditer-g
