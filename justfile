build:
  @mkdir -p .build/
  gcc src/*.c -o .build/equation_solver 

clean:
  rm -rf .build

run *ARGS: build
  .build/equation_solver {{ARGS}}

