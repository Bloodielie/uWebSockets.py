# work in progress, come back in a month

<div align="center">
<img src="misc/logo.svg" height="180" />

*µWebSockets.py™ (it's "[micro](https://en.wikipedia.org/wiki/Micro-)") is simple, secure*<sup>[[1]](https://github.com/uNetworking/uWebSockets/tree/master/fuzzing)</sup> *& standards compliant*<sup>[[2]](https://unetworking.github.io/uWebSockets.js/report.pdf)</sup> *web I/O for the most demanding*<sup>[[3]](https://github.com/uNetworking/uWebSockets/tree/master/benchmarks)</sup> *of applications.*

• [For JavaScript](https://github.com/uNetworking/uWebSockets.js) • [User manual](https://github.com/uNetworking/uWebSockets/blob/master/misc/READMORE.md) • [Docs](https://unetworking.github.io/uWebSockets.js/generated/)

*© 2016-2019, >39,632,272 downloads*

</div>

#### In a nutshell

`µWebSockets.py` is an ultra fast and standards compliant Http/WebSockets server/broker/router available to JavaScript developers since 2016, now also Python developers (hello handsome!). It is written entirely in C++ and integrates seamlessly with any `asyncio` project by plugging the elegant Python interpreter with its own `Selector` implementation.

```python
app = uWS.App()

def httpGet(res, req):
	res.end("Sorry, no HTTP for you this time")

app.get("/*", httpGet)

def wsMessage(ws, message, isBinary):
	ws.send(message)

app.ws("/*", {
	"maxPayloadLength": 1024,
	"message": wsMessage
});

def listenHandler():
        print("Listening to port 3000")

app.listen(3000, listenHandler)
```

#### Ready all thrusters

* `pip install uws` no compiler needed, has no dependencies. Integrates seamlessly with Python 3.6+.

For a performance reference, consider [Japronto performance graph](https://github.com/squeaky-pl/japronto#performance), then the following graph:

![](misc/perf.png)

#### Pay what you want.
Commercially developed on a sponsored/consulting basis; BitMEX, Bitfinex and Coinbase are current or previous sponsors. Contact [me, the author](https://github.com/alexhultman) for support, feature development or consulting/contracting.

![](https://raw.githubusercontent.com/uNetworking/uWebSockets/master/misc/2018.png)

#### Know thy legal matters.

*µWebSockets.py is intellectual property licensed Apache 2.0 with limitations on trademark use. Forks must be clearly labelled as such and must not be confused with the original.*
