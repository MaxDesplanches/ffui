#include "mainwindow.hh"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_id = 0;

    // default

  //  ui->video_formLayout->setFormAlignment( Qt::AlignLeft | Qt::AlignTop );
  //  ui->audio_formLayout->setFormAlignment( Qt::AlignLeft | Qt::AlignTop );

    ui->mainToolBar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    ui->parameters_tab->setEnabled(false);

    if (ui->tabWidget->currentIndex() != 1) {
        ui->tabWidget->setCurrentIndex(1);
    }

    // default file extension

    int index = ui->filename_comboBox->findText("mp4");
    if(index >= 0) {
        ui->filename_comboBox->setCurrentIndex(index);
    }

    this->setMinimumWidth(800);

    this->adjustSize();

    QDesktopWidget wid;

    m_screenWidth = wid.screen()->width();
    m_screenHeight = wid.screen()->height();

    int width = this->frameGeometry().width();
    int height = this->frameGeometry().height();

    this->setGeometry((m_screenWidth/2)-(width/2), (m_screenHeight/2)-(height/2), width, height);

    firstLaunch();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::displayError(QString error) {
    QMessageBox messageBox;
    messageBox.critical(0,"Error", error);
    messageBox.setFixedSize(500,200);

}


void MainWindow::firstLaunch() {
    m_codecs = new Mcodecs;
    m_pixfmt = new Mpixfmt;
    m_loading = new Mloading;

    // pixfmt connect

    connect(m_pixfmt, SIGNAL(sendPixfmtToLoad(QString)), m_loading, SLOT(pxlFmt(QString)));
    connect(m_loading, SIGNAL(sendPxlFmtToMain(QList<Mloading::m_data>)), this, SLOT(updatePxlFmt(QList<Mloading::m_data>)));

    // codecs connect

    connect(m_codecs, SIGNAL(sendCodecs(QByteArray)), m_loading, SLOT(codecs(QByteArray)));
    connect(m_loading, SIGNAL(sendAudioCodecToMain(QList<Mloading::m_data>)), this, SLOT(updateAudioCodec(QList<Mloading::m_data>)));
    connect(m_loading, SIGNAL(sendVideoCodecToMain(QList<Mloading::m_data>)), this, SLOT(updateVideoCodec(QList<Mloading::m_data>)));

    // loading connect

    connect(m_loading, SIGNAL(sendFpsToMain(QString, int)), this, SLOT(updateFps(QString, int)));
    connect(m_loading, SIGNAL(sendDurationToMain(double, int)), this, SLOT(updateDuration(double, int)));

    m_codecs->exec("ffmpeg -codecs");
    m_pixfmt->exec("ffmpeg -pix_fmts");

    // Adding Resolution Params

    ui->resolution_comboBox->addItem("4096x2160 (4K)");
    ui->resolution_comboBox->addItem("3840x2160 (UHD)");
    ui->resolution_comboBox->addItem("2560x1440 (2K)");
    ui->resolution_comboBox->addItem("1920x1080 (Full HD)");
    ui->resolution_comboBox->addItem("1280x800 (WXGA)");
    ui->resolution_comboBox->addItem("1280x768 (WXGA)");
    ui->resolution_comboBox->addItem("1280x720 (HD Ready)");
    ui->resolution_comboBox->addItem("1024x600 (WSVGA)");
    ui->resolution_comboBox->addItem("768x576 (PAL)");
    ui->resolution_comboBox->addItem("854x480 (WVGA)");
    ui->resolution_comboBox->addItem("640x360 (Very bad)");
    ui->resolution_comboBox->addItem("480x320 (Some pixel)");

    QHeaderView* header = ui->tableWidget->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::on_browse_source_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/");

    if (!fileName.isEmpty()) {

        ui->browse_line_edit->setText(fileName);

        QStringList parts = fileName.split("/");
        QString lastBit = parts.at(parts.size()-1);

        QStringList extension = lastBit.split(".", QString::SkipEmptyParts);

        if (checkExtension(extension[extension.length() - 1]) == false) {
            displayError(QString(tr("File extension is not correct.")));
            return;
        }

        // Default file name

        QString out = lastBit.split(".", QString::SkipEmptyParts).at(0) +
                "_" + QDate::currentDate().toString() +
                "_" + QTime::currentTime().toString().replace(":", "_");

        ui->filename_edit->setText(out.replace(" ", "_"));

        // target folder

        ui->browse_target_edit->setText(fileName.replace(lastBit, ""));

        // Mfileinfo: file
        // MFileinfo: id
        // MFileinfo: name
        // MFileinfo: target

        Mfileinfo *fileinfo = new Mfileinfo;
        Mfile *file = new Mfile;

        connect(file, SIGNAL(sendFileInfo(QString, int)), m_loading, SLOT(file(QString, int)));

        fileinfo->setEncoded(true);
        fileinfo->setIsRunning(false);

        m_fileinfo.push_back(fileinfo);

        m_fileinfo.last()->setId(m_id);
        m_fileinfo.at(getPosById(m_id))->setFile(file);

        m_fileinfo.at(getPosById(m_id))->getFile()->exec("ffmpeg -i \"" +
                                                         ui->browse_line_edit->text() +
                                                         "\"", m_id);

        // Enable Functionality

        ui->browse_target->setEnabled(true);
        ui->browse_target_edit->setEnabled(true);
        ui->filename_edit->setEnabled(true);
        ui->filename_comboBox->setEnabled(true);
        ui->parameters_tab->setEnabled(true);

        if (ui->copyvideocodec_checkbox->isChecked() == false) {
            setEnabledVideo(true);
        }
        else {
            setEnabledVideo(false);
        }

        if (ui->tabWidget->currentIndex() != 1) {
            ui->tabWidget->setCurrentIndex(1);
        }
    }
}

