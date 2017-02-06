MINI_OBJECTS = maintest.o listchina.o gprs.o
CC = arm-linux-gcc
LIBS = -lpthread -lminigui -lmgext -lpng -ljpeg -lsqlite3
LINK = $(CC) -o $@ 

maintest: $(MINI_OBJECTS)
	$(LINK) $(MINI_OBJECTS) $(LIBS)

%.o: %.c
	@echo '$(CC)-g -c $<'; \
	$(CC) -g -c $< 

clean:
	rm -rf maintest *.o *~
