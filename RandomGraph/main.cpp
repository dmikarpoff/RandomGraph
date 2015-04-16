#include "mainwindow.h"
#include "tester.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    if (argc == 1) {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();
        return a.exec();
    }
    Tester tester;
    tester.runAllTests();
    return 0;
}
