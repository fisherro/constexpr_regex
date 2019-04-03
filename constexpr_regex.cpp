/*
 * This code starts with Rob Pike's regex code...
 * https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html
 * ...and make it constexpr.
 *
 * TODO:
 * Add +
 * Add ?
 * Add quoting of metacharacters with backslash
 * Add character classes
 *  Can we replace the char of match_star with a structure while remaining
 *  constexpr?
 * Captures?
 *
 * Do a separate glob matcher?
 *  Explicitly matches the whole string
 *  * matches any number of characters
 *  ? matchas any single character
 */

#include <string_view>

namespace cxrx {
    /*
     * Pattern language:
     * . matches any single character
     * ^ matches the beginning of the input
     * $ matches the end of the input
     * * matches zero or more occurrences of the previous character
     *   other characters match themselves
     */

    constexpr bool match_here(std::string_view regex, std::string_view text);

    //Search for c*regex at beginning of text
    constexpr bool match_star(
            const char c, std::string_view regex, std::string_view text)
    {
        do {
            if (match_here(regex, text)) {
                return true;
            }

            if (text.empty()) {
                break;
            }

            if ((c != text[1]) and ('.' != c)) {
                break;
            }
        } while (true);
        return false;
    }

    //match_here: Search for regex at beginning of text
    constexpr bool match_here(std::string_view regex, std::string_view text)
    {
        if (regex.empty()) {
            return true;
        }

        if ('*' == regex[0]) {
            return match_star(regex[0], regex.substr(2), text);
        }

        if (('$' == regex[0]) and (1 == regex.size())) {
            return text.empty();
        }

        if ((not text.empty()) and
            (('.' == regex[0]) or (text[0] == regex[0])))
        {
            return match_here(regex.substr(1), text.substr(1));
        }

        return false;
    }

    //match: Search for regex anywhere in the text
    constexpr bool match(std::string_view regex, std::string_view text)
    {
        if (regex.empty()) {
            return true;
        }

        if ('^' == regex[0]) {
            return match_here(regex.substr(1), text);
        }

        do {
            if (match_here(regex, text)) return true;
            text = text.substr(1);
        } while (not text.empty());

        return false;
    }
}

int main()
{
    using namespace std::literals;

    static_assert(true == cxrx::match("a"sv, "a"sv));
    static_assert(false == cxrx::match("a"sv, "b"sv));
    static_assert(true == cxrx::match("abc"sv, "abc"sv));
    static_assert(false == cxrx::match("abc"sv, "xbc"sv));
    static_assert(false == cxrx::match("abc"sv, "axc"sv));
    static_assert(false == cxrx::match("abc"sv, "abx"sv));

    static_assert(true == cxrx::match("."sv, "a"sv));
    static_assert(true == cxrx::match("."sv, "abc"sv));
    static_assert(true == cxrx::match("..."sv, "abc"sv));
    static_assert(false == cxrx::match("..."sv, "a"sv));

    static_assert(true == cxrx::match("(.*)"sv, "(abc)"sv));
    static_assert(false == cxrx::match("(.*)"sv, "abc"sv));

    static_assert(true == cxrx::match("bcd"sv, "abcde"sv));
    static_assert(false == cxrx::match("^bcd$"sv, "abcde"sv));
    static_assert(true == cxrx::match("^bcd$"sv, "bcd"sv));
    static_assert(false == cxrx::match("^bcd"sv, "abcde"sv));
    static_assert(false == cxrx::match("bcd$"sv, "abcde"sv));
    static_assert(true == cxrx::match("^bcd"sv, "bcde"sv));
    static_assert(true == cxrx::match("bcd$"sv, "abcd"sv));
}

