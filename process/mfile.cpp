#include "process/mfile.hh"

/* Mac OS:  R"( /usr/local/bin/ffmpeg 2>&1 )"
 *
 * Flag 0: ffmpeg system
 * Flag 1: Docker ffmpeg
 *
 * */

Mfile::Mfile()
{
    myProcess = new QProcess();
    connect(myProcess, SIGNAL(readyReadStandardError()), this, SLOT(updateError()));
    connect(myProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(updateText()));
}

void Mfile::updateError()
{
    QByteArray data = myProcess->readAllStandardError();
    m_output += QString(data);
}

void Mfile::updateText()
{
    QByteArray data = myProcess->readAllStandardOutput();
}

void Mfile::exec(QString command, int id) {
    m_output = "";

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

    emit sendFileInfo(m_output, id);

    myProcess->close();
}
