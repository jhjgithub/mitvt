#ifndef _LINUX_INTERVAL_TREE_H
#define _LINUX_INTERVAL_TREE_H

#include <stdint.h>

#include "rbtree.h"

typedef __uint128_t uint128_t;
typedef __int128    int128_t;

typedef uint32_t 	itvt_val_t;
//typedef uint64_t 	itvt_val_t;
//typedef uint128_t itvt_val_t;

typedef struct interval_tree_node {
	struct rb_node rb;
#if 0
	unsigned long start;	/* Start of interval */
	unsigned long last;	/* Last location _in_ interval */
	unsigned long __subtree_last;
#else
	itvt_val_t start;
	itvt_val_t last;
	itvt_val_t __subtree_last;
	uint32_t 	idx;
#endif

} itvt_node_t;

typedef void 	(*interval_tree_del_node_cb) (struct interval_tree_node*);

#define interval_tree_for_each_entry(pos, root) \
		for (pos = rb_entry(rb_first(root), typeof(*pos), rb);  \
			 pos != NULL /*&& (tpos = rb_entry(pos, typeof(*tpos), rb))*/; \
			 pos = rb_entry(rb_next(&pos->rb), typeof(*pos), rb) )


#define interval_tree_post_for_each_entry(pos, n, root) \
	rbtree_postorder_for_each_entry_safe(pos, n, root, rb)

extern void
interval_tree_insert(struct interval_tree_node *node, struct rb_root *root);

extern void
interval_tree_remove(struct interval_tree_node *node, struct rb_root *root);

extern struct interval_tree_node *
interval_tree_iter_first(struct rb_root *root, itvt_val_t start, itvt_val_t last);

extern struct interval_tree_node *
interval_tree_iter_next(struct interval_tree_node *node, itvt_val_t start, itvt_val_t last);

extern uint32_t interval_tree_search(itvt_val_t query, struct rb_root *root);
extern void interval_tree_destroy(struct rb_root *root, interval_tree_del_node_cb cb);

#endif	/* _LINUX_INTERVAL_TREE_H */
