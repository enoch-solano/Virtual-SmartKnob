#include "dependencies/darkstyle/darkstyle.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon("://icons/knobs/dial_512.png"));
//    a.setStyle(new DarkStyle);    // uncomment for darkstyle

    MainWindow w;
    w.show();
    return a.exec();
}
