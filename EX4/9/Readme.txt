Server:
compile: make server
run: ./server

Client:
compile: make client
run: ./client

clean 5
add 0 1
add 1 2
add 2 3
add 3 4
add 4 0
activate

random 17 4 6
activate


clean 4
addw 0 1 10
addw 0 2 6
addw 0 3 5
addw 1 3 15
addw 2 3 4
activate


exit



