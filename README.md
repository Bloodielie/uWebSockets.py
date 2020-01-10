# µWebSockets.py
This is a work-in-progress Python extension much like the Node.js addon µWebSockets.js. It performs very nicely, even outrunning the Node.js addon already, by a quite large margin. Python is an excellent interpreter with a lean and unforgiving C API that sure doesn't let me down.

The idea here is to ultimately provide a seamless, pip install kind of works-out-of-the-box experience but for Python developers.

This extension already outperforms Japronto, but will of course have a much more mature feature set. Here's what works right now:
```python
# Questionable module name
import uwebsocketspy as uWS

port = 4000

# the app
app = uWS.App({
        "some": "option"
})

# register http get
def getHandler(res, req):
        res.end("Hello Python!")

app.get("/*", getHandler)

# register http post
def postHandler(res, req):
        res.end("Hello POST!")

app.post("/*", postHandler)

# register candy
def candyHandler(res, req):
    res.end("Candy candy!");

app.get("/candy", candyHandler);

# register websockets
def wsOpen(ws, req):
	ws.send("I'm alive!");
	print("new websocket!")

def wsClose(ws, code, message):
	print("websocket gone!")

def wsDrain():
	print("websocket drain")

def wsMessage(ws, message, isBinary):
	ws.send(message)

app.ws("/*", {
	"maxPayloadLength": 1024,
	"open": wsOpen,
	"close": wsClose,
	"message": wsMessage,
	"drain": wsDrain
});

# listen
def listenHandler():
        print("Listening to port " + str(port))

app.listen("localhost", port, listenHandler)

# Note: Will block asyncio for now, should integrate seamlessly with uvloop later on
app.run()

```
