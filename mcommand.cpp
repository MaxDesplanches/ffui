#include "mcommand.hh"

/* Mac OS:  R"( /usr/local/bin/ffmpeg 2>&1 )"
 *
 * Flag 0: ffmpeg system
 * Flag 1: Docker ffmpeg
 *
 * */

Mcommand::Mcommand()
{
    m_actualfile = 0;
    myProcess = new QProcess();
    connect(myProcess, SIGNAL(readyReadStandardError()), this, SLOT(updateError()));
    connect(myProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(updateText()));
}

void Mcommand::exec(QStringList commandlist) {
    m_commandlist = commandlist;

    exec(commandlist.at(m_actualfile));
}

void Mcommand::execNext() {
    m_actualfile++;

    if (m_actualfile < m_commandlist.size()) {
        exec(m_commandlist.at(m_actualfile));
    }
}

void Mcommand::updateError()
{
    QByteArray data = myProcess->readAllStandardError();
    output += QString(data);

    if (output.contains("At least one output file must be specified", Qt::CaseInsensitive)) {
        myProcess->close();
        emit sendFileInfo(output);
    }
    else if (QString(data).contains("time=")) {
        QStringList info = QString(data).split(" ", QString::SkipEmptyParts);

        for (int i = 0 ; i < info.size() ; ++i) {
            QString tmp = info.at(i);

            if (tmp.startsWith("time=")) {
                emit sendFileProgress(tmp, m_actualfile);
            }
        }
    }
    else if (QString(data).contains("muxing overhead")) {
        emit sendFileEnd(m_actualfile);
        myProcess->close();
        execNext();
    }
}

void Mcommand::updateText()
{
    QByteArray data = myProcess->readAllStandardOutput();
    output += (QString(data));
}

QString Mcommand::exec(QString command) {
    output = "";

    qDebug() << "COMMAND" << command;

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

    myProcess->close();
    return (output);
}

QString Mcommand::getCmdOutput(const std::string& mStr)
{
    QString result = "";
    FILE* pipe{popen(mStr.c_str(), "r")};

    char buffer[2];

    while(fgets(buffer, sizeof(buffer), pipe) != NULL)
    {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

QProcess *Mcommand::getCurrentProcess() const {
    return (myProcess);
}
