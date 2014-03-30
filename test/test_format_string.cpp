#include "mettle.hpp"
#include "iodreams/format.hpp"
using namespace mettle;

namespace io {
  inline bool operator ==(const format_placeholder &lhs,
                          const format_placeholder &rhs) {
    return lhs.index == rhs.index && lhs.params == rhs.params;
  }

  inline std::ostream & operator <<(std::ostream &o,
                                    const format_placeholder &ph) {
    return o << "placeholder(" << ph.index << ", \"" << ph.params << "\")";
  }
}

suite<> format_suite("build format strings", [](auto &_) {
  using ph = io::format_placeholder;

  _.test("no placeholders", []() {
    io::format_string empty("");
    expect(empty.strings_, array(""));
    expect(empty.placeholders_, array());

    io::format_string text("text");
    expect(text.strings_, array("text"));
    expect(text.placeholders_, array());
  });

  _.test("escaped braces", []() {
    io::format_string open("{{");
    expect(open.strings_, array("{"));
    expect(open.placeholders_, array());

    io::format_string close("}}");
    expect(close.strings_, array("}"));
    expect(close.placeholders_, array());

    io::format_string both("{{}}");
    expect(both.strings_, array("{}"));
    expect(both.placeholders_, array());

    io::format_string mixed("open: {{, close: }}, hooray!");
    expect(mixed.strings_, array("open: {, close: }, hooray!"));
    expect(mixed.placeholders_, array());
  });

  _.test("one param-less placeholder", []() {
    io::format_string single("{0}");
    expect(single.strings_, array("", ""));
    expect(single.placeholders_, array(ph(0, "")));

    io::format_string text_before("text {0}");
    expect(text_before.strings_, array("text ", ""));
    expect(text_before.placeholders_, array(ph(0, "")));

    io::format_string text_after("{0} text");
    expect(text_after.strings_, array("", " text"));
    expect(text_after.placeholders_, array(ph(0, "")));

    io::format_string text_both("text {0} more text");
    expect(text_both.strings_, array("text ", " more text"));
    expect(text_both.placeholders_, array(ph(0, "")));
  });

  _.test("one param-ful placeholder", []() {
    io::format_string single("{0|param}");
    expect(single.strings_, array("", ""));
    expect(single.placeholders_, array(ph(0, "param")));

    io::format_string text_before("text {0|param}");
    expect(text_before.strings_, array("text ", ""));
    expect(text_before.placeholders_, array(ph(0, "param")));

    io::format_string text_after("{0|param} text");
    expect(text_after.strings_, array("", " text"));
    expect(text_after.placeholders_, array(ph(0, "param")));

    io::format_string text_both("text {0|param} more text");
    expect(text_both.strings_, array("text ", " more text"));
    expect(text_both.placeholders_, array(ph(0, "param")));
  });

  _.test("multiple placeholders", []() {
    io::format_string pair("{0}{1}");
    expect(pair.strings_, array("", "", ""));
    expect(pair.placeholders_, array(ph(0, ""), ph(1, "")));

    io::format_string pair_paramful("{0|foo}{1|bar}");
    expect(pair_paramful.strings_, array("", "", ""));
    expect(pair_paramful.placeholders_, array(ph(0, "foo"), ph(1, "bar")));
  });

  subsuite(_, "invalid format strings", [](auto &_) {
    _.test("unterminated placeholder", []() {
      std::string placeholders[] = { "{", "{0", "{|", "{0|", "{|foo",
                                     "{0|foo" };

      for(auto &i : placeholders) {
        auto thrown_eos = thrown<io::format_string_error>(
          "unexpected end of string"
        );
        expect([&i]() { io::format_string f(i); }, thrown_eos);
        expect([&i]() { io::format_string f("text " + i); }, thrown_eos);
      }
    });

    _.test("unexpected closing brace", []() {
        expect([]() { io::format_string f("}"); },
               thrown<io::format_string_error>("unexpected end of string"));
        expect([]() { io::format_string f("}a"); },
               thrown<io::format_string_error>("unexpected '}' in string"));

    });

    _.test("bad character in placeholder", []() {
      expect(
        []() { io::format_string f("{a}"); },
        thrown<io::format_string_error>("unexpected character in placeholder")
      );
      expect(
        []() { io::format_string f("{0a|"); },
        thrown<io::format_string_error>(
          "unexpected character in placeholder index"
        )
      );
      expect(
        []() { io::format_string f("{|\xff}"); },
        thrown<io::format_string_error>(
          "unexpected character in placeholder params"
        )
      );
    });
  });
});
