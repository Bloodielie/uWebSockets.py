default:
	WITH_LIBUV=1 make -C uWebSockets/uSockets
	g++ -std=c++17 -shared -O3 -flto -fPIC -I /usr/include/python3.8 -I uWebSockets/uSockets/src -I uWebSockets/src src/extension.cpp uWebSockets/uSockets/uSockets.a -lz -luv -o uwebsocketspy.so
