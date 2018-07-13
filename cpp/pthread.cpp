#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <iostream>
#include <typeinfo>

int global;

void* thread_function(void* arg);

int main(void)
{
	int local;
	int *dynamic;
	dynamic = (int *)malloc(1383);

	printf("Local = %p\n", &local);       //stack
	printf("Dynamic = %p\n", dynamic);    //heap
	printf("Global = %p\n", &global);     //data
	printf("main = %p\n", main);          //text

	int pid = fork();
	printf("process pid: %d\n", pid);

	std::cout << typeid(pthread_t).name() << std::endl;
	printf("\n\n");
	{
		pthread_t p_thread;
		int create_result = pthread_create(&p_thread, NULL, thread_function, NULL);
		printf("======>TID(%ld)\n", p_thread);
	}
	{
		pthread_t p_thread;
		int create_result = pthread_create(&p_thread, NULL, thread_function, NULL);
		printf("======>TID(%ld)\n", p_thread);
	}
	{
		pthread_t p_thread;
		int create_result = pthread_create(&p_thread, NULL, thread_function, NULL);
		printf("======>TID(%ld)\n", p_thread);
		pthread_join(p_thread, NULL);
	}

	return 0;
}

void* thread_function(void* arg){
	printf("->PID(%d) >>> TID(%ld, %ld)\n", getpid(), pthread_self(), syscall(SYS_gettid));
	while(1);
}

/*
   fork(), vfork(), clone(), pthread_create() -> sys_clone(), sys_vfork(), sys_fork() -> do_fork() -> kernel_thread()

 */
