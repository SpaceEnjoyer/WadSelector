#include "wadselector.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WadSelector w;
    w.show();
    return a.exec();
}
