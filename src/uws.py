import asyncio
import uwebsocketspy

# We expose our own event loop for use with asyncio
class Loop(asyncio.SelectorEventLoop):
	def __init__(self):
		self.selector = uwebsocketspy.Selector()
		super().__init__(self.selector)
	def call_soon(self, *args, **kwargs):
		self.selector.tick();
		return super().call_soon(*args, **kwargs)
	def call_at(self, *args, **kwargs):
		self.selector.tick();
		return super().call_at(*args, **kwargs)

App = uwebsocketspy.App
