#ifndef MENCODE_H
#define MENCODE_H

#include <QProcess>
#include <QDebug>
#include <QDir>

class Mencode : public QObject {
    Q_OBJECT
private:
    QProcess*   myProcess;
    QString     m_output;
    int         m_id;
public:
    explicit Mencode();
    void exec(QString, int);
    QProcess * getCurrentProcess() const;

signals:
    void sendFileProgress(QString, int);
    void sendFileEnd(int);
    void sendFileInfo(QString, int);
    void sendFileError(QString, int);
public slots:
    void updateText();
    void updateError();
};

#endif // MENCODE_H
