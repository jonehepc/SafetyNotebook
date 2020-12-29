#include "mainwindow.h"
#include "loader.h"

#include <QtWidgets/QApplication>
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QtCore>

#include "SNoteItem.h"

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
