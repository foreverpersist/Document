
struct cfq_rb_root 
	|-struct rb_root rb;
	|-struct rb_node *left;
	|-unsigned count;
	|-u64 min_vdisktime;
	+-struct cfq_ttime ttime;
		|-u64 last_end_request;
		|-u64 ttime_total;
		|-u64 ttime_mean;
		+-unsigned long ttime_samples;

struct cfq_queue 
	|-int ref;
	|-unsigned int flags;
	|-struct cfq_data *cfqd;
	|-struct rb_node rb_node;
	|-u64 rb_key;
	|-struct rb_node p_node;
	|-struct rb_root *p_root;
	|-struct rb_root sort_list;
	|-struct request *next_rq;
	|-int queued[2];
	|-int allocated[2];
	|-struct list_head fifo;
	|-u64 dispatch_start;
	|-u64 allocated_slice;
	|-u64 slice_dispatch;
	|-u64 slice_start;
	|-u64 slice_end;
	|-s64 slice_resid;
	|-int prio_pending;
	|-int dispatched;
	|-unsigned short ioprio, org_ioprio;
	|-unsigned short ioprio_class, org_ioprio_class;
	|-pid_t pid;
	|-u32 seek_history;
	|-sector_t last_request_pos;
	|-struct cfq_rb_root *service_tree;
	|-struct cfq_queue *new_cfqq;
	|-struct cfq_group *cfqg;
	+-unsigned long nr_sectors;


struct cfqg_stats 
	|-struct blkg_rwstat		merged;
	|-struct blkg_rwstat		service_time;
	|-struct blkg_rwstat		wait_time;
	|-struct blkg_rwstat		queued;
	+-struct blkg_stat		time;

struct cfq_group_data 
	|-struct blkcg_policy_data cpd;
	|-unsigned int weight;
	+-unsigned int leaf_weight;

struct cfq_group 
	|-struct blkg_policy_data pd;
	|-struct rb_node rb_node;
	|-u64 vdisktime;
	|-int nr_active;
	|-unsigned int children_weight;
	|-unsigned int vfraction;
	|-unsigned int weight;
	|-unsigned int new_weight;
	|-unsigned int dev_weight;
	|-unsigned int leaf_weight;
	|-unsigned int new_leaf_weight;
	|-unsigned int dev_leaf_weight;
	|-int nr_cfqq;
	|-unsigned int busy_queues_avg[CFQ_PRIO_NR];
	|-struct cfq_rb_root service_trees[2][3];
	|-struct cfq_rb_root service_tree_idle;
	|-u64 saved_wl_slice;
	|-enum wl_type_t saved_wl_type;
	|-enum wl_class_t saved_wl_class;
	|-int dispatched;
	|-struct cfq_ttime ttime;
	|-struct cfqg_stats stats;	/* stats for this cfqg */
	|-struct cfq_queue *async_cfqq[2][IOPRIO_BE_NR];
	+-struct cfq_queue *async_idle_cfqq;

struct cfq_io_cq 
	|-struct io_cq		icq;		/* must be the first member */
	|-struct cfq_queue	*cfqq[2];
	|-struct cfq_ttime	ttime;
	|-int			ioprio;		/* the current ioprio */
	+-uint64_t		blkcg_serial_nr; /* the current blkcg serial */

struct cfq_data 
	|-struct request_queue *queue;
	|-struct cfq_rb_root grp_service_tree;
	|-struct cfq_group *root_group;
	|-enum wl_class_t serving_wl_class;
	|-enum wl_type_t serving_wl_type;
	|-u64 workload_expires;
	|-struct cfq_group *serving_group;
	|-struct rb_root prio_trees[CFQ_PRIO_LISTS];
	|-unsigned int busy_queues;
	|-unsigned int busy_sync_queues;
	|-int rq_in_driver;
	|-int rq_in_flight[2];
	|-int rq_queued;
	|-int hw_tag;
	|-int hw_tag_est_depth;
	|-unsigned int hw_tag_samples;
	|-struct hrtimer idle_slice_timer;
	|-struct work_struct unplug_work;
	|-struct cfq_queue *active_queue;
	|-struct cfq_io_cq *active_cic;
	|-sector_t last_position;
	|-unsigned int cfq_quantum;
	|-unsigned int cfq_back_penalty;
	|-unsigned int cfq_back_max;
	|-unsigned int cfq_slice_async_rq;
	|-unsigned int cfq_latency;
	|-u64 cfq_fifo_expire[2];
	|-u64 cfq_slice[2];
	|-u64 cfq_slice_idle;
	|-u64 cfq_group_idle;
	|-u64 cfq_target_latency;
	|-struct cfq_queue oom_cfqq;
	+-u64 last_delayed_sync;
