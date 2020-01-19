# PYTHONPATH=. python examples/WebSockets.py
import uwebsocketspy as uWS

app = uWS.App()

def httpGet(res, req):
	res.end("Sorry, no HTTP for you this time")

app.get("/*", httpGet)

def wsOpen(ws, req):
	print("Welcome customer!");

def wsMessage(ws, message, isBinary):
	ws.send(message)

def wsClose(ws, code, message):
	print("Aw, we lost a customer");

app.ws("/*", {
	"maxPayloadLength": 1024,
	"open": wsOpen,
	"message": wsMessage,
	"close": wsClose
});

def listenHandler():
        print("Listening to port 3000")

app.listen(3000, listenHandler)

app.run()

