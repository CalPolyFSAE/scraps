#include <stdlib.h>

void* operator new(size_t objsize) {
 return malloc(objsize);
}

void operator delete(void* obj) {
 free(obj);
}

__extension__ typedef int __guard __attribute__((mode (__DI__)));

extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);
extern "C" void __cxa_pure_virtual(void);

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
void __cxa_guard_abort (__guard *) {};
void __cxa_pure_virtual(void) {};

extern "C" int atexit( void (*func)(void));
extern "C" int __cxa_atexit();

int atexit( void (*func)(void)) {return -1;}
int __cxa_atexit() {return -1;}