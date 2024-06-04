# Softirq 和 Worker 的区别

在 Linux 内核中，softirq（软中断）和 worker（工作队列中的工作线程）是两种不同类型的上下文，它们在设计、用途和行为上有显著的区别：

## Softirq 上下文

1. **定义和用途：** Softirq 是一种中断上下文，用于处理较高优先级的任务，通常与硬件中断处理相关。它们是为了减少硬件中断处理的延迟而设计的。
2. **执行环境：** Softirq 在中断上下文中运行，这意味着它们不能睡眠（即不能执行可能会阻塞的操作）。Softirq 需要快速执行，以避免阻塞其他中断。
3. **调度：** Softirq 由内核自动调度，并在处理完硬件中断后或当系统空闲时由 ksoftirqd 内核线程处理。
4. **限制：** 由于运行在中断上下文中，softirq 不能执行任何可能导致阻塞的操作，如等待信号量、睡眠等。

### 代码分析

```c
static void irq_io_loop(struct irq_fd *irq, struct uml_pt_regs *regs)
{
/*
 * irq->active guards against reentry
 * irq->pending accumulates pending requests
 * if pending is raised the irq_handler is re-run
 * until pending is cleared
 */
	if (irq->active) {
		irq->active = false;
		do {
			irq->pending = false;
			do_IRQ(irq->irq, regs);
		} while (irq->pending && (!irq->purge));
		if (!irq->purge)
			irq->active = true;
	} else {
		irq->pending = true;
	}
}
```

```c
/*
 * do_IRQ handles all normal device IRQs (the special
 * SMP cross-CPU interrupts have their own specific
 * handlers).
 */
unsigned int do_IRQ(int irq, struct uml_pt_regs *regs)
{
	struct pt_regs *old_regs = set_irq_regs((struct pt_regs *)regs);
	irq_enter();
	generic_handle_irq(irq);
	irq_exit();
	set_irq_regs(old_regs);
	return 1;
}
```

```c
/**
 * irq_exit - Exit an interrupt context, update RCU and lockdep
 *
 * Also processes softirqs if needed and possible.
 */
void irq_exit(void)
{
	__irq_exit_rcu();
	rcu_irq_exit();
	 /* must be last! */
	lockdep_hardirq_exit();
}
```

```c
/**
 * irq_exit_rcu() - Exit an interrupt context without updating RCU
 *
 * Also processes softirqs if needed and possible.
 */
void irq_exit_rcu(void)
{
	__irq_exit_rcu();
	 /* must be last! */
	lockdep_hardirq_exit();
}
```

```c
static inline void __irq_exit_rcu(void)
{
#ifndef __ARCH_IRQ_EXIT_IRQS_DISABLED
	local_irq_disable();
#else
	lockdep_assert_irqs_disabled();
#endif
	account_hardirq_exit(current);
	preempt_count_sub(HARDIRQ_OFFSET);
	if (!in_interrupt() && local_softirq_pending())
		invoke_softirq();

	tick_irq_exit();
}
```

```c
static inline void invoke_softirq(void)
{
	if (ksoftirqd_running(local_softirq_pending()))
		return;

	if (!force_irqthreads) {
#ifdef CONFIG_HAVE_IRQ_EXIT_ON_IRQ_STACK
		/*
		 * We can safely execute softirq on the current stack if
		 * it is the irq stack, because it should be near empty
		 * at this stage.
		 */
		__do_softirq();
#else
		/*
		 * Otherwise, irq_exit() is called on the task stack that can
		 * be potentially deep already. So call softirq in its own stack
		 * to prevent from any overrun.
		 */
		do_softirq_own_stack();
#endif
	} else {
		wakeup_softirqd();
	}
}

```

```c
asmlinkage __visible void __softirq_entry __do_softirq(void)
{
	unsigned long end = jiffies + MAX_SOFTIRQ_TIME;
	unsigned long old_flags = current->flags;
	int max_restart = MAX_SOFTIRQ_RESTART;
	struct softirq_action *h;
	bool in_hardirq;
	__u32 pending;
	int softirq_bit;

	/*
	 * Mask out PF_MEMALLOC as the current task context is borrowed for the
	 * softirq. A softirq handled, such as network RX, might set PF_MEMALLOC
	 * again if the socket is related to swapping.
	 */
	current->flags &= ~PF_MEMALLOC;

	pending = local_softirq_pending();

	softirq_handle_begin();
	in_hardirq = lockdep_softirq_start();
	account_softirq_enter(current);

restart:
	/* Reset the pending bitmask before enabling irqs */
	set_softirq_pending(0);

	local_irq_enable();

	h = softirq_vec;

	while ((softirq_bit = ffs(pending))) {
		unsigned int vec_nr;
		int prev_count;

		h += softirq_bit - 1;

		vec_nr = h - softirq_vec;
		prev_count = preempt_count();

		kstat_incr_softirqs_this_cpu(vec_nr);

		trace_softirq_entry(vec_nr);
		h->action(h);
		trace_softirq_exit(vec_nr);
		if (unlikely(prev_count != preempt_count())) {
			pr_err("huh, entered softirq %u %s %p with preempt_count %08x, exited with %08x?\n",
			       vec_nr, softirq_to_name[vec_nr], h->action,
			       prev_count, preempt_count());
			preempt_count_set(prev_count);
		}
		h++;
		pending >>= softirq_bit;
	}

	if (!IS_ENABLED(CONFIG_PREEMPT_RT) &&
	    __this_cpu_read(ksoftirqd) == current)
		rcu_softirq_qs();

	local_irq_disable();

	pending = local_softirq_pending();
	if (pending) {
		if (time_before(jiffies, end) && !need_resched() &&
		    --max_restart)
			goto restart;

		wakeup_softirqd();
	}

	account_softirq_exit(current);
	lockdep_softirq_end(in_hardirq);
	softirq_handle_end();
	current_restore_flags(old_flags, PF_MEMALLOC);
}
```

