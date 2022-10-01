#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "knob_data.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void importProfile();
    void exportProfile();
};
#endif // MAINWINDOW_H
