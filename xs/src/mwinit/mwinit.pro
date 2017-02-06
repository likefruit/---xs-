#-------------------------------------------------
#
# Project created by QtCreator 2013-03-15T10:26:52
#
#-------------------------------------------------

QT       += core

#QT       -= gui

TARGET = mwinit
CONFIG   += console
#CONFIG   -= app_bundle

TEMPLATE = app

!win32{
LIBS += -lrt
SOURCES += ../mwlib/mwlib.c
}
win32{
LIBS += -L../mwcom/mt -lmwhrf_bj
LIBS += -lws2_32 -lpthread -lregex
LIBS += -L../mwcom/mwlib -lmwlib
}
INCLUDEPATH += . ../com ../mwcom
SOURCES += main.cpp \
    ../mwcom/opcard.c

SOURCES += \
../com/xs_config.c \
../com/xs_malloc.c \
../com/xs_popen.c \
../com/xs_base64.c \
../com/xs_stat.c \
../com/xs_mempool.c \
../com/xs_sort.c \
../com/xs_sock.c \
../com/xs_ctrl.c \
../com/xs_aio.c \
../com/xs_server.c \
../com/xs_ev.c \
../com/xs_log.c \
../com/xs_rbtree.c \
../com/xs.c \
../com/xs_model.c \
../com/xs_tree.c \
../com/xs_net.c \
../__all_files__.c

HEADERS += \
../com/xs_itoa.h \
../com/xs_rbtree.h \
../com/xs_tree.h \
../com/xs_def.h \
../com/xs_dict.h \
../com/xs_ctrl.h \
../com/xs_base64.h \
../com/xs.h \
../com/xs_server.h \
../com/xs_heap.h \
../com/xs_atomic.h \
../com/xs_sort.h \
../com/xs_popen.h \
../com/xs_malloc.h \
../com/xs_object.h \
../com/xs_model.h \
../com/xs_config.h \
../com/xs_mempool.h \
../com/xs_log.h \
../com/xs_ev.h \
../com/xs_list.h \
../com/xs_hash.h \
../com/xs_aio.h \
../com/xs_util.h \
../com/xs_sock.h \
../com/xs_net.h \
../com/xs_stat.h
