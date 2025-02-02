#include <stdlib.h>

// Variables declared outside any function are global in scope
// These variables are stored in the initalized data segment within static memory
// The below expression reserves 5MB of space
// This memory is de-allocated when the program terminates
// One char : 1 byte in memory
char global_variable[5242880];

int main()
{
	char *heap_variable;

	// Values instantiated within a function call are added to the stack
	// This memory is de-allocated when the callee terminates 
	char stack_variable[1048576];

	// Dynamically-allocated memory reserves space on the heap during runtime
	// This expression returns a pointer to heap memory
	// The argument passed to malloc indicates the size reserved in bytes, or 10MB
	heap_variable = (char*)malloc(10485760);
	
	// Heap data needs to be deliberately de-allocated by the developer
	free(heap_variable);
}
