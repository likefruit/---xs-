OBJ = gui.bin
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
DEPANDS = $(SOURCES:.c=.d)

CC = arm-linux-gcc
LIBS = -lpthread -lminigui -lmgext -lpng -ljpeg -lsqlite3
LINK = $(CC) -o $@ 

$(OBJ): $(OBJECTS)
	$(LINK) $(OBJECTS) $(LIBS)

#%.o: %.c
#	@echo '$(CC)-g -c $<'; \
#	$(CC) -g -c $< 

ifneq ($(MAKECMDGOALS),clean) 
-include $(DEPANDS)
endif

%.d: %.c
	@set -e; \
       $(CC) -MM $(CPPFLAGS) $< > $@; \
       sed -i 's#.*\.o#$@#' $@;  \
       sed -i 's#\.d#\.o#' $@;  \

clean:
	rm -rf *.o *~ $(OBJ) *.d
