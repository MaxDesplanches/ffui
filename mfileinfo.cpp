#include "Mfileinfo.hh"

int Mfileinfo::getId() const
{
    return m_id;
}

void Mfileinfo::setId(int id)
{
    m_id = id;
}

QString Mfileinfo::getCommand() const
{
    return m_command;
}

void Mfileinfo::setCommand(const QString &command)
{
    m_command = command;
}

double Mfileinfo::getDuration() const
{
    return m_duration;
}

void Mfileinfo::setDuration(const double &duration)
{
    m_duration = duration;
}

bool Mfileinfo::getEncoded() const
{
    return m_encoded;
}

void Mfileinfo::setEncoded(bool encoded)
{
    m_encoded = encoded;
}

Mfile *Mfileinfo::getFile() const
{
    return m_file;
}

void Mfileinfo::setFile(Mfile *file)
{
    m_file = file;
}

Mencode *Mfileinfo::getEncode() const
{
    return m_encode;
}

void Mfileinfo::setEncode(Mencode *encode)
{
    m_encode = encode;
}

QString Mfileinfo::getFps() const
{
    return m_fps;
}

void Mfileinfo::setFps(QString fps)
{
    m_fps = fps;
}

QString Mfileinfo::getName() const
{
    return m_name;
}

void Mfileinfo::setName(const QString &name)
{
    m_name = name;
}

QString Mfileinfo::getTarget() const
{
    return m_nametarget;
}

bool Mfileinfo::getIsRunning() const
{
    return m_isRunning;
}

void Mfileinfo::setIsRunning(bool isRunning)
{
    m_isRunning = isRunning;
}

int Mfileinfo::getRow() const
{
    return m_row;
}

void Mfileinfo::setRow(int row)
{
    m_row = row;
}

QString Mfileinfo::getNametarget() const
{
    return m_nametarget;
}

void Mfileinfo::setNametarget(const QString &nametarget)
{
    m_nametarget = nametarget;
}

QString Mfileinfo::getFoldertarget() const
{
    return m_foldertarget;
}

void Mfileinfo::setFoldertarget(const QString &foldertarget)
{
    m_foldertarget = foldertarget;
}

Mfileinfo::Mfileinfo(QObject *parent) : QObject(parent)
{
    m_id = 0;
    m_command = "";
    m_encoded = false;
}
