#include "mloading.hh"

Mloading::Mloading() {
    resolution = "";
    videocodec = "";
    audiocodec = "";
}

void Mloading::pxlFmt(QString result) {
    if (!result.contains("-----") || result == "") {
        return;
    }

    QStringList pixfmt = result.split("-----", QString::SkipEmptyParts);
    QString codeclist = pixfmt.at(1);
    QStringList pixfmtlist = codeclist.split("\n", QString::SkipEmptyParts);

    for (int i = 1 ; i < pixfmtlist.size() ; ++i) {

        QString line = pixfmtlist.at(i);
        QStringList tmp = line.split(" ", QString::SkipEmptyParts);
        QString available = tmp.at(0);
        QString flag = tmp.at(1);
        QString name = tmp.at(3);
        name.remove(QRegExp("[\\n\\t\\r]"));
        flag.remove(QRegExp("[\\n\\t\\r]"));

        if (available.contains("O")) {
            m_data tmp;
            tmp.m_flag = flag;
            tmp.m_name = name;
            mypixfmt.push_back(tmp);
        }
    }

    emit sendPxlFmtToMain(mypixfmt);
}

void Mloading::codecs(QByteArray result) {

    QStringList myresult =
            (QString::fromStdString(result.toStdString())).split("\n");

    if (myresult.size() == 0) {
        return;
    }

    bool isCodec = false;

    QList<m_data> m_video;
    QList<m_data> m_audio;

    // -------

    for (int i = 0 ; i < myresult.size() ; ++i) {
        if (myresult.at(i).contains("-------") == true) {
            isCodec = true;
        }

        QStringList tmp;

          qDebug() << myresult.at(i);

        if (isCodec == true) {
            tmp = myresult.at(i).split(" ", QString::SkipEmptyParts);

            if (tmp.size() >= 3) {
                QString available = tmp.at(0);
                QString flag = tmp.at(1);
                QString name = tmp.at(2);
                /*
                for (int j = 2 ; j < tmp.size() ; ++j) {
                    name += " " + tmp.at(j);
                }
                */
                name.remove(QRegExp("[\\n\\t\\r]"));
                flag.remove(QRegExp("[\\n\\t\\r]"));

                if (available.contains("EV")) {
                    m_data tmp;
                         qDebug() << "Adding default Video Codec: " << name;
                    tmp.m_flag = flag;
                    tmp.m_name = name;
                    m_video.push_back(tmp);
                }
                else if (available.contains("EA")) {
                    m_data tmp;
                         qDebug() << "Adding default Audio Codec: " << name;
                    tmp.m_flag = flag;
                    tmp.m_name = name;
                    m_audio.push_back(tmp);
                }
            }
        }
        tmp.clear();
    }
    emit sendVideoCodecToMain(m_video);
    emit sendAudioCodecToMain(m_audio);
}

void Mloading::setVideoValue(int id) {

    qDebug() << "video size" << m_filevideo.size();

    for (int i = 0 ; i < m_filevideo.size() ; ++i) {
        QStringList tmp = m_filevideo.at(i);
        for (int j = 0 ; j < tmp.size() ; ++j) {
            QString mytmp = tmp.at(j);

            //    qDebug() << "Found Video: " << mytmp;

            // FPS

            if (mytmp.contains("fps", Qt::CaseInsensitive)) {
                QStringList mytmplist = mytmp.split(" ", QString::SkipEmptyParts);
                if (mytmplist.size() > 1) {
                    emit sendFpsToMain(mytmplist.at(0), id);
                }
            }

            // Resolution

            if (mytmp.contains(QRegExp("[\\d]{2,4}x[\\d]{2,4}"))) {
                resolution = mytmp;
                //         qDebug() << "added Resolution: " << resolution;
            }

            // Video Codec

            if (mytmp.contains("Video:")) {
                QStringList videotmp = mytmp.split(" ", QString::SkipEmptyParts);
                if (videotmp.size() > 2) {
                    videocodec = videotmp.at(2);
                    //          qDebug() << "added VideoCodec: " << videocodec;
                }
            }
        }
    }
}

void Mloading::setAudioValue(int id) {
    for (int i = 0 ; i < m_fileaudio.size() ; ++i) {
        QStringList tmp = m_fileaudio.at(i);
        for (int j = 0 ; j < tmp.size() ; ++j) {
            QString mytmp = tmp.at(j);
            //    qDebug() << "Found Audio: " << mytmp;

            // Audio Codec

            if (mytmp.contains("Audio:")) {
                QStringList audiotmp = mytmp.split(" ", QString::SkipEmptyParts);
                if (audiotmp.size() > 2) {

                    audiocodec = audiotmp.at(2);
                    //          qDebug() << "added AudioCodec: " << audiocodec;
                }
            }
        }
    }
}

void Mloading::file(QString result, int id) {
    m_filevideo.clear();
    m_fileaudio.clear();
    QStringList split = result.split("Stream");

    for (int i = 0 ; i < split.size() ; ++i) {
        QString tmp = split.at(i);

        if (tmp.contains("Duration:")) {
            tmp = tmp.split("Duration:")[1];
            tmp = tmp.split(", ")[0];
            tmp = tmp.replace(" ", "");
            emit sendDurationToMain(convertQStringToSecond(tmp), id);
        }
        if (tmp.contains("Video:")) {
            m_filevideo.append(tmp.split(", "));
        }
        else if (tmp.contains("Audio:")) {
            m_fileaudio.append(tmp.split(", "));
        }
    }
    setVideoValue(id);
}

double Mloading::convertQStringToSecond(QString time)
{
    double final = 0;

    QStringList mytime = time.split(":");
    qDebug() << "Mytime size: " << mytime.size();
    if (mytime.size() >= 3) {
        final += mytime.at(0).toDouble() * 3600;
        final += mytime.at(1).toDouble() * 60;
        final += mytime.at(2).toDouble();
    }
    else {
        qDebug() << "Error on calculate size from time. (Mloading)";
    }

    return (final);
}
