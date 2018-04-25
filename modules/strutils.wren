class Lexer {
    construct new(str) {
        _cursor = 0
        _parts = str.codePoints.map {|c| String.fromCodePoint(c)}.toList
    }

    hasNext { _cursor < _parts.count }

    peek { _parts[_cursor] }

    peekAhead(n) {
        return _parts[_cursor + n]
    }

    match(test) {
        return _parts.skip(_cursor).take(test.count).join("") == test
    }

    consumeAll() {
        return _parts.skip(_cursor).join("")
    }

    consumeUntil(c) {
        var buffer = []

        while (hasNext) {
            var ch = this.peek

            if (ch == c) {
                break
            } else {
                buffer.add(ch)
                this.skip()
            }
        }

        return buffer.join("")
    }

    consumeUntilNewline() {
        var buffer = []

        while (hasNext) {
            var ch = this.peek

            if (ch == "\n") {
                this.skip()
                break
            } else if (ch == "\r") {
                if (this.peekAhead(1) == "\n") {
                    this.skip(2)
                    break
                }
            } else {
                buffer.add(ch)
                this.skip()
            }
        }

        return buffer.join("")
    }

    skip() {
        _cursor = _cursor + 1
    }

    skip(n) {
        _cursor = _cursor + n
    }

    skipMatching(test) {
        var i = 0
        var parts = test.codePoints.map {|c| String.fromCodePoint(c)}.toList

        while (hasNext) {
            var ch = this.peek

            if (i >= parts.count) break

            if (ch != parts[i]) {
                return false
            }

            i = i + 1
            this.skip()
        }

        return true
    }

    skipWhitespace() {
        while (hasNext) {
            var ch = this.next()

            if (ch == "\n" || ch == "\t" || ch == " ") {
            } else if (ch == "\r" && this.peekAhead(2) == "\n") {
                this.skip()
            } else {
                this.skip(-1)
                break
            }
        }
    }

    next() {
        var p = _parts[_cursor]
        this.skip()

        return p
    }
}
