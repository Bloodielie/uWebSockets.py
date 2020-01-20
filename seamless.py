# this is an example of seamless integration between uWebSockets.py and any asyncio project (here we have aiohttp)
# both servers run on the same event loop yet the performance difference is major (5k vs. 127k or more)

import uwebsocketspy as uWS
import asyncio

# hook up with asyncio (all needed?)
asyncio.set_event_loop(asyncio.SelectorEventLoop(uWS.Selector()))

# aio do your thang
from aiohttp import web

async def handle(request):
    name = request.match_info.get('name', "Anonymous")
    text = "Hello, " + name
    return web.Response(text=text)

app = web.Application()
app.add_routes([web.get('/', handle),
                web.get('/{name}', handle)])

# uwebsockets do its thang
appp = uWS.App({
	"some": "option"
})

def getHandler(res, req):
	res.fastEnd("Hello Python!")

appp.get("/*", getHandler)

def listenHandler():
	print("Listening to port 3000")

appp.listen(3000, listenHandler)

web.run_app(app)
print("we cannot come here!")
