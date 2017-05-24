#ifndef COMMAND_H
#define COMMAND_H

#include <QDebug>
#include <QString>
#include <QDir>
#include <QStringList>
#include <QProcess>
#include <QObject>

class Mcommand : public QObject {
    Q_OBJECT
private:
    QProcess*   myProcess;
    QString     output;
    int         m_actualfile;
    QStringList m_commandlist;
public:
    explicit Mcommand();
    void exec(QStringList);
    QString exec(QString);
    QString getCmdOutput(const std::string&);
    void execNext();
    QProcess *getCurrentProcess() const;
signals:
    void sendFileProgress(QString, int);
    void sendFileEnd(int);
    void sendFileInfo(QString);
public slots:
    void updateText();
    void updateError();
};

#endif // COMMAND_H
