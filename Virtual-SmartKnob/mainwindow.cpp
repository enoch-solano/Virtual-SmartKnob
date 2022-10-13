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

    connect(ui->detentDistDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(detentDistValueChanged(double)));

    connect(ui->numPosSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numPosValueChanged(int)));
    connect(ui->minPosSpinBox, SIGNAL(valueChanged(int)), this, SLOT(minPosValueChanged(int)));
    connect(ui->maxPosSpinBox, SIGNAL(valueChanged(int)), this, SLOT(maxPosValueChanged(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/************************************************************************/
/*********************** COMPUTE HELPER FUNCS ***************************/
/************************************************************************/

int MainWindow::computeNumRevs(double dist, int count) {
    return int(ceil((dist * (count-1)) / 360.0));
}

int MainWindow::computeMax(int min, int count) {
    return min + count - 1;
}

int MainWindow::computeCount(int min, int max) {
    return max + 1 - min;
}


/************************************************************************/
/*********************** SETTER FUNCS ***********************************/
/************************************************************************/

void MainWindow::setKnob(QDial *dial, int val, int min, int count) {
    dial->setMinimum(min);
    dial->setMaximum(min + count - 1);
    dial->setValue(val);
}

void MainWindow::setSpinBoxValue(QSpinBox *spinbox, int val) {
    const QSignalBlocker blocker(spinbox);
    spinbox->setValue(val);
}

void MainWindow::setNumRevs(QSpinBox *spinbox, double dist, int count) {
    const QSignalBlocker blocker(spinbox);
    spinbox->setValue(computeNumRevs(dist, count));
}


/************************************************************************/
/*********************** MAINWIN SLOTS***********************************/
/************************************************************************/

void MainWindow::knobValueChanged(int val) {
    {
        QDial *otherKnob = sender() == ui->emulatorKnob ? ui->profileKnob : ui->emulatorKnob;
        const QSignalBlocker blocker(otherKnob);
        otherKnob->setValue(val);
    }

    ui->profileKnobValueLabel->setText(QString("Current Value: %1").arg(val));
    ui->emulatorKnobValueLabel->setText(QString("Current Value: %1").arg(val));
}

void MainWindow::detentDistValueChanged(double detentDist) {
    setNumRevs(ui->numRevsSpinBox, detentDist, ui->numPosSpinBox->value());
}

void MainWindow::numPosValueChanged(int val) {
    int detentCount = val;
    double detentDist = ui->detentDistDoubleSpinBox->value();
    setNumRevs(ui->numRevsSpinBox, detentDist, detentCount);

    int newMax = computeMax(ui->minPosSpinBox->value(), val);
    setSpinBoxValue(ui->maxPosSpinBox, newMax);

    ui->emulatorKnob->setMaximum(newMax);
    ui->profileKnob->setMaximum(newMax);
}

void MainWindow::minPosValueChanged(int val) {
    int detentCount = computeCount(val, ui->maxPosSpinBox->value());
    double detentDist = ui->detentDistDoubleSpinBox->value();
    setNumRevs(ui->numRevsSpinBox, detentDist, detentCount);

    setSpinBoxValue(ui->numPosSpinBox, detentCount);

    ui->emulatorKnob->setMinimum(val);
    ui->profileKnob->setMinimum(val);
}

void MainWindow::maxPosValueChanged(int val) {
    int detentCount = computeCount(ui->minPosSpinBox->value(), val);
    double detentDist = ui->detentDistDoubleSpinBox->value();
    setNumRevs(ui->numRevsSpinBox, detentDist, detentCount);

    setSpinBoxValue(ui->numPosSpinBox, detentCount);

    ui->emulatorKnob->setMaximum(val);
    ui->profileKnob->setMaximum(val);
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

/************************************************************************/
/*********************** PROFILE PARSING ********************************/
/************************************************************************/

void MainWindow::parseProfile(QJsonDocument &doc) {
    QJsonObject obj = doc.object();

    int detentCount = obj[DETENT_COUNT_].toInt();
    int startPos = obj[STARTING_POSITION_].toInt();
    int startVal = obj[STARTING_VALUE_].toInt();

    setKnob(ui->profileKnob, startPos, startVal, detentCount);
    setKnob(ui->emulatorKnob, startPos, startVal, detentCount);

    setSpinBoxValue(ui->numPosSpinBox, detentCount);
    setSpinBoxValue(ui->minPosSpinBox, startVal);
    setSpinBoxValue(ui->maxPosSpinBox, computeMax(startVal, detentCount));

    double detentDist = obj[DETENT_DISTANCE_].toDouble();
    int detentStrength = obj[DETENT_STRENGTH_].toInt();
    int endstopStrength = obj[ENDSTOP_STRENGTH_].toInt();

    ui->detentDistDoubleSpinBox->setValue(detentDist);
    ui->detentStrengthSpinBox->setValue(detentStrength);
    ui->endstopStrengthSpinBox->setValue(endstopStrength);

    double snapPoint = obj[SNAP_POINT_].toDouble();
    ui->snapPointDoubleSpinBox->setValue(snapPoint);

    QVector<QString> descriptor = obj[DESCRIPTOR_].toString().split(QRegularExpression("\n"));
    ui->descriptorLineEdit_1->setText(descriptor[0]);
    ui->descriptorLineEdit_2->setText(descriptor.size() > 1 ? descriptor[1] : "");
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

QByteArray MainWindow::profileToJSON() {
    QJsonObject obj;
    obj[DETENT_COUNT_] = ui->numPosSpinBox->value();
    obj[STARTING_POSITION_] = ui->profileKnob->value();
    obj[STARTING_VALUE_] = ui->minPosSpinBox->value();
    obj[DETENT_DISTANCE_] = ui->detentDistDoubleSpinBox->value();
    obj[DETENT_STRENGTH_] = ui->detentStrengthSpinBox->value();
    obj[ENDSTOP_STRENGTH_] = ui->endstopStrengthSpinBox->value();
    obj[SNAP_POINT_] = ui->endstopStrengthSpinBox->value();
    obj[DESCRIPTOR_] = ui->descriptorLineEdit_1->text() + (ui->descriptorLineEdit_2->text().isEmpty() ? "" : "/n" + ui->descriptorLineEdit_2->text());

    QJsonDocument doc(obj);
    return doc.toJson();
}

void MainWindow::exportAsJSON() {
    const QString DIR_NAME = "../Virtual-SmartKnob/Knob-Profiles/";
    const QFileInfo DIR(DIR_NAME);

    QString dir = DIR.exists() && DIR.isDir() ? DIR_NAME : "/";
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Open Knob Profile"),
                                                    dir,
                                                    tr("JSON Files (*.json *.JSON)"));

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    file.write(profileToJSON());
    file.close();
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
