#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>

#include "interval_tree.h"

#define	printk	printf
#define	KERN_ALERT	""
#define	u32	uint32_t
#define	cycles_t	time_t
#define	get_cycles()	time(NULL)
#define	div_u64(numerator, denominator)	(numerator / denominator)
#define	prandom_seed_state(arg1, arg2)	srand((unsigned int)arg2)
#define	prandom_u32_state(arg1)	rand()
struct rnd_state {
	unsigned long	val;
};

//#define NODES        100
#define NODES        4
//#define PERF_LOOPS   100000
#define PERF_LOOPS   1
//#define SEARCHES     100
#define SEARCHES     4
//#define SEARCH_LOOPS 10000
#define SEARCH_LOOPS 1

static struct rb_root g_root = RB_ROOT;
static struct interval_tree_node nodes[NODES];
static itvt_val_t queries[SEARCHES];

static struct rnd_state rnd;

//////////////////////////////////////////////

void uint128_to_str_iter(uint128_t n, char *out,int firstiter)
{
	static int offset=0;
	if (firstiter){
		offset=0;
	}
	if (n == 0) {
		return;
	}
	uint128_to_str_iter(n/10,out,0);
	out[offset++]=n%10+0x30;
}

char* uint128_to_str(uint128_t n)
{
	char *out=calloc(sizeof(char),40);

	if (n == 0) {
		out[0] = '0';
		out[1] = 0;

		return out;
	}

	uint128_to_str_iter(n, out, 1);

	return out;
}

// interval_tree_generic.h

static inline unsigned long
search(itvt_val_t query, struct rb_root *root)
{
	struct interval_tree_node *node;
	unsigned long results = 0;

	for (node = interval_tree_iter_first(root, query, query); node;
		 node = interval_tree_iter_next(node, query, query)) {
		results++;
		
		char *s = uint128_to_str(node->start);
		char *e = uint128_to_str(node->last);

		//printf("node: %lu - %lu \n", node->start, node->last);

		printf("node(%u): %s - %s \n", node->idx, s, e);
		free(s);
		free(e);
	}

	return results;
}

static void init(void)
{
#if 0
	int i;
	for (i = 0; i < NODES; i++) {
		u32 a = prandom_u32_state(&rnd);
		u32 b = prandom_u32_state(&rnd);
		if (a <= b) {
			nodes[i].start = a;
			nodes[i].last = b;
		} else {
			nodes[i].start = b;
			nodes[i].last = a;
		}
	}

	for (i = 0; i < SEARCHES; i++)
		queries[i] = prandom_u32_state(&rnd);
#else
	nodes[0].start = 0;
	nodes[0].last   = 100;
	nodes[0].idx   = 1;
	nodes[1].start = 10;
	nodes[1].last   = 90;
	nodes[1].idx   = 2;
	nodes[2].start = 30;
	nodes[2].last   = 95;
	nodes[2].idx   = 3;
	nodes[3].start = 20;
	nodes[3].last   = 50;
	nodes[3].idx   = 4;

	queries[0] = 45;
	queries[1] = 20;
	queries[2] = 90;
	queries[3] = 60;
#endif
}

#if 0
void interval_tree_destroy(struct rb_root *root)
{
	struct interval_tree_node *node;

	while (1) {
		node = rb_entry(rb_first(root), struct interval_tree_node, rb);
		if (!node) {
			break;
		}

		interval_tree_remove(node, root);

		printf("D node(%u): %d - %d \n", node->idx, node->start, node->last);
	}

}
#endif

void free_node(struct interval_tree_node *node)
{
	printf("D node(%u): %d - %d \n", node->idx, node->start, node->last);
}

int dump_interval_tree(struct rb_root *root)
{
	struct interval_tree_node *node, *n;

	interval_tree_for_each_entry(node, root) {
		printf("node(%u): %d - %d \n", node->idx, node->start, node->last);
	}

	interval_tree_post_for_each_entry(node, n, root) {
		printf("R node(%u): %d - %d \n", node->idx, node->start, node->last);
	}

	interval_tree_destroy(root, free_node);

	return 0;
}


static int interval_tree_test_init(void)
{
	int i, j;
	unsigned long results, n;
	cycles_t time1, time2, _time;

	printk(KERN_ALERT "Interval tree insert/remove");

	prandom_seed_state(&rnd, 3141592653589793238ULL);
	init();

	time1 = get_cycles();

	for (i = 0; i < PERF_LOOPS; i++) {
		for (j = 0; j < NODES; j++)
			interval_tree_insert(nodes + j, &g_root);
		for (j = 0; j < NODES; j++)
			interval_tree_remove(nodes + j, &g_root);
	}

	time2 = get_cycles();
	_time = time2 - time1;

	_time = div_u64(_time, PERF_LOOPS);
	printk(" -> %llu cycles\n", (unsigned long long)time);


	for (j = 0; j < NODES; j++)
		interval_tree_insert(nodes + j, &g_root);

	time1 = get_cycles();

	results = 0;
	for (i = 0; i < SEARCH_LOOPS; i++) {
		for (j = 0; j < SEARCHES; j++) {
			n = interval_tree_search(queries[j], &g_root);
			char *s = uint128_to_str(queries[j]);
			printf("Value=%s, matched=%lu \n", s, n);

			free(s);

			results += n;
		}
	}

	time2 = get_cycles();
	_time = time2 - time1;

	_time = div_u64(_time, SEARCH_LOOPS);
	results = div_u64(results, SEARCH_LOOPS);
	printk(KERN_ALERT "interval tree search\n");
	printk(" -> %llu cycles (%lu results)\n",
	       (unsigned long long)time, results);

	dump_interval_tree(&g_root);

	return -EAGAIN; /* Fail will directly unload the module */
}

static void interval_tree_test_exit(void)
{
	printk(KERN_ALERT "test exit\n");
}

int main(int argc, char **argv)
{
	interval_tree_test_init();
	interval_tree_test_exit();
	return 0;
}
