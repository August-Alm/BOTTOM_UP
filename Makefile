LINK_TAR = bin/bottomup
LINK_TEST = bin/tests
TEST_FILE = parsetest.lc
SRC = ${wildcard src/*.c}
OBJ = ${patsubst src/%.c, obj/%.o, ${SRC}}

#-std=c11
CFLAGS = -Wall -g -std=c11

obj/%.o: src/%.c
	gcc $(CFLAGS) -o $@ -c $^

$(LINK_TAR): $(OBJ)
	gcc $(CFLAGS) -o $@ $^ main.c

run: ${LINK_TAR}
	./$(LINK_TAR) $(TEST_FILE)

all: clean $(LINK_TAR)
	echo "compiling.."

REBUILDS = $(OBJ) $(LINK_TAR) $(LINK_TEST)

.PHONY clean:
clean:
	rm -f $(REBUILDS)

$(LINK_TEST): clean
	gcc $(CFLAGS) -o $@ ${SRC} test/test_file.c -lcunit

VALGRIND = --leak-check=full --track-origins=yes --show-leak-kinds=all

tests: $(LINK_TEST)
	./$^
#valgrind $(VALGRIND) ./$(LINK_TEST)

memtest: all
	$(VALGRIND) ./$(LINK_TAR)  $(TEST_FILE)

MODE =
debug: all ${LINK_TEST}
ifeq (${MODE}, testing)
	gdb --args ./${LINK_TEST}
else
	gdb --args ./bin/bottomup ${TEST_FILE}
endif