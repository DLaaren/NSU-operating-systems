all: static_lib dynamic_lib dynamic_runtime_lib executable

executable: static_lib dynamic_lib dynamic_runtime_lib
	gcc -Wl,-rpath=. hello.c hello_static.a hello_dynamic.so -o hello
static_lib:
	gcc -c hello_static.c -o hello_static.o
	ar rcs hello_static.a hello_static.o
dynamic_lib:
	gcc hello_dynamic.c -shared -fPIC -o hello_dynamic.so
dynamic_runtime_lib:
	gcc hello_from_dyn_runtime.c -shared -fPIC -o hello_from_dyn_runtime.so
