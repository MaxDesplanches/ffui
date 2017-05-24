#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include            <QMainWindow>
#include            <iostream>
#include            <QFileDialog>
#include            <QDebug>
#include            <QDesktopWidget>
#include            <QTime>
#include            <QDate>
#include            <QTreeWidgetItem>
#include            <QMessageBox>
#include            <QList>
#include            <QDesktopServices>
#include            <QFile>
#include            <QProgressBar>
#include            <QPalette>
#include            <math.h>

#include            "ui_mainwindow.h"
#include            "mloading.hh"
#include            "mfileinfo.hh"
#include            "process/mcodecs.hh"
#include            "process/mencode.hh"
#include            "process/mfile.hh"
#include            "process/mpixfmt.hh"

namespace           Ui {
class               MainWindow;
}

template <typename T>
class specialFpsSort
{
public:
    inline bool operator()( const T &t1, const T &t2 ) const
    {
        return( t1.toFloat() < t2.toFloat());
    }
};

template <typename T>
class specialResSort
{
public:
    inline bool operator()( const T &t1, const T &t2 ) const
    {
        QStringList tmp1 = t1.split(" ");
        tmp1 = tmp1.at(0).split("x");
        QStringList tmp2 = t1.split(" ");
        tmp2 = tmp2.at(0).split("x");
        return(tmp1.at(0).toInt() * tmp1.at(1).toInt() <
               tmp2.at(0).toInt() * tmp2.at(1).toInt()
               );
    }
};

class               MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    int             m_screenWidth;
    int             m_screenHeight;
    Mloading        *m_loading;
    double          m_previous;
    QStringList     m_fpslist;
    Mcodecs         *m_codecs;
    Mpixfmt         *m_pixfmt;
    int             m_id;
    bool            isExpert;
    QList<Mfileinfo*> m_fileinfo;
public:
    Ui::MainWindow* ui;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void            setEnabledVideo(bool);
    void            setEnabledAudio(bool);
    void            defaultParams();
    void            addCommand(QString &);
    bool            checkExtension(QString);
    void            displayError(QString);
    double          convertQStringToSecond(QString time);
    void            firstLaunch();
    int             getRowById(int);
    int             getPosByRow(int);
    int             getPosById(int);
    void            execNext();

public slots:
    void            updatePxlFmt(QList<Mloading::m_data>);
    void            updateFileProgress(QString, int);
    void            updateFileEnd(int);
    void            updateFileError(QString, int);
    void            updateVideoCodec(QList<Mloading::m_data>);
    void            updateAudioCodec(QList<Mloading::m_data>);
    void            updateDuration(double, int);
    void            updateFps(QString, int);

private slots:
    void            on_browse_source_clicked();
    void            on_browse_target_clicked();
    void            on_copyvideocodec_checkbox_clicked(bool checked);
    void            on_copyaudiocodec_checkbox_clicked(bool checked);
    void            on_fpscustom_checkBox_clicked(bool checked);
    void            on_customresolution_checkBox_clicked(bool checked);
    void            on_video_checkBox_clicked(bool checked);
    void            on_audio_checkBox_clicked(bool checked);
    void            on_audio_custombitrate_checkBox_clicked(bool checked);
    void            on_audio_format_comboBox_currentTextChanged(const QString &arg1);
    void            on_actionQuit_triggered();
    void            on_actionDon_triggered();
    void            on_actionAdd_file_triggered();
    void            on_actionTwitter_triggered();

    void on_actionDelete_selected_row_triggered();

    void on_actionEncode_triggered();

    void on_actionExpert_mode_triggered(bool checked);

protected:
    void            closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
