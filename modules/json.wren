import "strutils" for Lexer

class JsonParser {
    construct new(input) {
        _input = input
        _lex = Lexer.new(input)
    }

    white() {
        var c = _lex.peek
        if (c == "\r" || c == "\n" || c == " " || c == "\t") {
            _lex.skipWhitespace()
        }
    }

    skip(c) {
        var success = _lex.skipMatching(c)

        if (!success) {
            Fiber.abort("Expected '%(c)' but didn't find one!")
        }
    }

    consumeUntilDelimiter() {
        var buffer = []

        while (_lex.hasNext) {
            var c = _lex.next()

            if (c == "," || c == "}" || c == "]") break

            buffer.add(c)
        }

        return buffer.join("")
    }

    string {
        white()
        if (_lex.peek == "\"") {
            _lex.skip()
            var s = _lex.consumeUntil("\"") // TODO: escapes probably
            _lex.skip()
            return s
        } else {
            Fiber.abort("Expected a string, but did not find one!")
        }
    }

    bool {
        white()
        if (_lex.match("true")) {
            _lex.skipMatching("true")
            return true
        } else if (_lex.match("false")) {
            _lex.skipMatching("false")
            return false
        } else {
            Fiber.abort("Expected a boolean, but did not find one!")
        }
    }

    num {
        white()
        var ns = consumeUntilDelimiter()
        Num.fromString(ns)
    }

    value {
        white()
        var c = _lex.peek

        if (c == "{") {
            return object
        } else if (c == "[") {
            return array
        } else if (c == "\"") {
            return string
        } else if (c == "t" || c == "f") {
            return bool
        } else {
            return num
        }
    }

    object {
        white()
        var obj = {}
        skip("{")

        while (_lex.peek != "}") {
            var key = string
            skip(":")
            white()

            var value = this.value
            white()
            _lex.skipMatching(",")
            white()

            obj[key] = value
        }

        skip("}")
        return obj
    }

    array {
        white()
        var arr = []
        skip("[")

        while (_lex.peek != "]") {
            var value = this.value
            white()
            _lex.skipMatching(",")

            arr.add(value)
        }

        skip("]")
        return arr
    }

    parse { value }
}
