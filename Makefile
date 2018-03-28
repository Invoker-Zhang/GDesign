srcdir := src
hdrdir := src
objdir := bin

sources := $(addprefix $(srcdir)/, $(filter %.c, $(shell ls $(srcdir)) ))
headers := $(addprefix $(hdrdir)/, $(filter %.h, $(shell ls $(srcdir) )))
objects := $(addprefix $(objdir)/, $(patsubst %.c, %.o, $(filter %.c, $(shell ls $(srcdir)))))

run: a.out  clean
	sudo ./a.out /dev/sdb1

tags:
	ctags src/* 

a.out: $(objects)
	gcc -g -o a.out $^

$(objects) : $(objdir)/%.o : $(srcdir)/%.c $(headers)
	gcc -g -c $< -I $(hdrdir) -o $@ -D TEST_EXT2 -DDEBUG

.PHONY: run clean test tags

clean: 
	@ rm $(objdir)/*.o
