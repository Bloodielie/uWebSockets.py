# PYTHONPATH=. python examples/WebSockets.py
import uws
import asyncio

# Integrate with asyncio
asyncio.set_event_loop(uws.Loop())

app = uws.App()

def wsOpen(ws, req):
	print("WebSocket opened!")

def wsMessage(ws, message, isBinary):
	ws.send(message)

def wsClose(ws, code, message):
	print("WebSocket closed!")

app.ws("/*", {"maxPayloadLength": 1024,
              "open": wsOpen,
              "message": wsMessage,
              "close": wsClose
              })

def listenHandler():
        print("Listening to port 3000")

app.listen(3000, listenHandler)

# Run asyncio event loop
asyncio.get_event_loop().run_forever()

