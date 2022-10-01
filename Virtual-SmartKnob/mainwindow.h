#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "knob_data.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QFileDialog>


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

    QStringList doubleProfileKeys;
    QStringList stringProfileKeys;
    QStringList necessaryProfileKeys;

    bool isValidProfile(QJsonDocument &doc);
    bool validateProfile(QJsonDocument &doc);

    void exportAsJSON();

    QByteArray promptUserForFile();

private slots:
    void importProfile();
    void exportProfile();
};
#endif // MAINWINDOW_H
