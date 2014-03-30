# iodreams

`iodreams` is a (currently proof-of-concept) library designed to replace the
standard C++ I/O streams library with something that takes better advantage of
modern C++ (read: this will use the bleedingest-edge C++ humanly possible
because that's **cool**).

## Usage

I don't know why you'd want to use this library now but here's how it looks so
far:

```
io::format(std::cout,
           "First we have {1}; wait, before that was {0}. Finally, {2}.\n",
           "foo", 1, "bar");

```

## Dependencies

This project currently requires a C++11 compiler. To run the tests, you'll need
a C++14 compiler with Boost.

## License

This library is licensed under the BSD 3-Clause license.
