# Japronto only runs on Linux, Python below 3.7 (discontinued project)

from japronto import Application

def hello(request):
    return request.Response(text='Hello Python!')

app = Application()

# They do routing
r = app.router
r.add_route('/', hello, method='GET')

app.run()
