#include <sys/sys.h>

int main(int argc, char** argv)
{
    if (!Sys_Initialize("Arkanoid 26", "ARKA26", 1280, 1024, 0))
        return 1;

    while (1)
    {
        ;
    }
}