bool MainWindow::checkExtension(QString extension) {
    int count = ui->filename_comboBox->count();

    for (int i = 0 ; i < count ; ++i) {
        if (ui->filename_comboBox->itemText(i).contains
                (extension ,Qt::CaseInsensitive)) {
            return (true);
        }
    }
    return (false);
}


void MainWindow::on_browse_target_clicked()
{
    QString folderName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/");
    ui->browse_target_edit->setText(folderName);
}

void MainWindow::setEnabledVideo(bool value) {
    ui->videocodec_comboBox->setEnabled(value);
    ui->fps_comboBox->setEnabled(value);
    ui->resolution_comboBox->setEnabled(value);
    ui->quality_spinBox->setEnabled(value);
    ui->fpscustom_checkBox->setEnabled(value);
    ui->bitrate_comboBox->setEnabled(value);
    ui->bitratetarget_spinBox->setEnabled(value);
    ui->pixelformat_comboBox->setEnabled(value);
    ui->customresolution_checkBox->setEnabled(value);
    if (ui->customresolution_checkBox->isChecked() == true && value == true) {
        ui->customresheight_spinBox->setEnabled(value);
        ui->customreswidth_spinBox->setEnabled(value);
    }
    else {
        ui->customresheight_spinBox->setEnabled(false);
        ui->customreswidth_spinBox->setEnabled(false);
    }
    if (ui->fpscustom_checkBox->isChecked() == true && value == true) {
        ui->customfps_spinBox->setEnabled(value);
    }
    else {
        ui->customfps_spinBox->setEnabled(false);
    }
}

void MainWindow::setEnabledAudio(bool value) {
    ui->audiocodec_comboBox->setEnabled(value);
    ui->audio_custombitrate_checkBox->setEnabled(value);
    ui->audiobitrate_comboBox->setEnabled(value);
    ui->audio_format_comboBox->setEnabled(value);
    if (ui->audio_custombitrate_checkBox->isChecked() == true && value == true){
        ui->audio_custombitrate_spinBox->setEnabled(value);
    }
    else {
        ui->audio_custombitrate_spinBox->setEnabled(false);
    }
}

void MainWindow::on_copyvideocodec_checkbox_clicked(bool checked)
{
    if (checked == false) {
        setEnabledVideo(true);
    }
    else {
        setEnabledVideo(false);
    }
}

void MainWindow::on_copyaudiocodec_checkbox_clicked(bool checked)
{
    if (checked == false) {
        setEnabledAudio(true);
    }
    else {
        setEnabledAudio(false);
    }
}

void MainWindow::on_fpscustom_checkBox_clicked(bool checked)
{
    if (checked == true) {
        ui->customfps_spinBox->setEnabled(true);
        ui->customfps_spinBox->setValue(ui->fps_comboBox->itemText(ui->fps_comboBox->currentIndex()).toInt());
        ui->fps_comboBox->setEnabled(false);
    }
    else {
        ui->customfps_spinBox->setEnabled(false);
        ui->fps_comboBox->setEnabled(true);
    }
}

