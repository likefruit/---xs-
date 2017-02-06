RC_FILE = mw.rc

SOURCES += \
    mfmainwnd.cpp \
    ../com/xs_md5.c \
    ../mwcom/md_serial.c \
    ../mwcom/opcard.c \
    dbop/mfsqltablemodel.cpp \
    dbop/mftableitemdelegate.cpp \
    dbop/mftableview.cpp \
    dbop/mftableviewcontainer.cpp \
    dlgs/mfdaccinfo.cpp \
    dlgs/mfdaccoption.cpp \
    reports/mfreport.cpp \
    views/mfviews.cpp \
    dbop/mfsetupsqlconn.cpp \
    dlgs/mfseedinfo.cpp \
    dlgs/mfgdwcopy.cpp
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
../com/xs_stat.h \
    mfmainwnd.h \
    mfview.h \
    mw.h \
    dbop/mfsetupsqlconn.h \
    dbop/mftableview.h \
    dbop/mftableviewcontainer.h \
    mftoolbar.h \
    views/mfworkarea.h \
    dbop/mfsqltablemodel.h \
    dbop/mftableitemdelegate.h \
    views/mffilter.h \
    ../com/xs_vec.h \
    ../com/xs_md5.h \
    mfcarddetect.h \
    dlgs/mfdaccoption.h \
    ../mwcom/opcard.h \
    dlgs/mfdaccinfo.h \
    mfwidget.h \
    ../mwcom/mddef.h \
    reports/mfreport.h \
    views/mfviews.h \
    dlgs/mfseedinfo.h \
    dlgs/mfgdwcopy.h

win32{
LIBS += -lws2_32 -lpthread -lregex
LIBS += -L../mwcom/mt -lmwhrf_bj
LIBS += -L../mwcom/mwlib -lmwlib
LIBS += -ldiskid32
}
!win32{
LIBS += -lrt
SOURCES += ../mwlib/mwlib.c
}
INCLUDEPATH += . ../com dbop views reports dlgs
QT += sql network

INCLUDEPATH += ../mwcom
OTHER_FILES +=

RESOURCES +=

QT += gui widgets printsupport
