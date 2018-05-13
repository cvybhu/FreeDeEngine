TARGET_EXEC ?= main

BUILD_DIR ?= .
SRC_DIRS ?= ./src

CXXFLAGS := -DGLM_ENABLE_EXPERIMENTAL -DGLFW_INCLUDE_NONE -std=c++17 -Wall -Wshadow -O3
LDFLAGS := -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl
INCLUDE_DIRS := -I./include  -I./scr


SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@


.PHONY: clean

clean:
	find $(BUILD_DIR)/$(SRC_DIRS) -name "*.o" -type f -delete && find $(BUILD_DIR)/$(SRC_DIRS) -name "*.d" -type f -delete

-include $(DEPS)

MKDIR_P ?= mkdir -p

