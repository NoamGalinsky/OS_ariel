SERVER_CPP = Server/Server.cpp Server/EulerAlgorithm.cpp Server/GraphAlgorithm.cpp Server/Graph.cpp Server/HamiltonCircuitAlgorithm.cpp Server/MaxCliqueAlgorithm.cpp Server/MSTWeightAlgorithm.cpp Server/NumberOfCliquesAlgorithm.cpp Server/GraphUtilities.cpp Server/GraphAlgorithmFactory.cpp Server/ClientHandler.cpp

SERVER_H = Server/Server.h Server/EulerAlgorithm.h Server/GraphAlgorithm.h Server/Graph.h Server/HamiltonCircuitAlgorithm.h Server/MaxCliqueAlgorithm.h Server/MSTWeightAlgorithm.h Server/NumberOfCliquesAlgorithm.h Server/GraphUtilities.h Server/GraphAlgorithmFactory.h Server/ClientHandler.h

all: server client

server: $(SERVER_CPP) $(SERVER_H)
	g++ $(SERVER_CPP) -o server

client: Client/Client.h Client/Client.cpp
	g++ Client/Client.cpp -o client

clean:
	rm client server