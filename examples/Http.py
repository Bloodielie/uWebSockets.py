# PYTHONPATH=. python examples/Http.py
import uws
import asyncio

# Integrate with asyncio
asyncio.set_event_loop(uws.Loop())

app = uws.App({
        "some": "option"
})

def getHandler(res, req):
	res.end("Hello Python!")

app.get("/*", getHandler)

def listenHandler():
        print("Listening to port 3000")

app.listen(3000, listenHandler)

# Run asyncio event loop
asyncio.get_event_loop().run_forever()
