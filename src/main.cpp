#include "option.h"
#include "calendar.h"

int main(int argc, char *argv[])
{
    Option opt;
    opt.parseArgs(argc, argv);
    Calendar cal(opt);
    cal.print();
    return 0;
}