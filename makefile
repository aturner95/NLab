CC := clang
CFLAGS := -Wall -Wextra -Wpedantic -Wfloat-equal -Wvla -std=c99 -Werror
SANITIZE := -fsanitize=undefined -fsanitize=address
NLBS := $(wildcard *.nlb)
RESULTS := $(NLBS:.nlb=.result)

## all: parse parse_s parse_v test_parse test_parse_s test_parse_v interp interp_s interp_v test_interp test_interp_s test_interp_v

# <-- parse -->
## production
parse: src/nlab.h src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c
	$(CC) src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c ${CFLAGS} -O2 -o parse -lm

parse_s: src/nlab.h src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c
	$(CC) src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c ${CFLAGS} ${SANITIZE} -g3 -o parse_s -lm

parse_v: src/nlab.h src/nlab.c src/prog_builder.c  src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c
	$(CC) src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c ${CFLAGS} -g3 -o parse_v -lm

## test
test_parse: src/nlab.h src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c
	$(CC) src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c ${CFLAGS} -O2 -o test_parse -lm -DTESTMODE

test_parse_s: src/nlab.h src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c
	$(CC) src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c ${CFLAGS} ${SANITIZE} -g3 -o test_parse_s -lm -DTESTMODE

test_parse_v: src/nlab.h src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c
	$(CC) src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c ${CFLAGS} -g3 -o test_parse_v -lm -DTESTMODE

# <-- interp -->
## production
interp: src/nlab.h src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c
	$(CC) src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c ${CFLAGS} -O2 -DINTERP -o interp -lm

interp_s: src/nlab.h src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c 
	$(CC) src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c  ${CFLAGS} ${SANITIZE} -g3 -DINTERP -o interp_s -lm

interp_v: src/nlab.h src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c 
	$(CC) src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c ${CFLAGS} -g3 -DINTERP -o interp_v -lm

## test
test_interp: src/nlab.h src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c
	$(CC) src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c ${CFLAGS} -O2 -DINTERP -o test_interp -lm -DTESTMODE

test_interp_s: src/nlab.h src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c
	$(CC) src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c ${CFLAGS} ${SANITIZE} -g3 -DINTERP -o test_interp_s -lm -DTESTMODE

test_interp_v: src/nlab.h src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c
	$(CC) src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c ${CFLAGS} -DINTERP -g3 -o test_interp_v -lm -DTESTMODE


# <-- exntension -->
## production
extension: src/nlab.h src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c
	$(CC) src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c ${CFLAGS} -O2 -DINTERP -DEXTENSION -o extension -lm

extension_s: src/nlab.h src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c 
	$(CC) src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c  ${CFLAGS} ${SANITIZE} -g3 -DINTERP -DEXTENSION -o extension_s -lm

extension_v: src/nlab.h src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c 
	$(CC) src/nlab.c src/prog_builder.c src/stack/realloc.c src/map/map.c src/nlab_array/nlab_array.c ${CFLAGS} -g3 -DINTERP -DEXTENSION -o extension_v -lm

## test
test_extension: src/nlab.h src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c
	$(CC) src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c ${CFLAGS} -O2 -DINTERP -DEXTENSION -o test_extension -lm -DTESTMODE

test_extension_s: src/nlab.h src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c
	$(CC) src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c ${CFLAGS} ${SANITIZE} -g3 -DINTERP -DEXTENSION -o test_extension_s -lm -DTESTMODE

test_extension_v: src/nlab.h src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c
	$(CC) src/nlab.c src/prog_builder.c test/test_nlab.c src/stack/realloc.c test/test_stack.c src/map/map.c test/test_map.c src/nlab_array/nlab_array.c test/test_nlab_array.c ${CFLAGS} -DINTERP -DEXTENSION -g3 -o test_extension_v -lm -DTESTMODE

## runall: $(RESULTS)

##%.result:
##	./interp $*.nlb > $*.results

clean:
	rm -f parse parse_s parse_v test_parse test_parse_s test_parse_v interp interp_s interp_v test_interp test_interp_s test_interp_v extension test_extension extension_s test_extension_s extension_v test_extension_v $(RESULTS)