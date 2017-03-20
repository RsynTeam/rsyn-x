#include <chrono>
#include <thread>

#include <iostream>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>



int main() {
    using std::cout;
    using std::cerr;

    cout << "C++ dlopen demo\n\n";

    // open the library
    cout << "Opening hello.so...\n";
    
	std::cout << dlopen("/usr/lib/x86_64-linux-gnu/libwx_gtk2u_xrc-3.0.so", RTLD_NOW|RTLD_GLOBAL);
	std::cout << dlopen("/usr/lib/x86_64-linux-gnu/libwx_gtk2u_html-3.0.so", RTLD_NOW|RTLD_GLOBAL);
	std::cout << dlopen("/usr/lib/x86_64-linux-gnu/libwx_gtk2u_qa-3.0.so", RTLD_NOW|RTLD_GLOBAL);
	std::cout << dlopen("/usr/lib/x86_64-linux-gnu/libwx_gtk2u_adv-3.0.so", RTLD_NOW|RTLD_GLOBAL);
	std::cout << dlopen("/usr/lib/x86_64-linux-gnu/libwx_gtk2u_core-3.0.so", RTLD_NOW|RTLD_GLOBAL);
	std::cout << dlopen("/usr/lib/x86_64-linux-gnu/libwx_baseu_xml-3.0.so", RTLD_NOW|RTLD_GLOBAL);
	std::cout << dlopen("/usr/lib/x86_64-linux-gnu/libwx_baseu_net-3.0.so", RTLD_NOW|RTLD_GLOBAL);
	std::cout << dlopen("/usr/lib/x86_64-linux-gnu/libwx_baseu-3.0.so", RTLD_NOW|RTLD_GLOBAL);
	
    void* handle = dlopen("./lib.so",RTLD_NOW);
    
    if (!handle) {
        cerr << "Cannot open library: " << dlerror() << '\n';
        return 1;
    }
    
    // load the symbol
    cout << "Loading symbol hello...\n";
    typedef void (*hello_t)();

    // reset errors
    dlerror();
    hello_t hello = (hello_t) dlsym(handle, "hello");
    const char *dlsym_error = dlerror();
    
    if (dlsym_error) {
        cerr << "Cannot load symbol 'hello': " << dlsym_error <<
            '\n';
        dlclose(handle);
        return 1;
    }
    
    // use it to do the calculation
    cout << "Calling hello...\n";
    
    pid_t pid = fork();
    if (pid == 0) {
        // child process
 	   hello();
    } else if (pid > 0) {
        // parent process
        wait();
    } else {
        // fork failed
        printf("fork() failed!\n");
        return 1;
    }

    // close the library
    cout << "Closing library...\n";
    dlclose(handle);
}
