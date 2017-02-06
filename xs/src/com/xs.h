#ifdef __cplusplus
extern "C"{
#endif



#ifndef __XS_H__
#define __XS_H__

#include "xs_net.h"
#include "xs_def.h"
#include "xs_util.h"
#include "xs_log.h"
#include "xs_base64.h"
#include "xs_atomic.h"
#include "xs_list.h"
#include "xs_mempool.h"
#include "xs_malloc.h"
#include "xs_heap.h"
#include "xs_sort.h"
#include "xs_rbtree.h"
#include "xs_popen.h"
#include "xs_stat.h"
#include "xs_itoa.h"
#include "xs_dict.h"
#include "xs_config.h"
#include "xs_tree.h"
#include "xs_ev.h"
#include "xs_sock.h"
#include "xs_aio.h"
#include "xs_hash.h"
#include "xs_object.h"
#include "xs_ctrl.h"
#include "xs_model.h"
#include "xs_server.h"
#include "xs_vec.h"
#include "xs_md5.h"

void xs_init();
void xs_fini();

#endif

#ifdef __cplusplus
}
#endif
