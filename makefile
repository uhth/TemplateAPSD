
CXX := g++
CXXFLAGS := --std=c++20 -MMD -MP -g -Wall
OPTFLAG += -O2
LDFLAGS := $(shell pkg-config --libs allegro-5 allegro_primitives-5 allegro_color-5 allegro_font-5 allegro_ttf-5)

MPI_COMPILE_FLAGS := $(shell mpicxx --showme:compile)
MPI_LINK_FLAGS := $(shell mpicxx --showme:link)

LDFLAGS += $(MPI_LINK_FLAGS)
CXXFLAGS += $(MPI_COMPILE_FLAGS)

SRC_DIR := src
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

main.out: $(OBJ_FILES)
	$(CXX) $(LDFLAGS) $(OPTFLAG) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(OPTFLAG) -c -o $@ $<

clean:
	rm -rf ${OBJ_FILES}

-include $(OBJ_FILES:.o=.d)