LINK_TAR = bin/bottomup
#LINK_TEST = bin/tests
TEST_FILE = parsetest.lc

OBJ =	\
	obj/leaf.o\
	obj/single.o\
	obj/branch.o\
	obj/node.o\
	obj/heap.o\
	obj/uplink.o\
	obj/downclean.o\
	obj/reduce.o\

#-std=c11 
CFLAGS = -Wall -g -std=c11

run: all
	./$(LINK_TAR) $(TEST_FILE)

all: clean $(LINK_TAR)
	echo "compiling.."

REBUILDS = $(OBJ) $(LINK_TAR) #$(LINK_TEST)

.PHONY clean:
clean:
	rm -f $(REBUILDS)

$(LINK_TAR): $(OBJ)
	gcc $(CFLAGS) -o $@ $^ main.c

#$(LINK_TEST): $(OBJ)
#	gcc $(CFLAGS) -o $@ $^ test/test.c -lcunit

obj/%.o: src/%.c
	gcc $(CFLAGS) -o $@ -c $<

VALGRIND = valgrind --leak-check=full --track-origins=yes

#tests: clean $(LINK_TEST)
#	$(VALGRIND) ./$(LINK_TEST)

memtest: all
	$(VALGRIND) ./$(LINK_TAR)  $(TEST_FILE)

debug: all
	gdb --args ./bin/bottomup ${TEST_FILE}
