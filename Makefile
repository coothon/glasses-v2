include make.conf

BUILD_DIR:=./build
SRC_DIRS:=./src

SRCS:=$(shell find $(SRC_DIRS) -name '*.c' -or -name '*.s')

ANALYZE_SRCS:=$(shell find $(SRC_DIRS) -name '*.c' -or -name '*.s' -or -name '*.h')

OBJS:=$(SRCS:%=$(BUILD_DIR)/%.o)

DEPS:=$(OBJS:.o=.d)

INC_DIRS:=$(shell find $(SRC_DIRS) -type d)
INC_FLAGS:=$(addprefix -I,$(INC_DIRS))

CPPFLAGS:=$(INC_FLAGS) -MMD -MP

.PHONY: clean analyze all

all: clean $(BUILD_DIR)/$(PROJ_NAME)

$(BUILD_DIR)/$(PROJ_NAME): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

# The "insecureAPI" check is often tells you to use non-portable functions, so turn it off.
# The valist check is buggy, so turn it off, too.
analyze:
	clang-tidy $(ANALYZE_SRCS) --quiet \
		--checks="-clang-analyzer-security.insecureAPI.*,-clang-analyzer-valist.Uninitialized,-clang-diagnostic-incompatible-pointer-types-discards-qualifiers" \
		-- $(CPPFLAGS) $(CFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	-rm -r $(BUILD_DIR)

-include $(DEPS)
