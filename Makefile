LIB = libgbsndemu.a

CXXFLAGS = `sdl2-config --cflags` -std=c++11 -pipe -I ../inc

CXXFILES = gb_apu/Blip_Buffer.cpp gb_apu/Gb_Apu.cpp gb_apu/Gb_Oscs.cpp gb_apu/Multi_Buffer.cpp Basic_Gb_Apu.cpp Sound_Queue.cpp

CXXOBJECTS = $(CXXFILES:.cpp=.o)

SOURCES = $(CXXFILES)
OBJECTS = $(CXXOBJECTS)

all: opt

opt: CXXFLAGS += -DNDEBUG -march=native -O2
opt: $(LIB)

debug: CXXFLAGS += -g -D_DEBUG
debug: $(LIB)

coverage: CXXFLAGS += -g -D_DEBUG -fprofile-arcs -ftest-coverage
coverage: $(LIB)

$(LIB): $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIB) $(OBJECTS)

%.o: %.cpp $(PCH)
	$(CXX) $(CXXFLAGS) $< -c -o $@

.PHONY: clean
clean:
	-rm -f $(LIB) $(OBJECTS) $(PCH)
