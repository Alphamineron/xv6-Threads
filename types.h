typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;

typedef struct __lock_t {
    uint value;
} lock_t;

typedef struct ptr_struct {
  int busy;
  void *stack;
  void *ptr;
} thr_table;