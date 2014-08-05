CXX := clang++
CXXFLAGS := -std=c++1y -stdlib=libc++ -Wall -Wextra -pedantic -Werror
LDFLAGS := -lsupc++

TESTS := $(patsubst %.cpp,%,$(wildcard test/*.cpp))

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
	mettle --verbose 2 --color test/test_format_string

.PHONY: clean
clean:
	rm -f $(TESTS) test/*.o test/*.d

.PHONY: gitignore
gitignore:
	@echo $(TESTS) | sed -e 's|test/||g' -e 's/ /\n/g' > test/.gitignore
