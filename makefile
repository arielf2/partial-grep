objects = IO.o StringSearch.o
CFLAGS = -g -std=gnu99

my_grep : $(objects)
	cc -o my_grep $(objects)

IO.o : StringSearch.h
StringSearch.o : StringSearch.h


.PHONY : clean
clean:
	-rm my_grep $(objects)