void MainWindow::defaultParams() {
    if (ui->tabWidget->currentIndex() != 0) {
        ui->tabWidget->setCurrentIndex(0);
    }
    ui->browse_line_edit->setText("");
    ui->browse_target_edit->setText("");
    ui->filename_edit->setText("");
    ui->browse_target->setEnabled(false);
    ui->browse_target_edit->setEnabled(false);
    ui->filename_edit->setEnabled(false);
    ui->filename_comboBox->setEnabled(false);
    ui->parameters_tab->setEnabled(false);
}

void MainWindow::on_customresolution_checkBox_clicked(bool checked)
{
    if (checked == true) {
        ui->resolution_comboBox->setEnabled(false);
        ui->customresheight_spinBox->setEnabled(true);
        ui->customreswidth_spinBox->setEnabled(true);
    }
    else {
        ui->resolution_comboBox->setEnabled(true);
        ui->customresheight_spinBox->setEnabled(false);
        ui->customreswidth_spinBox->setEnabled(false);
    }
}

void MainWindow::on_video_checkBox_clicked(bool checked)
{
    if (checked == true) {
        ui->copyvideocodec_checkbox->setEnabled(true);
        ui->copyvideocodec_checkbox->setChecked(true);
        setEnabledVideo(false);
    }
    else {
        ui->copyvideocodec_checkbox->setEnabled(false);
        setEnabledVideo(false);
    }
}

void MainWindow::on_audio_checkBox_clicked(bool checked)
{
    if (checked == true) {
        ui->copyaudiocodec_checkbox->setEnabled(true);
        ui->copyaudiocodec_checkbox->setChecked(true);
        setEnabledAudio(false);
    }
    else {
        ui->copyaudiocodec_checkbox->setEnabled(false);
        setEnabledAudio(false);
    }
}

void MainWindow::on_audio_custombitrate_checkBox_clicked(bool checked)
{
    if (checked == true) {
        ui->audio_custombitrate_spinBox->setEnabled(true);
        ui->audiobitrate_comboBox->setEnabled(false);
    }
    else {
        ui->audio_custombitrate_spinBox->setEnabled(false);
        ui->audiobitrate_comboBox->setEnabled(true);
    }
}

double MainWindow::convertQStringToSecond(QString time)
{
    double final = 0;

    QStringList mytime = time.split(":");
    if (mytime.size() >= 3) {
        final += mytime.at(0).toDouble() * 3600;
        final += mytime.at(1).toDouble() * 60;
        final += mytime.at(2).toDouble();
    }
    else {
        displayError(tr("Error on calculate size from time."));
    }
    return (final);
}


void MainWindow::updateAudioCodec(QList<Mloading::m_data> a_codecs) {
    for (int i = 0 ; i < a_codecs.size() ; ++i) {
        Mloading::m_data tmp = a_codecs.at(i);
        ui->audiocodec_comboBox->addItem(tmp.m_name + " (" + tmp.m_flag + ")");
    }
}

void MainWindow::updateVideoCodec(QList<Mloading::m_data> v_codecs) {
    for (int i = 0 ; i < v_codecs.size() ; ++i) {
        Mloading::m_data tmp = v_codecs.at(i);
        ui->videocodec_comboBox->addItem(tmp.m_name + " (" + tmp.m_flag + ")");
    }
    // Default Codec comboBox

    int index = ui->videocodec_comboBox->findText("HEVC");
    if(index >= 0) {
        ui->videocodec_comboBox->setCurrentIndex(index);
    }
}

void MainWindow::updatePxlFmt(QList<Mloading::m_data> a_pixfmt) {
    for (int i = 0 ; i < a_pixfmt.size() ; ++i) {
        Mloading::m_data tmp = a_pixfmt.at(i);
        ui->pixelformat_comboBox->addItem(tmp.m_flag + " BpP:" + tmp.m_name);
    }
}

void MainWindow::updateDuration(double value, int id) {
    m_fileinfo.at(getPosById(id))->setDuration(value);
}

