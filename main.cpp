#include "mainwindow.hh"
#include "mloading.hh"

#include <unistd.h>
#include <QApplication>
#include <QTranslator>
#include <QtDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString locale = QLocale::system().name().section('_', 0, 0);

    QTranslator translator;
    translator.load(QString("software_") + locale);
    app.installTranslator(&translator);

    MainWindow window;

    window.show();

    return app.exec();
}

