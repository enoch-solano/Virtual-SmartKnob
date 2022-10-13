#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "knob_data.h"
#include "qdial.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QFileDialog>
#include <QRegularExpression>


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
    void parseProfile(QJsonDocument &doc);

    void setKnob(QDial *dial, int val, int min, int count);

    inline int computeNumRevs(double dist, int count);

    void exportAsJSON();

    QByteArray promptUserForProfileJSONFile();

private slots:
    void importProfile();
    void exportProfile();

    void knobValueChanged(int);
};
#endif // MAINWINDOW_H
