# https://make.mad-scientist.net/papers/advanced-auto-dependency-generation/#combine

EXECUTABLE := a
SOURCE_FILES = $(wildcard *.c)
OBJECT_FILES := $(SOURCE_FILES:.c=.o) # an object file for each C file
DEPENDENCY_FILES := $(SOURCE_FILES:.c=.d)

CC := gcc
# CFLAGS := -Wall -O2 -MP -MD

.PHONY: all
all: $(EXECUTABLE)

.PHONY: clean
clean:
	rm -f $(EXECUTABLE) $(OBJECT_FILES) $(DEPENDENCY_FILES)

# # $(EXECUTABLE): $(OBJECT_FILES)
# # 	$(CC) -o $@ $^
# #
# # %.o: %.c
# # 	$(CC) $(CFLAGS) -c $< -o $@
# #
# # -include $(DEPENDENCY_FILES)

# $(EXECUTABLE): $(OBJECT_FILES)
# 	$(CC) $(CFLAGS) -o $@ $^
# # seems like `$@` is the targer name, so `$(EXECUTABLE)`
# # seems like `$^` are the dependencies, so `$(OBJECT_FILES)`

# %.o: %.c
# 	$(CC) $(CCFLAGS) -o $@ -c $^

DEPFLAGS = -MT $@ -MMD -MP -MF $*.d
# `-MT $@` sets name of target
# `-MMD` generate dep info as a side effect of compilation, not instead; this omits system headers, if u want the mu can use -MD
# `-MP` adds target for each prerequisite in the list (to avoid errors when deleting files)
# `-MF $*.d` where to put the generated dependencies

$(EXECUTABLE): $(OBJECT_FILES)

%.o: %.c
# delete the built-in rules for building object files

# declare the dependencies as prerequisites
%.o: %.c %.d
	$(CC) $(DEPFLAGS) $(CCFLAGS) -c $<

$(DEPENDENCY_FILES): # mention each dependenci as target so that make doesnt cry if it doesnt find them (is this really needed?)

include $(wildcard $(DEPENDENCY_FILES))
