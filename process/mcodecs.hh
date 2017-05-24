#ifndef MCODECS_H
#define MCODECS_H

#include <QProcess>
#include <QDebug>
#include <QDir>

class Mcodecs : public QObject {
    Q_OBJECT
private:
    QProcess*   myProcess;
    QByteArray  m_output;
public:
    explicit Mcodecs();
    void exec(QString);
signals:
    void sendCodecs(QByteArray);
public slots:
    void updateText();
    void updateError();
};

#endif // MCODECS_H
