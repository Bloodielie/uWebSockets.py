# Questionable module name
import uwebsocketspy as uWS

# the app
app = uWS.App({
	"some": "option"
})

# register http get
def getHandler(res, req):
	res.end("Hello Python!")

app.get(getHandler)

# register http post
def postHandler(res, req):
	res.end("Hello POST!")

app.post(postHandler)

# listen
def listenHandler():
	print("Listening to port 3000")

app.listen(listenHandler)

# Note: Will block asyncio for now, should integrate seamlessly with uvloop later on
app.run()
