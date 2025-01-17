#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}

// ========================
// THREADING LIBRARY CODE
// ========================

void lock_acquire(lock_t* lock) {
  while(xchg(&lock->value, 1) != 0)
  ;
}

void lock_release(lock_t* lock) {
  lock->value = 0;
}

void lock_init(lock_t* lock) {
  lock->value = 0;
}



thr_table pointers[64];
// max process size = 64

int thread_create(void (*start_routine)(void *, void *), void *arg1, void *arg2) {
  // max process size = 64
  // page size = 4096
  void* ptr = malloc(4096 * 2);

  // validate pointer
  if (ptr == 0){
    return -1;
  }

  // stack to be used for thread
  void* stack;

  // pointer should be multiple of 4096
  if ((uint)ptr % 4096 != 0){
    // stack starts at recalculated pointer
    stack = ptr+(4096 - ((uint)ptr % 4096));
  } else{
    // stack starts at pointer
    stack = ptr;
  }

  // set pointers as used if not currently used
  for (int i = 0; i < 64; i++){
    if(pointers[i].busy == 0){
      pointers[i].ptr = ptr;
      pointers[i].stack = stack;
      pointers[i].busy = 1;
      break;
    }
  }
  // return int from clone()
  return clone(start_routine, arg1, arg2, stack);
}

int thread_join() {
  // max process size = 64
  void* stack;
  // to be returned
  int stack_addr = join(&stack);

  for(int i = 0; i < 64; i++){
    // free pointers struct
    if(pointers[i].stack == stack && pointers[i].busy == 1) {
      // free memory
      free(pointers[i].ptr);
      // not busy
      pointers[i].busy = 0;
      // set pointer to null
      pointers[i].ptr = 0;
      // set stack to null
      pointers[i].stack = 0;
      break;
    }
  }

  return stack_addr;
}