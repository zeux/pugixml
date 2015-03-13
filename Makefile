config=debug
defines=standard

BUILD=build/make-$(CXX)-$(config)-$(defines)

SOURCES=src/pugixml.cpp tests/main.cpp tests/allocator.cpp tests/test.cpp tests/writer_string.cpp $(wildcard tests/test_*.cpp)
EXECUTABLE=$(BUILD)/test

CXXFLAGS=-g -Wall -Wextra -Werror -pedantic
LDFLAGS=

ifeq ($(config),release)
	CXXFLAGS+=-O3 -DNDEBUG
endif

ifeq ($(config),coverage)
	CXXFLAGS+=-DNDEBUG
	CXXFLAGS+=-fprofile-arcs -ftest-coverage
	LDFLAGS+=-fprofile-arcs
endif

ifneq ($(defines),standard)
	COMMA=,
	CXXFLAGS+=-D $(subst $(COMMA), -D ,$(defines))
endif

OBJECTS=$(SOURCES:%=$(BUILD)/%.o)

all: $(EXECUTABLE)

ifeq ($(config),coverage)
test: $(EXECUTABLE)
	-@find $(BUILD) -name '*.gcda' | xargs rm
	./$(EXECUTABLE)
	@gcov -b -c $(BUILD)/src/pugixml.cpp.gcda | sed -e '/./{H;$!d;}' -e 'x;/pugixml.cpp/!d;'
	@ls *.gcov | grep -v pugixml.cpp.gcov | xargs rm
else
test: $(EXECUTABLE)
	./$(EXECUTABLE)
endif

fuzz:
	@mkdir -p $(BUILD)
	$(AFL)/afl-clang++ tests/fuzz_parse.cpp tests/allocator.cpp src/pugixml.cpp $(CXXFLAGS) -o $(BUILD)/fuzz_parse
	$(AFL)/afl-fuzz -i tests/data_fuzz_parse -o $(BUILD)/fuzz_parse_out -x $(AFL)/testcases/_extras/xml/ -- $(BUILD)/fuzz_parse @@

clean:
	rm -rf $(BUILD)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD)/%.o: %
	@mkdir -p $(dir $@)
	$(CXX) $< $(CXXFLAGS) -c -MMD -MP -o $@

-include $(OBJECTS:.o=.d)

.PHONY: all test clean	
