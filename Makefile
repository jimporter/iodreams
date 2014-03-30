.PHONY: clean test

test:
	clang++ -std=c++1y -Iinclude -I../mettle/include -lboost_program_options -Wall -Wextra -o test/test_format_string test/test_format_string.cpp
	test/test_format_string --verbose --color

clean:
	rm test/test_format_string
