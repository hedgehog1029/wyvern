import "strutils" for Lexer

foreign class Socket {
    construct new(protocol, uri, port) {}
    foreign connect()
    foreign read(n)
    foreign write(buf)
    foreign close()
}

class URL {
    construct parse(uri) {
        _original = uri
        var lex = Lexer.new(uri)

        _protocol = lex.consumeUntil(":")
        lex.skipMatching("://")
        _host = lex.consumeUntil("/")
        _path = lex.consumeUntil("?")
        _query = lex.consumeAll()
    }

    protocol { _protocol }
    host { _host }
    path { _path }
    query { _query.split("&") }
    queryStr { _query }

    fullPath { _path + _query }

    toString { _protocol + "://" + _host + fullPath }
}

class HttpRequestBuilder {
    construct new() {
        _method = "GET"
        _path = "/"
        _headers = []
    }

    method(method) {
        _method = method
        return this
    }

    path(path) {
        _path = path
        return this
    }

    addHeader(key, val) {
        _headers.add("%(key): %(val)")
        return this
    }

    buildHeaders { _headers.join("\r\n") }

    build() {
        return "%(_method) %(_path) HTTP/1.1\r\n%(buildHeaders)\r\n\r\n"
    }
}

class HttpResponse {
    construct parse(res) {
        var lex = Lexer.new(res)

        _statusline = lex.consumeUntilNewline()
        var statusParts = _statusline.split(" ")
        _status = Num.fromString(statusParts[1])

        _headers = {}

        while (true) {
            var header = lex.consumeUntilNewline()
            
            if (header.bytes.count == 0) {
                break
            }

            var parts = header.split(": ")
            _headers[parts[0]] = parts[1]
        }

        _body = lex.consumeAll()
    }

    statusCode { _status }
    headers { _headers }
    body { _body }

    toString { "HttpResponse(status=%(statusCode), headers=%(headers))" }
}

class HttpClient {
    construct new() {}

    request(method, url, body, headers) {
        var uri = URL.parse(url)
        var sock = Socket.new("tcp", uri.host, "80")
        var req = HttpRequestBuilder.new().
            method(method).path(uri.fullPath).
            addHeader("User-Agent", "WyvernHttp/0.1.0 Wyvern/0.0.1 stdlib").
            addHeader("Host", uri.host)

        for (k in headers.keys) {
            req.addHeader(k, headers[k])
        }

        var bodyLength = body.bytes.count
        if (bodyLength > 0) {
            req.addHeader("Content-Length", bodyLength.toString)
        }

        sock.connect()
        sock.write(req.build())
        if (bodyLength > 0) {
            sock.write(body)
        }

        var res = ""
        while (true) {
            var chunk = sock.read(1024)

            res = res + chunk
            if (chunk.count < 1024) break
        }

        sock.close()
        return HttpResponse.parse(res)
    }

    request(method, url) {
        return request(method, url, "", {})
    }
}
