#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    doubleProfileKeys << DETENT_COUNT_
                      << STARTING_POSITION_
                      << STARTING_VALUE_
                      << DETENT_STRENGTH_
                      << ENDSTOP_STRENGTH_
                      << DETENT_DISTANCE_
                      << SNAP_POINT_;
    stringProfileKeys << DESCRIPTOR_;

    necessaryProfileKeys << doubleProfileKeys
                         << stringProfileKeys;

    connect(ui->actionImport_Profile, SIGNAL(triggered(bool)), this, SLOT(importProfile()));
    connect(ui->actionExport_Profile, SIGNAL(triggered(bool)), this, SLOT(exportProfile()));

    connect(ui->profileKnob, SIGNAL(valueChanged(int)), this, SLOT(knobValueChanged(int)));
    connect(ui->emulatorKnob, SIGNAL(valueChanged(int)), this, SLOT(knobValueChanged(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::isValidProfile(QJsonDocument &doc) {
    return validateProfile(doc);
}

bool MainWindow::validateProfile(QJsonDocument &doc) {
    QJsonObject obj = doc.object();

    for (auto const &key : necessaryProfileKeys) {
        if (!obj.contains(key)) {
            qDebug() << "missing necessary key:" << key;
            return false;
        }
    }

    for (auto const &key : doubleProfileKeys) {
        if (!obj[key].isDouble()) {
            qDebug() << "missing double key:" << key;
            return false;
        }
    }

    for (auto const &key : stringProfileKeys) {
        if (!obj[key].isString()) {
            qDebug() << "missing string key:" << key;
            return false;
        }
    }

    return true;
}

QByteArray MainWindow::promptUserForProfileJSONFile() {
    const QString DIR_NAME = "../Virtual-SmartKnob/Knob-Profiles/";
    const QFileInfo DIR(DIR_NAME);

    QString dir = DIR.exists() && DIR.isDir() ? DIR_NAME : "/";
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open Knob Profile"),
                                                    dir,
                                                    tr("JSON Files (*.json *.JSON)"));

    if (!QFile::exists(filename)) {
        return "";
    }

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "";
    }

    QByteArray filetext = file.readAll();
    file.close();

    return filetext;
}

int MainWindow::computeNumRevs(double dist, int count) {
    return int(ceil((dist * (count-1)) / 360.0));
}

void MainWindow::setKnob(QDial *dial, int val, int min, int count) {
    dial->setMinimum(min);
    dial->setMaximum(min + count - 1);
    dial->setValue(val);
}

void MainWindow::parseProfile(QJsonDocument &doc) {
    QJsonObject obj = doc.object();

    int detCount = obj[DETENT_COUNT_].toInt();
    int startPos = obj[STARTING_POSITION_].toInt();
    int startVal = obj[STARTING_VALUE_].toInt();

    setKnob(ui->profileKnob, startPos, startVal, detCount);
    setKnob(ui->emulatorKnob, startPos, startVal, detCount);

    ui->numPosSpinBox->setValue(detCount);
    ui->minPosSpinBox->setValue(startVal);
    ui->maxPosSpinBox->setValue(startVal + detCount - 1);

    double detentDist = obj[DETENT_DISTANCE_].toDouble();
    int detentStrength = obj[DETENT_STRENGTH_].toInt();
    int endstopStrength = obj[ENDSTOP_STRENGTH_].toInt();

    ui->detentDistDoubleSpinBox->setValue(detentDist);
    ui->detentStrengthSpinBox->setValue(detentStrength);
    ui->endstopStrengthSpinBox->setValue(endstopStrength);

    int numRev = computeNumRevs(detentDist, detCount);
    ui->numRevsSpinBox->setValue(numRev);

    double snapPoint = obj[SNAP_POINT_].toDouble();
    ui->snapPointDoubleSpinBox->setValue(snapPoint);

    QVector<QString> descriptor = obj[DESCRIPTOR_].toString().split(QRegularExpression("\n"));
    ui->descriptorLineEdit_1->setText(descriptor[0]);
    ui->descriptorLineEdit_2->setText(descriptor.size() > 1 ? descriptor[1] : "");
}

void MainWindow::knobValueChanged(int val) {
    {
        QDial *otherKnob = sender() == ui->emulatorKnob ? ui->profileKnob : ui->emulatorKnob;
        const QSignalBlocker blocker(otherKnob);
        otherKnob->setValue(val);
    }

    ui->profileKnobValueLabel->setText(QString("Current Value: %1").arg(val));
    ui->emulatorKnobValueLabel->setText(QString("Current Value: %1").arg(val));
}

void MainWindow::importProfile() {
    QByteArray text = promptUserForProfileJSONFile();

    if (text.isEmpty()) {
        return;
    }

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(text, &jsonError);

    if (QJsonParseError::NoError != jsonError.error) {
        qDebug() << "error reading profile";
        return;
    } else if (!isValidProfile(doc)) {
        qDebug() << "profile is not valid";
        return;
    }

    parseProfile(doc);
}

void MainWindow::exportProfile() {
    exportAsJSON();
}

void MainWindow::exportAsJSON() {

}
