CXXFLAGS := -std=c++1y
PREFIX := /usr

-include config.mk

TESTS := $(patsubst %.cpp,%,$(wildcard test/*.cpp))
COMPILATION_TESTS := $(wildcard test/compilation/*.cpp)
ALL_TESTS := $(TESTS) "caliber $(COMPILATION_TESTS)"

# Include all the existing dependency files for automatic #include dependency
# handling.
-include $(TESTS:=.d)

# Build .o files and the corresponding .d (dependency) files. For more info, see
# <http://scottmcpeak.com/autodepend/autodepend.html>.
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -Iinclude -c $< -o $@
	$(eval TEMP := $(shell mktemp))
	$(CXX) $(CXXFLAGS) -MM -Iinclude $< > $(TEMP)
	@sed -e 's|.*:|$*.o:|' < $(TEMP) > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $(TEMP) | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $(TEMP)

$(TESTS): %: %.o
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -lmettle -o $@

tests: $(TESTS)

.PHONY: test
test: tests
	mettle --output=verbose --color $(ALL_TESTS)

.PHONY: clean
clean:
	rm -f $(TESTS) test/*.o test/*.d

.PHONY: gitignore
gitignore:
	@echo $(TESTS) | sed -e 's|test/||g' -e 's/ /\n/g' > test/.gitignore
