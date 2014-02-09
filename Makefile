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

ifneq ($(defines),standard)
	COMMA=,
	CXXFLAGS+=-D $(subst $(COMMA), -D ,$(defines))
endif

OBJECTS=$(SOURCES:%=$(BUILD)/%.o)

all: $(EXECUTABLE)

test: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -rf $(BUILD)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD)/%.o: %
	@mkdir -p $(dir $@)
	$(CXX) $< $(CXXFLAGS) -MMD -MP -o $@

-include $(OBJECTS:.o=.d)

.PHONY: all test clean	