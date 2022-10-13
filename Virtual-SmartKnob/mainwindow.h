#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "knob_data.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QDial>
#include <QSpinBox>

#include <QMessageBox>
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
    void setSpinBoxValue(QSpinBox *spinbox, int val);
    void setNumRevs(QSpinBox *spinbox, double dist, int count);

    inline int computeNumRevs(double dist, int count);
    inline int computeMax(int min, int count);
    inline int computeCount(int min, int max);

    void exportAsJSON();
    QByteArray profileToJSON();

    QByteArray promptUserForProfileJSONFile();

private slots:
    void importProfile();
    void exportProfile();

    void knobValueChanged(int);
    void detentDistValueChanged(double);

    void numPosValueChanged(int);
    void minPosValueChanged(int);
    void maxPosValueChanged(int);
};
#endif // MAINWINDOW_H
