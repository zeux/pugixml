config=debug
defines=standard

BUILD=build/make-$(CXX)-$(config)-$(defines)

SOURCES=src/pugixml.cpp $(wildcard tests/*.cpp)
EXECUTABLE=$(BUILD)/test

CXXFLAGS=-c -g -Wall -Wextra -Werror -pedantic
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
	@find $(BUILD) -name '*.gcda' | xargs rm
	./$(EXECUTABLE)
	@gcov -b -c $(BUILD)/src/pugixml.cpp.gcda | sed -e '/./{H;$!d;}' -e 'x;/pugixml.cpp/!d;'
	@ls *.gcov | grep -v pugixml.cpp.gcov | xargs rm
else
test: $(EXECUTABLE)
	./$(EXECUTABLE)
endif

clean:
	rm -rf $(BUILD)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD)/%.o: %
	@mkdir -p $(dir $@)
	$(CXX) $< $(CXXFLAGS) -MMD -MP -o $@

-include $(OBJECTS:.o=.d)

.PHONY: all test clean	