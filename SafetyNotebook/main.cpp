#include "mainwindow.h"

#include <QtWidgets/QApplication>
#include <QtCore/QDebug>
#include "loader.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QString string = "Test1234";
    QByteArray data = string.toUtf8();
    QByteArray sha256_sum_b = sha512_sum(data);
    qDebug() << "Or:" << string;
    qDebug() << "Da:" << data;
    qDebug() << "Ha:" << sha256_sum_b;
    qDebug() << "Si:" << sha256_sum_b.size();

    MainWindow w;
    w.show();
    return a.exec();
}
