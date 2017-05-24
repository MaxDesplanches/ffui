#include "process/mpixfmt.hh"

Mpixfmt::Mpixfmt()
{
    myProcess = new QProcess();
    connect(myProcess, SIGNAL(readyReadStandardError()), this, SLOT(updateError()));
    connect(myProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(updateText()));
}

void Mpixfmt::updateError()
{
    QByteArray data = myProcess->readAllStandardError();
    output += QString(data);
}

void Mpixfmt::updateText()
{
    QByteArray data = myProcess->readAllStandardOutput();
    output += QString(data);
}

void Mpixfmt::exec(QString command) {
    output = "";

#ifdef __APPLE__

    QString program = "/usr/local/bin/" + command;

#elif _WIN32

    QString program = "ffmpeg\\bin\\" + command;

#endif

    myProcess->start(program);

    if (myProcess->waitForStarted(1000) == false) {
        qDebug() << "Error starting external program";
    }
    else {
        qDebug() << "external program running";
    }

    myProcess->waitForReadyRead(1000);
    myProcess->waitForFinished(1000);

    emit sendPixfmtToLoad(output);

    myProcess->close();
}
