#include "QtWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtWindow w;
    w.show();
    return a.exec();
}