### mutex 路径

```c
static int __sched
__rt_mutex_slowlock(struct rt_mutex *lock, int state,
		    struct hrtimer_sleeper *timeout,
		    struct rt_mutex_waiter *waiter,
		    struct ww_acquire_ctx *ww_ctx)
{
	int ret = 0;

	for (;;) {
		/* Try to acquire the lock: */
		if (try_to_take_rt_mutex(lock, current, waiter))
			break;

		if (timeout && !timeout->task) {
			ret = -ETIMEDOUT;
			break;
		}
		if (signal_pending_state(state, current)) {
			ret = -EINTR;
			break;
		}

		if (ww_ctx && ww_ctx->acquired > 0) {
			ret = __mutex_lock_check_stamp(lock, ww_ctx);
			if (ret)
				break;
		}

		raw_spin_unlock_irq(&lock->wait_lock);

		schedule();

		raw_spin_lock_irq(&lock->wait_lock);
		set_current_state(state);
	}

	__set_current_state(TASK_RUNNING);
	return ret;
}

```



```
asmlinkage __visible void __sched schedule(void)
{
	struct task_struct *tsk = current;

	sched_submit_work(tsk);
	do {
		preempt_disable();
		__schedule(false, false);
		sched_preempt_enable_no_resched();
	} while (need_resched());
	sched_update_worker(tsk);
}
EXPORT_SYMBOL(schedule);
```

```
static void __sched notrace __schedule(bool preempt, bool spinning_lock)
{
	struct task_struct *prev, *next;
	unsigned long *switch_count;
	unsigned long prev_state;
	struct rq_flags rf;
	struct rq *rq;
	int cpu;

	cpu = smp_processor_id();
	rq = cpu_rq(cpu);
	prev = rq->curr;

	schedule_debug(prev, preempt);

	if (sched_feat(HRTICK))
		hrtick_clear(rq);

	local_irq_disable();
	rcu_note_context_switch(preempt);

	/*
	 * Make sure that signal_pending_state()->signal_pending() below
	 * can't be reordered with __set_current_state(TASK_INTERRUPTIBLE)
	 * done by the caller to avoid the race with signal_wake_up():
	 *
	 * __set_current_state(@state)		signal_wake_up()
	 * schedule()				  set_tsk_thread_flag(p, TIF_SIGPENDING)
	 *					  wake_up_state(p, state)
	 *   LOCK rq->lock			    LOCK p->pi_state
	 *   smp_mb__after_spinlock()		    smp_mb__after_spinlock()
	 *     if (signal_pending_state())	    if (p->state & @state)
	 *
	 * Also, the membarrier system call requires a full memory barrier
	 * after coming from user-space, before storing to rq->curr.
	 */
	rq_lock(rq, &rf);
	smp_mb__after_spinlock();
```

```c
void rcu_note_context_switch(bool preempt)
{
	struct task_struct *t = current;
	struct rcu_data *rdp = this_cpu_ptr(&rcu_data);
	struct rcu_node *rnp;

	trace_rcu_utilization(TPS("Start context switch"));
	lockdep_assert_irqs_disabled();
	WARN_ON_ONCE(!preempt && rcu_preempt_depth() > 0);
	if (rcu_preempt_depth() > 0 &&
	    !t->rcu_read_unlock_special.b.blocked) {

		/* Possibly blocking in an RCU read-side critical section. */
		rnp = rdp->mynode;
		raw_spin_lock_rcu_node(rnp);
		t->rcu_read_unlock_special.b.blocked = true;
		t->rcu_blocked_node = rnp;

		/*
		 * Verify the CPU's sanity, trace the preemption, and
		 * then queue the task as required based on the states
		 * of any ongoing and expedited grace periods.
		 */
		WARN_ON_ONCE((rdp->grpmask & rcu_rnp_online_cpus(rnp)) == 0);
		WARN_ON_ONCE(!list_empty(&t->rcu_node_entry));
		trace_rcu_preempt_task(rcu_state.name,
				       t->pid,
				       (rnp->qsmask & rdp->grpmask)
				       ? rnp->gp_seq
				       : rcu_seq_snap(&rnp->gp_seq));
		rcu_preempt_ctxt_queue(rnp, rdp);
	} else {
		rcu_preempt_deferred_qs(t);
	}
```



## Worker 上下文

1. **定义和用途：** Worker 是工作队列的一部分，用于执行可能需要较长时间或需要能够睡眠的任务。工作队列是一种机制，允许将任务推迟到将来某个时刻异步执行。
2. **执行环境：** Worker 在进程上下文中运行，这意味着它们可以睡眠。因此，worker 可以执行文件操作、等待 I/O 完成、获取互斥锁等操作。
3. **调度：** Worker 由内核中的工作队列机制管理，通常在用户空间进程不活跃时执行。它们由特定的内核线程（例如 kworker）调度执行。
4. **灵活性：** Worker 提供了执行长时间运行或需要阻塞操作的任务的能力，这在 softirq 中是不允许的。

## 主要区别

**阻塞能力：** Softirq 不能睡眠或执行阻塞操作，而 worker 可以。

**调度和优先级：** Softirq 通常具有比常规进程和 worker 更高的执行优先级，因为它们处理的是与中断相关的任务。

**用途：** Softirq 适用于快速、非阻塞性的中断驱动任务，而 worker 适用于可能需要更长时间或需要阻塞的后台任务。

这些区别使得 softirq 和 worker 各自适合于不同类型的任务，根据任务的特性和需求选择合适的执行机制是很重要的。