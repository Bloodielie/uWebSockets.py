# PYTHONPATH=. python examples/WebSockets.py
import uws
import asyncio

# Integrate with asyncio
asyncio.set_event_loop(uws.Loop())

app = uws.App()

def wsOpen(ws, req):
    print("WebSocket opened!");

def wsMessage(ws, message, isBinary):
    print('Incoming Message: ',message)
    ws.send('Echo '+message)

def wsClose(ws, code, message):
    print("WebSocket closed!");

app.ws("/soc", {
    "maxPayloadLength": 1024,
    "open": wsOpen,
    "message": wsMessage,
    "close": wsClose
});

def getHandler2(res, req):
    res.end('''
            <!DOCTYPE html>
            <body>
            	<div id='chatbox' type="text" style="width:300px; height:250px; border:solid black 1px;"></div>
            	<input id='message' value='Hello'></input>
            	<input id='clickMe' type="button" value="SEND">
                <script>
                      function updateChatbox( message_in ){
                    		var start_value = document.getElementById("chatbox").innerHTML;
                            var new_content = start_value +"<br/>"+message_in
                            document.getElementById("chatbox").innerHTML= new_content;
                      }
                      var exampleSocket = new WebSocket("ws://localhost:3000/soc");
                      exampleSocket.onopen = function (event) {
                           exampleSocket.send("Opened Connection ");
                      };
                      exampleSocket.onmessage = function (event) {
                            updateChatbox( 'RECEIVED: '+ event.data ) 
                      }
                      function sendMessage(event){	
                    		m = document.getElementById('message').value
                    		updateChatbox( 'SENT: '+m)
                    		exampleSocket.send( m );
                      }
                      document.getElementById("clickMe").onclick = sendMessage;
                </script>
            </body> ''')

app.get("/*", getHandler2)

def listenHandler():
    print("Listening to port 3000 http://localhost:3000")
app.listen(3000, listenHandler)

# Run asyncio event loop
asyncio.get_event_loop().run_forever()