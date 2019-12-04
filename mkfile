</$objtype/mkfile

TARG=all

all: day3
	date
day3: main.$O  
	$LD -o day3 main.$O 

main.$O: main.c
	$CC $CFLAGS main.c 

clean:V:
	rm -f *.[$OS] [$OS].out day3

