#ifndef Mfileinfo_H
#define Mfileinfo_H

#include <QObject>
#include <process/mfile.hh>
#include <process/mencode.hh>

class Mfileinfo : public QObject
{
    Q_OBJECT
private:
    QString     m_name;
    QString     m_nametarget;
    QString     m_foldertarget;
    int         m_row;
    bool        m_isRunning;
    int         m_id;
    QString      m_command;
    double      m_duration;
    QString      m_fps;
    bool        m_encoded;
    Mfile       *m_file;
    Mencode     *m_encode;

public:
    explicit Mfileinfo(QObject *parent = 0);

    int getId() const;
    void setId(int id);

    QString getCommand() const;
    void setCommand(const QString &command);

    double getDuration() const;
    void setDuration(const double &duration);

    bool getEncoded() const;
    void setEncoded(bool encoded);

    Mfile *getFile() const;
    void setFile(Mfile *file);

    Mencode *getEncode() const;
    void setEncode(Mencode *encode);

    QString getFps() const;
    void setFps(QString fps);

    QString getName() const;
    void setName(const QString &name);

    QString getTarget() const;
    void setTarget(const QString &target);

    bool getIsRunning() const;
    void setIsRunning(bool isRunning);

    int getRow() const;
    void setRow(int row);

    QString getNametarget() const;
    void setNametarget(const QString &nametarget);

    QString getFoldertarget() const;
    void setFoldertarget(const QString &foldertarget);

signals:

public slots:
};

#endif // Mfileinfo_H
