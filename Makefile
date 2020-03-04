build:
	gcc -g tema2.c functiiaux.c -o tema2 -Wall -Wextra
run: build
	./tema2 ${ARGS}
clean:
	rm -rf tema2
