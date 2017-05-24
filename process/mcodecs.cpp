#include "process/mcodecs.hh"

Mcodecs::Mcodecs()
{
    myProcess = new QProcess();
    connect(myProcess, SIGNAL(readyReadStandardError()), this, SLOT(updateError()));
    connect(myProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(updateText()));
}

void Mcodecs::updateError()
{
    QByteArray data = myProcess->readAllStandardError();
    m_output.append(data);
}

void Mcodecs::updateText()
{
    QByteArray data = myProcess->readAllStandardOutput();
    m_output.append(data);
}

void Mcodecs::exec(QString command) {
    m_output = "";

#ifdef __APPLE__

    QString program = "/usr/local/bin/" + command;

#elif _WIN32

    qDebug() << "mypath" << QDir::currentPath();

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

    emit sendCodecs(m_output);

    myProcess->close();
}
