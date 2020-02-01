all: serverA serverB serverC aws.server monitor client

serverA: serverA.cpp
	g++ -std=c++11 -o serverA serverA.cpp

serverB: serverB.cpp
	g++ -std=c++11 -o serverB serverB.cpp

serverC: serverC.cpp
	g++ -std=c++11 -o serverC serverC.cpp

aws.server: aws.cpp
	g++ -std=c++11 -o aws.server aws.cpp

monitor: monitor.cpp
	g++ -std=c++11 -o monitor monitor.cpp

client: client.cpp
	g++ -std=c++11 -o client client.cpp
