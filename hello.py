# Questionable module name
import uwebsocketspy as uWS

def listenHandler():
	print("Listening to port 3000")

def getHandler(res, req):
	res.end("Hello Python!")

def postHandler(res, req):
	res.end("Hello POST!");

app = uWS.App({
	"some": "option"
})
app.get(getHandler)
app.post(postHandler)
app.listen(listenHandler)

# Note: Will block asyncio for now, should integrate seamlessly with uvloop later on
app.run()
