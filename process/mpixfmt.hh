#ifndef MPIXFMT_H
#define MPIXFMT_H

#include <QProcess>
#include <QDebug>
#include <QDir>

class Mpixfmt : public QObject {
    Q_OBJECT
private:
    QProcess*   myProcess;
    QString     output;
public:
    explicit Mpixfmt();
    void exec(QString);
signals:
    void sendPixfmtToLoad(QString);
public slots:
    void updateText();
    void updateError();
};

#endif // MPIXFMT_H
