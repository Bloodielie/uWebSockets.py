# PYTHONPATH=. python examples/Simple.py
import uwebsocketspy as uWS

app = uWS.App({
        "some": "option"
})

def getHandler(res, req):
	res.fastEnd("Hello Python!")

app.get("/*", getHandler)

def listenHandler():
        print("Listening to port 3000")

app.listen(3000, listenHandler)

# Note: Will block asyncio for now, should integrate seamlessly with uvloop later on
app.run()

