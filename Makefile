CXXFLAGS := -std=c++1y
PREFIX := /usr

-include config.mk

ifndef TMPDIR
  TMPDIR := /tmp
endif

TESTS := $(patsubst %.cpp,%,$(wildcard test/*.cpp))
COMPILATION_TESTS := $(wildcard test/compilation/*.cpp)
ALL_TESTS := $(TESTS) "caliber $(COMPILATION_TESTS)"

# Include all the existing dependency files for automatic #include dependency
# handling.
-include $(TESTS:=.d)

# Build .o files and the corresponding .d (dependency) files. For more info, see
# <http://scottmcpeak.com/autodepend/autodepend.html>.
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -Iinclude -MMD -MF $*.d -c $< -o $@
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d

$(TESTS): %: %.o
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -lmettle -o $@

tests: $(TESTS)

.PHONY: test
test: tests
	mettle --color --output=verbose $(ALL_TESTS)

.PHONY: clean
clean:
	rm -f $(TESTS) test/*.o test/*.d

.PHONY: gitignore
gitignore:
	@echo $(TESTS) | sed -e 's|test/||g' -e 's/ /\n/g' > test/.gitignore
