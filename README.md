<div align="center">
<img src="misc/logo.svg" height="180" />

*µWebSockets.py™ (it's "[micro](https://en.wikipedia.org/wiki/Micro-)") is simple, secure*<sup>[[1]](https://github.com/uNetworking/uWebSockets/tree/master/fuzzing)</sup> *& standards compliant*<sup>[[2]](https://unetworking.github.io/uWebSockets.js/report.pdf)</sup> *web I/O for the most demanding*<sup>[[3]](https://github.com/uNetworking/uWebSockets/tree/master/benchmarks)</sup> *of applications.*

• [For JavaScript](https://github.com/uNetworking/uWebSockets.js) • [User manual](https://github.com/uNetworking/uWebSockets/blob/master/misc/READMORE.md) • [Docs](https://unetworking.github.io/uWebSockets.js/generated/)

*© 2016-2019, >39,632,272 downloads*

</div>

#### In a nutshell

µWebSockets is a fast C++ web server and broker made available to JavaScript developers since 2016, now also available to Python developers. It integrates seamlessly to any `asyncio` project as long as `uvloop` is set up and used as event loop.

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

For a performance reference, consider [Japronto performance graph](https://github.com/squeaky-pl/japronto#performance), then the following graph:

![](misc/perf.png)

The idea here is to ultimately provide a seamless, pip install kind of works-out-of-the-box experience but for Python developers.
