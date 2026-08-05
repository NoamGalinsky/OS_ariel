all:server client

server: Server/Server.h Server/Server.cpp Server/EulerAlgorithm.h Server/EulerAlgorithm.cpp Server/GraphAlgorithm.h Server/GraphAlgorithm.cpp Server/Graph.h Server/Graph.cpp
	g++ Server/Server.cpp Server/EulerAlgorithm.cpp Server/GraphAlgorithm.cpp Server/Graph.cpp -o server

client: Client/Client.h Client/Client.cpp
	g++ Client/Client.cpp -o client

clean:
	rm client server
