# CS537: xv6-Threads
**[Ankur Singh](https://github.com/Alphamineron) & [Rebecca Brown](https://github.com/rabrown22)**

## TODOs
* ## System Calls
  * clone()
    * **Prototype:** `int clone(void(*fcn)(void *, void *), void *arg1, void *arg2, void *stack)`
    * **Description:** This call creates a new kernel thread which shares the calling process's address space. File descriptors are copied as in `fork()`. The new process uses stack as its user stack, which is passed two arguments (`arg1` and `arg2`) and uses a fake return PC (`0xffffffff`);
    * **Implementation Notes**
      * Use `fork()` in `kernel/proc.c` as template for `clone()`
      * New thread's `np->pgdir` should be the same as the parent's
      * As with `fork()`, the PID of the new thread is returned to the parent (for simplicity, threads each have their own process ID).
      * Set up the kernel stack so that when `clone()` returns in the child (i.e., in the newly created thread) 
        * its run on the user stack passed into clone (stack).
        * the function `fcn` is the starting point of the child thread.
        * the arguments `arg1` and `arg2` are available to that function.
  * join()
    * **Prototype:** `int join(void **stack)`
    * **Description:** This call waits for a child thread that shares the address space with the calling process to exit.
    * **Implementation Notes**
      * It returns the PID of waited-for child or -1 if none.
      * The location of the child's user stack is copied into the argument stack (which can then be freed).
* ## C Library
  * thread_create()
    * **Prototype:** `int thread_create(void (*start_routine)(void *, void *), void *arg1, void *arg2)`
    * **Implementation Notes**
      * This routine should call `malloc()` to create a new user stack
      * use `clone()` to create the child thread
      * It returns the newly created PID to the parent and 0 to the child (if successful), -1 otherwise. 
  * thread_join()
    * **Prototype:** `int thread_join()`
    * **Implementation Notes**
      * It calls the underlying join() system call.
      * frees the user stack
      * then returns the waited-for PID (when successful), -1 otherwise.
  * ## Spin Lock
    The spin lock should use x86 atomic add to build the lock -- see [this wikipedia page](https://en.wikipedia.org/wiki/Fetch-and-add) for a way to create an atomic fetch-and-add routine using the x86 xaddl instruction.
    * lock_acquire()
      * **Prototype:** `void lock_acquire(lock_t *)`
    * lock_release()
      * **Prototype:** `void lock_release(lock_t *)`
    * lock_init()
      * **Prototype:** `void lock_init(lock_t *)`
      * Used to initialize the lock as need be (it should only be called by one thread)
* ## Additional Problems To Attend To:
  * Semantics of a couple of existing system calls
    * `int wait()` should wait for a child process that does not share the address space with this process. It should also free the address space if this is last reference to it.
    * `exit()` should work as before but for both processes and threads; little change is required here.
  * The thread library should be available as part of every program that runs in xv6
    * Add prototypes to `user/user.h`
    * Put actual code to implement the library routines in `user/ulib.c`
  * Growth of Address Space By A Thread
    * for example, when `malloc()` is called, it may call `sbrk` to grow the address space of the process
    * Trace this code path carefully and see where a new lock is needed and what else needs to be updated to grow an address space in a multi-threaded process correctly.
  * A proper thread will simply call `exit()` when it is done (and not return).
  * The stack should be one page in size and page-aligned.
