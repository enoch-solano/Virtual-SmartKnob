#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    doubleProfileKeys << "Detent Count"
                      << "Starting Position"
                      << "Detent Strength"
                      << "Endstop Strength"
                      << "Distance between Detents"
                      << "Snap Point";
    stringProfileKeys << "Descriptor";

    necessaryProfileKeys << doubleProfileKeys
                         << stringProfileKeys;

    connect(ui->actionImport_Profile, SIGNAL(triggered(bool)), this, SLOT(importProfile()));
    connect(ui->actionExport_Profile, SIGNAL(triggered(bool)), this, SLOT(exportProfile()));
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
            return false;
        }
    }

    for (auto const &key : doubleProfileKeys) {
        if (!obj[key].isDouble()) {
            return false;
        }
    }

    for (auto const &key : stringProfileKeys) {
        if (!obj[key].isString()) {
            return false;
        }
    }

    return true;
}

QByteArray MainWindow::promptUserForFile() {
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open Knob Profile"),
                                                    "../Virtual-SmartKnob/Knob-Profiles/",
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

void MainWindow::importProfile() {
    QByteArray text = promptUserForFile();

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
}

void MainWindow::exportProfile() {
    exportAsJSON();
}

void MainWindow::exportAsJSON() {

}
