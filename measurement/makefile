objects=clock.o 
clock_obj=clock.o
pipe_obj=pipe.o clock.o
all_obj=clock.o
CC=gcc
FLAGS=-pthread
clock_cpp.o : clock.cpp clock.hpp
		$(CC) $(FLAGS) -c clock.cpp
clock_cpp : clock_cpp.o
		$(CC) $(FLAGS) -o clock_cpp clock_cpp.o

clock.o : clock.c clock.h
		$(CC) $(FLAGS) -c clock.c
clock : $(clock_obj)
		$(CC) $(FLAGS) -o clock $(clock_obj)

pipe.o : clock.c clock.h pipe.c
		$(CC) $(FLAGS) -c pipe.c
pipe : $(pipe_obj)
		$(CC) $(FLAGS) -o pipe $(pipe_obj)
socket: server.c client.c
		$(CC) $(FLAGS) client.c -o client clock.o
		$(CC) $(FLAGS) server.c -o server clock.o

precision.o: clock.c clock.h precision.c
		$(CC) $(FLAGS) -c precision.c
precision: precision.o clock.o
		$(CC) $(FLAGS) -o precision precision.o clock.o

shared: shared_mem.c
		$(CC) $(FLAGS) shared_mem.c -o shared clock.o