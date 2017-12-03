#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#include "interval_tree.h"
#include "interval_tree_generic.h"

#define START(node) ((node)->start)
#define LAST(node)  ((node)->last)

#if 0
INTERVAL_TREE_DEFINE(struct interval_tree_node, rb,
		     unsigned long, __subtree_last,
		     START, LAST,, interval_tree)
#else
INTERVAL_TREE_DEFINE(struct interval_tree_node, rb,
		     itvt_val_t, __subtree_last,
		     START, LAST,, interval_tree)
#endif

uint32_t interval_tree_search(itvt_val_t query, struct rb_root *root)
{
	struct interval_tree_node *node;
	uint32_t results = 0;

	for (node = interval_tree_iter_first(root, query, query); node;
		 node = interval_tree_iter_next(node, query, query)) {
		results++;
	}

	return results;
}

void interval_tree_destroy(struct rb_root *root, interval_tree_del_node_cb cb)
{
	struct interval_tree_node *node;

	while (1) {
		node = rb_entry(rb_first(root), struct interval_tree_node, rb);
		if (!node) {
			break;
		}

		interval_tree_remove(node, root);

		if (cb) {
			cb(node);
		}
	}
}