void MainWindow::updateFps(QString value, int id) {

    m_fileinfo.at(getPosById(id))->setFps(value);

    // Adding FPS Params

    m_fpslist.clear();

    m_fpslist.push_back("24");
    m_fpslist.push_back("25");
    m_fpslist.push_back("30");
    m_fpslist.push_back("50");
    m_fpslist.push_back("60");
    m_fpslist.push_back("120");

    if (!m_fpslist.contains(value, Qt::CaseInsensitive)) {
        m_fpslist.push_back(value);
        qSort(m_fpslist.begin(), m_fpslist.end(), specialFpsSort<QString>());
    }
    ui->fps_comboBox->addItems(m_fpslist);

    if (!value.isEmpty()) {
        int index = ui->fps_comboBox->findText(value);
        ui->fps_comboBox->setCurrentIndex(index);
        ui->fps_comboBox->setItemText(index, value + " (As source)");
    }
    else {
        ui->fps_comboBox->setCurrentIndex(ui->fps_comboBox->findText("50"));
    }
}

void MainWindow::addCommand(QString & command) { 
    // Mfileinfo: command
    // Mfileinfo: encode
    Mencode* encode = new Mencode;

    connect(encode, SIGNAL(sendFileProgress(QString, int)),
            this, SLOT(updateFileProgress(QString, int)));
    connect(encode, SIGNAL(sendFileEnd(int)),
            this, SLOT(updateFileEnd(int)));
    connect(encode, SIGNAL(sendFileError(QString, int)),
            this, SLOT(updateFileError(QString, int)));

    QStringList parts = ui->browse_line_edit->text().split("/");
    QString lastBit = parts.at(parts.size() - 1);

    int pos = getPosById(m_id);

    m_fileinfo.at(pos)->setName(lastBit);
    m_fileinfo.at(pos)->setNametarget(ui->filename_edit->text() + "." + ui->filename_comboBox->currentText());
    m_fileinfo.at(pos)->setFoldertarget(ui->browse_target_edit->text());
    m_fileinfo.at(pos)->setCommand(command);
    m_fileinfo.at(pos)->setEncode(encode);
    m_fileinfo.at(pos)->setEncoded(false);
    m_fileinfo.at(pos)->setRow(ui->tableWidget->rowCount());


    QTableWidgetItem *item_name = new QTableWidgetItem;
    QTableWidgetItem *item_target = new QTableWidgetItem;

    item_name->setText(m_fileinfo.at(pos)->getName());

    item_target->setText(m_fileinfo.at(pos)->getTarget());
    QProgressBar *bar = new QProgressBar;
    bar->setAlignment(Qt::AlignCenter);
    bar->setValue(0);

    bar->setFormat(tr("Not started"));

    ui->tableWidget->insertRow(ui->tableWidget->rowCount());

    int row = getRowById(m_id);

    ui->tableWidget->setItem(row, 0, item_name);
    ui->tableWidget->setItem(row, 1, item_target);
    ui->tableWidget->setCellWidget(row, 2, bar);

    m_id++;

    defaultParams();

    if (ui->tabWidget->currentIndex() != 0) {
        ui->tabWidget->setCurrentIndex(0);
    }
    // create history

    QFile outFile("history" + m_fileinfo.at(pos)->getTarget() + "_log.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << command << endl;
}

int MainWindow::getPosByRow(int row) {
    for (int i = 0 ; i < m_fileinfo.size() ; ++i) {
        if (m_fileinfo.at(i)->getRow() == row) {
            return (i);
        }
    }
    return (-1);
}

int MainWindow::getRowById(int id) {
    for (int i = 0 ; i < m_fileinfo.size() ; ++i) {
        if (m_fileinfo.at(i)->getId() == id) {
            return (m_fileinfo.at(i)->getRow());
        }
    }
    return (-1);
}

int MainWindow::getPosById(int id) {
    for (int i = 0 ; i < m_fileinfo.size() ; ++i) {
        if (m_fileinfo.at(i)->getId() == id) {
            return (i);
        }
    }
    return (-1);
}

void MainWindow::execNext() {
    for (int i = 0 ; i < m_fileinfo.size(); ++i) {
        if (m_fileinfo.at(i)->getEncoded() == false) {
            m_fileinfo.at(i)->getEncode()->exec(m_fileinfo.at(i)->getCommand(), i);
            m_previous = 0;
            return;
        }
    }
}

void MainWindow::updateFileEnd(int id) {
    QProgressBar *bar = qobject_cast<QProgressBar*>(ui->tableWidget->cellWidget(getRowById(id), 2));
    bar->setValue(100);
    bar->setFormat(tr("Encoded"));

    int x = getPosById(id);

    m_fileinfo.at(x)->setEncoded(true);
    m_fileinfo.at(x)->setIsRunning(false);
    execNext();

}

void MainWindow::updateFileError(QString error, int id) {
    QProgressBar *bar = qobject_cast<QProgressBar*>(ui->tableWidget->cellWidget(getRowById(id), 2));
    bar->setValue(100);
    QPalette p = bar->palette();
    p.setColor(QPalette::Highlight, Qt::red);
    bar->setPalette(p);
    bar->setFormat(tr("Error"));

    int x = getPosById(id);

    m_fileinfo.at(x)->setEncoded(true);

    execNext();

    displayError(tr("Its seems ") + m_fileinfo.at(x)->getName() + tr(" has an error :( Log file has been created."));
    QFile outFile(m_fileinfo.at(x)->getTarget() + "_log.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << error << endl;
}

void MainWindow::updateFileProgress(QString time, int id) {
    QStringList timesplit = time.split("=");

    double updatedtime = convertQStringToSecond(timesplit.at(1));
    double pourcentage =(updatedtime * 100) / m_fileinfo.at(getPosById(id))->getDuration();
    if (pourcentage >= 100) {
        pourcentage = 100;
    }

    if (pourcentage < m_previous)
        return;

    int valueprogress = (int)(ceil(pourcentage));
    if (valueprogress >= 100) {
        valueprogress = 100;
    }

    QProgressBar *bar = (qobject_cast<QProgressBar*>(ui->tableWidget->cellWidget(getRowById(id), 2)));
    bar->setValue(valueprogress);
    m_previous = pourcentage;
    bar->setFormat(QString::number(pourcentage) + "%");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    (void) event;
    for (int i = 0 ; i < m_fileinfo.size() ; ++i) {
        if (m_fileinfo.at(i)->getIsRunning() == true) {
            m_fileinfo.at(i)->getEncode()->getCurrentProcess()->kill();
        }
    }
}

void MainWindow::on_audio_format_comboBox_currentTextChanged(const QString &arg1)
{
    if (arg1 == "mono") {
        ui->audiocodec_comboBox->setCurrentIndex(ui->audiocodec_comboBox->findText("AAC (aac)"));
    }
}

void MainWindow::on_actionQuit_triggered()
{
    for (int i = 0 ; i < m_fileinfo.size() ; ++i) {
        if (m_fileinfo.at(i)->getIsRunning() == true) {
            m_fileinfo.at(i)->getEncode()->getCurrentProcess()->kill();
        }
    }
    exit(EXIT_SUCCESS);
}

void MainWindow::on_actionDon_triggered()
{
    QString link = "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=max%2edesplanches%40gmail%2ecom&lc=FR&item_name=video%2dsoftware&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_SM%2egif%3aNonHosted";
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_actionAdd_file_triggered()
{
    if (ui->browse_line_edit->text() == "") {
        displayError(tr("There is something wrong with source file."));
        return;
    }
    else if (!QDir(ui->browse_target_edit->text()).exists()) {
        displayError(tr("There is something wrong with targeted folder."));
        return;
    }
    else if (ui->filename_edit->text() == "") {
        displayError(tr("There is something wrong with targeted file."));
        return;
    }

    QString command = "ffmpeg -i ";

    // file IN

    command += "\"" + ui->browse_line_edit->text() + "\" ";

    // Video codec

    if (ui->video_checkBox->isChecked() && !ui->copyvideocodec_checkbox->isChecked()) {

        // Codec

        QString codec = ui->videocodec_comboBox->itemText(ui->videocodec_comboBox->currentIndex());
        codec = (codec.split("(").at(1));
        codec = codec.replace(")", "");
        command += "-codec:v " + codec + " ";

        // Framerate

        QString framerate = "";

        if (ui->fpscustom_checkBox->isChecked()) {
            framerate = ui->customfps_spinBox->text();
        }
        else {
            framerate = ui->fps_comboBox->itemText(ui->fps_comboBox->currentIndex());
        }
        QStringList frameratelist = framerate.split(" ");
        command += "-r " + frameratelist.at(0) + " ";

        // Resolution

        QString resolution = "";

        if (ui->customresolution_checkBox->isChecked()) {
            resolution = ui->customreswidth_spinBox->text() + "x" + ui->customresheight_spinBox->text();
        }
        else {
            resolution = ui->resolution_comboBox->itemText(ui->resolution_comboBox->currentIndex());
            resolution = resolution.split(" ").at(0);
        }
        command += "-s " + resolution + " ";

        // Quality

        command += "-crf " + ui->quality_spinBox->text() + " ";

        // Bitrate

        if (ui->bitrate_comboBox->currentText() == "Constant") {
            command +=  "-b:v " + ui->bitratetarget_spinBox->text() +
                    "k -minrate " + ui->bitratetarget_spinBox->text() +
                    "k -maxrate "+ ui->bitratetarget_spinBox->text() +
                    "k -bufsize " + QString::number(ui->bitratetarget_spinBox->text().toLong() / 2) + "k ";
        }
        else {
            command +=  "-b:v " + ui->bitratetarget_spinBox->text() + "k ";
        }

        // Pixel format

        QString pix_fmt = ui->pixelformat_comboBox->itemText(ui->pixelformat_comboBox->currentIndex());

        command += "-pix_fmt " + pix_fmt.split(" ").at(0) + " ";

        // For fun

        command += "-strict experimental ";
    }
    else if (ui->video_checkBox->isChecked() && ui->copyvideocodec_checkbox->isChecked()) {
        command += "-vcodec copy ";
    }
    else {
        command += "-vn ";
    }

    // Audio codec

    if (ui->audio_checkBox->isChecked() && !ui->copyaudiocodec_checkbox->isChecked()) {

        // Audio Format

        if (ui->audio_format_comboBox->currentText() == "mono") {
            command += "-ac 1 -strict -2 ";
        }

        // Codec

        command += "-codec:a " + ui->audiocodec_comboBox->itemText(ui->audiocodec_comboBox->currentIndex());

        // Bitrate

        if (ui->audio_custombitrate_checkBox->isChecked()) {
            command += "-b:a " + ui->audio_custombitrate_spinBox->text() + " ";
        }
        else {
            command += "-b:a " + ui->audiobitrate_comboBox->itemText(ui->audiobitrate_comboBox->currentIndex());
        }
    }
    else if (ui->audio_checkBox->isChecked() && ui->copyaudiocodec_checkbox->isChecked()) {
        command += "-acodec copy ";
    }
    else {
        command += "-an ";
    }

    // file OUT

    command += "\"" + ui->browse_target_edit->text() + "\\" + ui->filename_edit->text().replace(".", "");
    command += "." + ui->filename_comboBox->currentText() + "\"";

    addCommand(command);
}

void MainWindow::on_actionTwitter_triggered()
{
    QString link = "https://twitter.com/max_desplanches";
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_actionDelete_selected_row_triggered()
{
    int process;

    process = 0;
    QModelIndexList indexList = ui->tableWidget->selectionModel()->selectedRows();
    foreach (QModelIndex index, indexList) {
        // kill running process
        int pos = getPosByRow(index.row());
        if (m_fileinfo.at(pos)->getIsRunning() == true) {
            m_fileinfo.at(pos)->getEncode()->getCurrentProcess()->kill();
            process = 1;
            // To do: delete file
        }
        m_fileinfo.removeAt(pos);
    }

    foreach (QModelIndex index, indexList) {
        ui->tableWidget->removeRow(index.row());
    }

    for (int i = 0 ; i < m_fileinfo.size() ; ++i) {
        if (m_fileinfo.at(i)->getRow() > 0) {
            m_fileinfo.at(i)->setRow(m_fileinfo.at(i)->getRow() - 1);
        }
    }
    if (process > 0) {
        execNext();
    }
}

void MainWindow::on_actionEncode_triggered()
{
    for (int i = 0 ; i < m_fileinfo.size(); ++i) {
        if (m_fileinfo.at(i)->getEncoded() == false) {
            m_fileinfo.at(i)->setIsRunning(true);
            m_fileinfo.at(i)->getEncode()->exec(m_fileinfo.at(i)->getCommand(), m_fileinfo.at(i)->getId());
            ui->browse_line_edit->setEnabled(false);
            m_previous = 0;
            return;
        }
    }
    displayError(tr("There is nothing to encode in file list: Add file (CTRL+A) to begin tasks."));
}

void MainWindow::on_actionExpert_mode_triggered(bool checked)
{
    isExpert = checked;
    ui->pixelformat_comboBox->setHidden(checked);
    ui->pixelformat_label->setHidden(checked);
    ui->pixelformat_comboBox->setEnabled(!checked);
    ui->pixelformat_label->setEnabled(!checked);
}
