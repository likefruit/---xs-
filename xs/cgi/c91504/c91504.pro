######################################################################
# Automatically generated by qmake (2.01a) Mon Jun 3 13:51:40 2013
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . ../ ../../src/com

DEFINES += XS_NO_FILE_INFO
DEFINES += XS_LOG_TO_FILE

win32{
LIBS += -lws2_32 -lpthread -lregex
}

# Input
SOURCES += c91504.c \
    ../../src/com/xs_tree.c \
    ../../src/com/xs_stat.c \
    ../../src/com/xs_sort.c \
    ../../src/com/xs_sock.c \
    ../../src/com/xs_server.c \
    ../../src/com/xs_rbtree.c \
    ../../src/com/xs_popen.c \
    ../../src/com/xs_net.c \
    ../../src/com/xs_model.c \
    ../../src/com/xs_mempool.c \
    ../../src/com/xs_md5.c \
    ../../src/com/xs_malloc.c \
    ../../src/com/xs_log.c \
    ../../src/com/xs_ev.c \
    ../../src/com/xs_ctrl.c \
    ../../src/com/xs_config.c \
    ../../src/com/xs_base64.c \
    ../../src/com/xs_aio.c \
    ../../src/com/xs.c

HEADERS += \
    ../../src/com/xs_vec.h \
    ../../src/com/xs_util.h \
    ../../src/com/xs_tree.h \
    ../../src/com/xs_stat.h \
    ../../src/com/xs_sort.h \
    ../../src/com/xs_sock.h \
    ../../src/com/xs_server.h \
    ../../src/com/xs_rbtree.h \
    ../../src/com/xs_popen.h \
    ../../src/com/xs_object.h \
    ../../src/com/xs_net.h \
    ../../src/com/xs_model.h \
    ../../src/com/xs_mempool.h \
    ../../src/com/xs_md5.h \
    ../../src/com/xs_malloc.h \
    ../../src/com/xs_log.h \
    ../../src/com/xs_list.h \
    ../../src/com/xs_itoa.h \
    ../../src/com/xs_heap.h \
    ../../src/com/xs_hash.h \
    ../../src/com/xs_ev.h \
    ../../src/com/xs_dict.h \
    ../../src/com/xs_def.h \
    ../../src/com/xs_ctrl.h \
    ../../src/com/xs_config.h \
    ../../src/com/xs_base64.h \
    ../../src/com/xs_atomic.h \
    ../../src/com/xs_aio.h \
    ../../src/com/xs.h \
    ../cgi.h \
    ../db.h


