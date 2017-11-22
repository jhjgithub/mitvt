VER=0.10-dev
RELEASE=v$(VER)

CC = gcc
CFLAGS = -Wall -ggdb -D_FILE_OFFSET_BITS=64 -I./

CFILES=interval_tree.c rbtree.c
csv_CFILES=text_main.c
test_CFILES=main.c
HEADERS=interval_tree_generic.h  interval_tree.h  rbtree_augmented.h  rbtree.h

objects = $(CFILES:.c=.o)
test_objects = $(test_CFILES:.c=.o)
csv_objects = $(csv_CFILES:.c=.o)
all_objects = $(objects) $(test_objects) $(csv_objects)

#progs = interval-tree-test text_main
progs = interval-tree-test mitvt

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBRARY_FLAGS)

all: $(progs)
#	./interval-tree-test
	./mitvt

$(all_objects): $(HEADERS)

text_main: $(objects) $(csv_objects)
	$(CC) $(CFLAGS) $(objects) $(csv_objects) -o text_main $(LIBRARY_FLAGS)

interval-tree-test: $(objects) $(test_objects)
	$(CC) $(CFLAGS) $(objects) $(test_objects) -o interval-tree-test $(LIBRARY_FLAGS)

mitvt: $(objects) mitvt.o
	$(CC) $(CFLAGS) $(objects) mitvt.o -o mitvt $(LIBRARY_FLAGS)

clean:
	rm -fr $(objects) $(progs)
	rm -f *.o

custom:
	ctags -R

