# Netwerken

# How to run?

Console 1:
start mqtt server by: mosquitto -p [your port], for instance: mosquitto -p 1883

Console 2:
subscribe to see the data the program sends:  mosquitto_sub -p [your port] -t [your topic path], for instance: mosquitto_sub -p 1883 -t text

compile with: gcc *.c -o main -pthread
run with ./main [your topic path] [port of mosquitto], so for example ./main text 1883

