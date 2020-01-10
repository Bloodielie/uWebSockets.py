# µWebSockets.py
**WIP extension for µWebSockets**
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
It performs significantly better than japronto, despite being a far more complete and mature project.
