SRCS = logger.cpp 
       
OBJS = logger.o

CXXFLAGS = -fPIC -Wall -std=c++0x -O3

INCLUDE = -I ../../thirdparty/boost_1_65_0/
PHONY : all 
all : $(OBJS) 
	ar r liblog.a $(OBJS)
	g++ -shared -o liblog.so $(OBJS)
	-cp -p liblog.a $(NAUT_ROOTDIR)/public/lib
#	-cp -p liblog.so $(NAUT_ROOTDIR)/public/lib
	-cp -p *.h $(NAUT_ROOTDIR)/public/liblog
$(OBJS):	
	g++ -c $(CXXFLAGS) $(INCLUDE) $(SRCS)

.PHONY : clean
clean:
	rm -f *.a *.o *.so


