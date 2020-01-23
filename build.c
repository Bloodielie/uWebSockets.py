#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* List of platform features */
#ifdef _WIN32
#define OS "win32"
#define IS_WINDOWS
#endif
#ifdef __linux
#define OS "linux"
#define IS_LINUX
#endif
#ifdef __APPLE__
#define OS "darwin"
#define IS_MACOS
#endif

/* System, but with string replace */
int run(const char *cmd, ...) {
    char buf[512];
    va_list args;
    va_start(args, cmd);
    vsprintf(buf, cmd, args);
    va_end(args);
    printf("--> %s\n\n", buf);
    return system(buf);
}

/* List of Python ABIs */
struct node_version {
    char *name;
    char *abi;
} versions[] = {
    {"3.8.1", "38"},
    {"3.7.6", "37"},
    {"3.6.10", "36"},
    //{"3.5.9", "79"} // does not have fastcall
};

/* Build for Unix systems */
void build(char *compiler, char *cpp_compiler, char *cpp_linker, char *os, char *arch) {
    /* Build uSockets.a */
    run("WITH_LIBUV=1 make -C uWebSockets/uSockets");

    /* Build for the different versions */
    for (unsigned int i = 0; i < sizeof(versions) / sizeof(struct node_version); i++) {
        run("mkdir -p dist/%s", versions[i].abi);
        run("cp src/uws.py dist/%s/uws.py", versions[i].abi);
        run("g++ -std=c++17 -shared -O3 -flto -fPIC -DUWS_NO_ZLIB -static-libstdc++ -static-libgcc -s -I ABIs -I ABIs/%s -I uWebSockets/uSockets/src -I uWebSockets/src src/extension.cpp uWebSockets/uSockets/uSockets.a -luv -o dist/%s/uwebsocketspy.so", versions[i].name, versions[i].abi);
    }
}

int main() {
    printf("\n[Building]\n");

#ifdef IS_MACOS
    /* Apple special case */
    build("clang -mmacosx-version-min=10.7",
          "clang++ -stdlib=libc++ -mmacosx-version-min=10.7",
          "-undefined dynamic_lookup",
          OS,
          "x64");
#else
    /* Linux */
    build("clang",
          "clang++",
          "-static-libstdc++ -static-libgcc -s",
          OS,
          "x64");
#endif
}


