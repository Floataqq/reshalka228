# OLD FLAGS FOR GCC

# ded_flags := "-D _DEBUG -ggdb3 -std=c23 -O1 -Wall -Wextra -Waggressive-loop-optimizations -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow	-Wsign-conversion -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-varargs -Wstack-protector -fcheck-new -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=131072 -Wstack-usage=131072 -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr"

# project_options := "-Iinclude/ -lm"

#build:
#  @mkdir -p .build/
#  @echo "Building..."
#  @gcc {{ded_flags}} {{project_options}} src/*.c -o .build/equation_solver 
#  @echo "Done!"

ded_flags := "-D _DEBUG -ggdb3 -std=c++17 -O1 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=131072 -Wstack-usage=131072 -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr"

project_options := "-Iinclude/ -lm"
src_files := "$(find src -type f -name \"*.c\" ! -name \"main.c\")"

build *FLAGS:
  @mkdir -p .build/
  @echo "Building..."
  @g++ {{ded_flags}} {{project_options}} {{FLAGS}} src/*.c -o .build/equation_solver
  @echo "Done!"

clean:
  rm -rf .build

run *ARGS: build
  .build/equation_solver {{ARGS}}

run-ndebug:
  just build "-D NDEBUG"
  .build/equation_solver 

build-tests *EXTRA_FLAGS:
  @mkdir -p .build/
  @echo "Building..."
  @g++ {{ded_flags}} {{EXTRA_FLAGS}} {{project_options}} test/*.c {{src_files}} -o .build/equation_solver_test
  @echo "Done!"

test *ARGS: build-tests
  .build/equation_solver_test {{ARGS}}

docs:
  @mkdir -p .build/docs
  doxygen

serve-docs: docs
  python -m http.server --directory .build/docs/html
