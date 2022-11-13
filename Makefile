.PHONY: all client server clean run

all: clean client server run

run:
	@wt .\bin\Server.exe && wt .\Client\bin\Debug\net6.0\Client.exe

clean: 
	@del .\bin\Server.exe .\Client\bin\Debug\net6.0\Client.exe

client:
	@cd .\Client\ && dotnet build && cd ..

server: .\Server\main.cpp
	@g++ -o .\bin\Server.exe .\Server\main.cpp -lWS2_32