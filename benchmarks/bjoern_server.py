# pip install bjoern
import bjoern

# This simple test does not include routing
def application(env, start_response):
	start_response('200 OK', [])
	return b"Hello Python!"

# I get poor performance on Linux, much better on macOS?
# 10k ish on Linux, 84k on macOS, let's compare on macOS to make it even!
bjoern.run(application, "localhost", 3000)
