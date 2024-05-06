FLAGS= -Wall -Werror

all : addecho remvocals

addecho : addecho.c
	gcc ${FLAGS} -o addecho.o $^

remvocals : remvocals.c
	gcc ${FLAGS} -o remvocals.o $^

clean :
	-rm *.o
