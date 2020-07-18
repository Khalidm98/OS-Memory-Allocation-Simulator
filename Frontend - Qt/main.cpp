#include "mem_allocator.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Mem_Allocator w;
    w.show();
    return a.exec();
}
