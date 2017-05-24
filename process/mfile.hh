#ifndef MFILE_H
#define MFILE_H

#include <QProcess>
#include <QDebug>
#include <QDir>

class Mfile : public QObject {
    Q_OBJECT
private:
    QString     m_output;
    QProcess*   myProcess;
public:
    explicit Mfile();
    void exec(QString, int);
signals:
    void sendFileInfo(QString, int);
public slots:
    void updateText();
    void updateError();
};

#endif // MFILE_H
