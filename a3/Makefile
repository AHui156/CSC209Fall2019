pmake : pmake.o helpers.o parse.o run_make.o
	gcc -Wall -g -o pmake pmake.o helpers.o parse.o run_make.o
	rm pmake.o
	rm helpers.o
	rm parse.o 
	rm run_make.o

pmake.o : pmake.c
	gcc -Wall -g -c pmake.c

helpers.o : helpers.c
	gcc -Wall -g -c helpers.c

parse.o : parse.c 
	gcc -Wall -g -c parse.c

run_make.o : run_make.c
	gcc -Wall -g -c run_make.c

test_target: 
	echo Executing test_target... 

clean : 
	rm pmake *.o