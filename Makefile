.PHONY: clean
build:
	mkdir -p .build
	gcc $(CFLAGS) src/*.c -o .build/equation_solver

.PHONY: clean
run: build
	.build/equation_solver

.PHONY: clean
clean:
	rm -rf .build

