#ifndef MLOADING_H
#define MLOADING_H

#include                <QStringList>
#include                <QVector>
#include                <QDebug>
#include                <QDateTime>

class                   Mloading : public QObject {
    Q_OBJECT
public:
    typedef             struct s_data {
        QString         m_flag;
        QString         m_name;
    }                   m_data;
    Mloading();
    double              convertQStringToSecond(QString time);

    void                setVideoValue(int);
    void                setAudioValue(int);
signals:
    void                sendVideoCodecToMain(QList<Mloading::m_data>);
    void                sendAudioCodecToMain(QList<Mloading::m_data>);
    void                sendPxlFmtToMain(QList<Mloading::m_data>);
    void                sendDurationToMain(double, int);
    void                sendFpsToMain(QString, int);
public slots:
    void                file(QString, int);
    void                pxlFmt(QString);
    void                codecs(QByteArray);
private:
    // default value
    QString             resolution;
    QString             audiocodec;
    QString             videocodec;
    QList<m_data>       mypixfmt;

    QList<QStringList>  m_filevideo;
    QList<QStringList>  m_fileaudio;
};

#endif // MLOADING_H
