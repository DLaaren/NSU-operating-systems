#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "hello_static.h"
#include "hello_dynamic.h"

void hello_from_dyn_runtime_lib_func() {
	//linking .so lib
	void *hDl = dlopen("hello_from_dyn_runtime.so", RTLD_LAZY); //means the symbol resolves only as the code is executed

	if (hDl == NULL) {
		fprintf(stderr, "dlopen: %s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	//asking for the address of the function with needed name from .so
	void (*func)() = dlsym(hDl, "hello_from_dyn_runtime_lib");
	if (func == NULL) {
		fprintf(stderr, "dlsym: %s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	func();

	int err = dlclose(hDl);
	if (err != 0) {
		fprintf(stderr, "dlclose: %s\n", dlerror());
		exit(EXIT_FAILURE);
	}
}

int main() {
	printf("Hello World!\n");
	hello_from_static_lib();
	hello_from_dynamic_lib();
	hello_from_dyn_runtime_lib_func();
	return 0;
}
