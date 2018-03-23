srcdir := src
hdrdir := src
objdir := bin

sources := $(addprefix $(srcdir)/, $(filter %.c, $(shell ls $(srcdir)) ))
headers := $(addprefix $(hdrdir)/, $(filter %.h, $(shell ls $(srcdir) )))
objects := $(addprefix $(objdir)/, $(patsubst %.c, %.o, $(filter %.c, $(shell ls $(srcdir)))))

run: a.out  clean
	sudo ./a.out /dev/sdc1

tags:
	ctags *

a.out: $(objects)

$(objects) : $(objdir)/%.o : $(srcdir)/%.c $(headers)
	gcc -c $< -I $(hdrdir) -o $@

.PHONY: run clean test tags

clean: 
	@ rm $(objdir)/*.o
