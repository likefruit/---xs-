
output := mddaemon.bin mdgui.bin test.bin
#output := test.bin

CC := arm-linux-gcc

all: $(output)
	cd mw && qmake
	cd mw && make
	cd mwinit && qmake
	cd mwinit && make 
	arm-linux-strip mddaemon.bin 
	arm-linux-strip mdgui.bin

# include dirs
includir := $(shell find -type d)
excludir := $(shell find  -type d | grep ".git")
excludir += $(shell find  -type d | grep ".svn")
includir := $(filter-out $(excludir),$(includir))
includir := $(addprefix -I,$(includir))

# make deps
sources := $(shell find -name "*.c")
sources += __all_files__.c
objects := $(sources:.c=.o)
depends := $(sources:.c=.d)

# flags and libs
CPPFLAGS += $(dirs) 
CPPFLAGS += -Wall -Wextra -g
LIBS += -lrt -lpthread -lzlog

# com objects for all 
com_objs := $(shell find ./com -name "*.c")
com_objs += __all_files__.c
com_objs := $(com_objs:.c=.o)

# mw com objs
mw_com_objs := $(shell find ./mwcom -name "*.c")
mw_com_objs := $(mw_com_objs:.c=.o)
mw_com_objs += mwlib/mwlib.c

# cli object
cli_objs := $(shell find ./cli -name "*.c")
cli_objs := $(cli_objs:.c=.o)

mddaemon_objs := $(shell find ./md_daemon -name "*.c")
mddaemon_objs := $(mddaemon_objs:.c=.o)

mdclient_objs := $(shell find ./md_client -name "*.c")
mdclient_objs := $(mdclient_objs:.c=.o)

mdgui_objs := $(shell find ./md_gui -name "*.c")
mdgui_objs := $(mdgui_objs:.c=.o)

test_objs := $(shell find ./test -name "*.c")
test_objs := $(test_objs:.c=.o)

# node object
#cluster_objs := $(shell find ./cluster -name "*.c")
#cluster_objs := $(cluster_objs:.c=.o)

# locks object
#locks_objs := $(shell find ./lock -name "*.c")
#locks_objs := $(locks_objs:.c=.o)

ifneq ($(MAKECMDGOALS),clean) 
-include $(depends)
endif

#md_gui/md_gb2312.c: md_gui/md_utf8.txt:
#	iconv -f utf-8 -t gb2312 $^> $@

test.bin: $(com_objs) $(test_objs)
	$(CC) -o $@ $^  -lpthread -lrt #-lzlog 

mdgui.bin: $(com_objs) $(mdgui_objs) $(mw_com_objs)
	$(CC) -o $@ $^ -lpthread -lrt -lminigui -lmgext -lpng -ljpeg #-lsqlite3 

cli.bin: $(com_objs) $(cli_objs)
	$(CC) -o $@ $^  -lpthread -lrt #-lzlog 

cs.bin: $(com_objs) $(chess_srv_objs)
	$(CC) -o $@ $^  -lpthread -lrt #-lzlog

mddaemon.bin: $(com_objs) $(mddaemon_objs) $(mw_com_objs)
	$(CC) -o $@ $^ -lpthread -lrt

daemon.bin: $(com_objs) $(daemon_objs)
	$(CC) -o $@ $^ -lpthread -lrt

mdclient.bin: $(com_objs) $(mdclient_objs)
	$(CC) -o $@ $^ -lpthread -lrt

# for 
allfiles := $(shell find -name "*.c" -o -name "*.h" -o -name "*.cpp"| grep -v "__all_files__.c")
allfile_deps := $(allfiles)
allfiles := $(sort $(allfiles))
allfiles := $(subst ./,,$(allfiles))
allfiles := $(addprefix \",$(allfiles))
allfiles := $(allfiles:=\",)

__all_files__.c: $(allfile_deps) Makefile
	@echo '#ifdef __cplusplus' > $@
	@echo 'extern "C"{' >> $@
	@echo '#endif' >> $@
	@echo 'const char* g_mem_files[] = {' >> $@
	@for filename in $(allfiles); \
		do \
		echo $$filename >> $@; \
		done
	@echo '};' >> $@
	@echo 'int g_mem_file_count=' >> $@
	@echo $(words $(allfiles)) >> $@
	@echo ';' >> $@
	@echo '#ifdef __cplusplus' >> $@
	@echo '}' >> $@
	@echo '#endif' >> $@

# all files end

CPPFLAGS += $(includir)
CPPFLAGS += -g -Wall -Wextra 

%.d: %.c
	@set -e; \
       $(CC) -MM $(CPPFLAGS) $< > $@; \
       sed -i 's#.*\.o#$@#' $@;  \
       sed -i 's#\.d#\.o#' $@;  \

.PHONY: clean
clean:
	rm $(objects) $(depends) *.log *.bin *.so *.d *.o *.tmp *.a -rf 
	cd mw && qmake
	cd mw && make distclean
	cd mwinit && qmake
	cd mwinit && make distclean

.PHONY: install
install:

.PHONY: uninstall
uninstall:

