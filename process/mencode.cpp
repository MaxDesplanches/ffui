#include "process/mencode.hh"

/* Mac OS:  R"( /usr/local/bin/ffmpeg 2>&1 )"
 *
 * Flag 0: ffmpeg system
 * Flag 1: Docker ffmpeg
 *
 * */

Mencode::Mencode()
{
    myProcess = new QProcess();
    connect(myProcess, SIGNAL(readyReadStandardError()), this, SLOT(updateError()));
    connect(myProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(updateText()));
}

void Mencode::updateError() {
    QByteArray data = myProcess->readAllStandardError();
    m_output += QString(data);
    qDebug() << m_output;
    if (m_output.contains("Invalid")) {
        emit sendFileError(m_output, m_id);
    }
    if (m_output.contains("muxing overhead")) {
        emit sendFileEnd(m_id);
    }
    else if (m_output.contains("time=")) {
        QStringList info = m_output.split(" ", QString::SkipEmptyParts);

        for (int i = 0 ; i < info.size() ; ++i) {
            QString tmp = info.at(i);

            if (tmp.startsWith("time=")) {
                emit sendFileProgress(tmp, m_id);
            }
        }
    }
}

void Mencode::updateText()
{
    QByteArray data = myProcess->readAllStandardOutput();
    m_output += QString(data);
}

void Mencode::exec(QString command, int id) {
    m_output = "";
    m_id = id;

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
}

QProcess *Mencode::getCurrentProcess() const {
    return (myProcess);
}
