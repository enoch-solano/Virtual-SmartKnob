#include "dependencies/darkstyle/darkstyle.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    a.setStyle(new DarkStyle);    // uncomment for darkstyle

    MainWindow w;
    w.show();
    return a.exec();
}
