#ifndef _PROCESS_H
#define _PROCESS_H

#include <sys/file.h>
#include <sys/idt.h>
#include <sys/kern.h>
#include <sys/page_alloc.h>

#define USR_STACK_SZ 512
#define STACK_SIZE 256 
#define SCHEDULE_POLICY RR
#define NUM_PROCS 256
#define NUM_PCB 32

extern uint16_t p_id, ready_procs, proc_count_in_list;
extern uint16_t upid;
extern uint64_t ready_queue[NUM_PROCS];
extern uint16_t process_count;
extern char curr_file[NAMEMAX];

/*
 * index for current user pcb for ready_queue
 */
extern uint16_t idx_pcb;

struct PCB *processes;
extern uint32_t child_pid;

/*
 * These have the current kernel threads and 
 * processes
 */
struct PCB *curr_pcb;
struct task_struct *curr_upcb;

typedef enum 
{ 
	WAIT, 
	SLEEPING, 
	READY, 
	RUNNING, 
	DEAD,	
	ZOMBIE 
} proc_state_t;

struct context
{
	uint64_t rip, rsp;
};

struct vm_area_struct
{
	struct mm_struct *vm_mm;       /* associated mm_struct */
	uint64_t vm_start;              /* VMA start, inclusive */
	uint64_t vm_end;                /* VMA end, exclusive */
	uint64_t vm_mmsz;               /* VMA size */ 
	unsigned long vm_flags;         /* flags */
	uint32_t grows_down;
	uint64_t vm_file;          /* mapped file, if any */
	struct vm_area_struct *vm_next;/* list of VMA's */ 
	uint64_t vm_offset;    /* file offset */  
};

struct mm_struct 
{
	struct vm_area_struct *mmap;
	struct vm_area_struct *mmap_avl;
	int count;
	uint64_t *pt; // page table pointer  
	uint64_t context;
	uint64_t start_code, end_code, start_data, end_data;
	uint64_t start_brk, brk, start_stack, start_mmap;
	uint64_t arg_start, arg_end, env_start, env_end;
	uint64_t rss, total_vm, locked_vm;
	uint64_t def_flags;
};

struct vm_area_struct 
*malloc_vma(
			struct mm_struct *mm
		   );

/*
 * user PCB structure
 */
struct task_struct
{
	uint64_t kstack[USR_STACK_SZ];   
	proc_state_t state;
	struct context ctx;
	uint64_t counter;	//how long its run;
	uint64_t pid;		//my id;
	uint64_t ppid;		//my parent pid   
	uint64_t wait_pid;	//wait pid of the child; 0 if not waiting on the child
	uint64_t priority;	// when should it run
	uint64_t *stack;	// its own stack 
	uint64_t pml4e;		// will directly go to cr3 register 
	uint64_t cr3;
	uint64_t *bmpPtr;	// user stack of the process
	int exit_status; 	// exit status
	int sleep_time;
	uint64_t entry;
	char pname[100];	//process name
	struct mm_struct *mm;	// a pointer to mm_struct
	struct vm_area_struct *heap_vma;	// vma for heap
};

struct task_struct zombie_queue[NUM_PROCS];

/*
 * kernel PCB structure
 */
struct PCB 
{
	uint64_t kstack[STACK_SIZE];
	struct context c_t;
	uint64_t pid;
	proc_state_t proc_state;
	int exit_status;
	struct PCB *next;
};

void
dispatch(
		);

struct task_struct *
init_usr_pcb(
         	 char *file_name
        	);

uint64_t
create_usr_pcb(
			   char *file
              );

struct PCB*
init_pcb(
         void (*t)()
        );

int
create_pcb(
          );

void
thread2(
	   );

void
thread1(
	   );

int 
fn_call(
	   );

int
schedule(
         struct PCB *curr,
         struct PCB *next
        );

void
yield(
     );

struct vm_area_struct *
malloc_vma(
           struct mm_struct *mm
          );

void
allocate_regions(
                 void *va,
                 size_t len
                );

void
load_segment(
             struct task_struct *pcb,
             char *file,
             size_t sz
            );

int
fork_process(
            );

void
exit_process(
     		 int state
    		);

uint64_t
_sleep(
      );

uint64_t
sleep_proc(
      	   uint64_t time
     	  );

void*
malloc(
       uint64_t size
      );

uint64_t
exec(
     char *arg1,
     char **arg2,
     char **arg3
    );

uint64_t
get_pid(
       );

uint64_t
get_ppid(
        );

int
syswaitpid(
           int pid,
           int status
       	  );

void
dp(
  );

void
kshutdown(
         );

void
move_to_end(
			struct PCB *pcb
		   );

void
add_to_ready_list(
                  struct PCB *pcb
                 );

void
add_to_ready_list_user(
                       struct task_struct *pcb
                      );

void
move_to_next_pcb(
                );

void
move_to_next(
			);
#endif
