UNAME := $(shell uname)
TARGET_SERVER = multiproc_server
TARGET_CLIENT = multiproc_client

CXXFLAGS = -std=c++14 -Wall -Wextra -pedantic
IMGUI_SOURCES = $(wildcard imgui/*.cpp)
SERVER_SOURCES = $(wildcard *server.cpp) imdbg.cpp $(IMGUI_SOURCES)
#CLIENT_SOURCES = $(wildcard *client.cpp)
CLIENT_SOURCES = main_client.cpp
COMMON_INCLUDES = -isystem.
CLIENT_DEFINE = -DVDBG_ENABLED

PLATFORM_LD_FLAGS = 
ifeq ($(UNAME), Linux)
   CXX = g++7
   PLATFORM_LD_FLAGS = -lGL
else ifeq ($(UNAME), Darwin)
   CXX = clang++
   PLATFORM_LD_FLAGS = -framework OpenGL -framework Quartz -framework Cocoa -Wl,-undefined,dynamic_lookup
endif

#includes
INC = $(COMMON_INCLUDES)
LDFLAGS = -lpthread ./SDL2/libSDL2.a -ldl $(PLATFORM_LD_FLAGS)

.PHONY: all

# server target
$(TARGET_SERVER): $(SERVER_SOURCES)
	$(CXX) $(CXXFLAGS) $(SERVER_SOURCES) $(INC) $(LDFLAGS) -o $(TARGET_SERVER)

# client target
$(TARGET_CLIENT): $(CLIENT_SOURCES)
	$(CXX) $(CXXFLAGS) $(CLIENT_SOURCES) $(INC) $(LDFLAGS) -o $(TARGET_CLIENT) $(CLIENT_DEFINE)

.PHONY : clean
clean:
	find . -type f -name '*.o' -delete
	rm $(TARGET_SERVER) $(TARGET_CLIENT)


.PHONY: debug
debug :  CXXFLAGS += -g -D_DEBUG -Werror -pg
debug : $(TARGET_SERVER) $(TARGET_CLIENT)

.PHONY: release
release :   CXXFLAGS += -O3 -DNDEBUG
release : $(TARGET_SERVER) $(TARGET_CLIENT)