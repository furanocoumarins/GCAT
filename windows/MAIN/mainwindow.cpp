#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtConcurrent>
#include <QFutureWatcher>

#include <QFileDialog>
#include "../../utils/utils.h"

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loaderMovie = new QMovie("assets/shark.gif", QByteArray(), this);

    if (ui->popUP) {
        ui->popUP->setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint);
        ui->popUP->setFixedSize(300, 300);

        if (ui->loaderLabel) {
            ui->loaderLabel->setMovie(loaderMovie);
            ui->loaderLabel->setScaledContents(true);
        }

        ui->popUP->hide();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::is_endposValid(int value){
    int start = ui->startPOSITION->value();
    value = value < start ? ++start : value;
    ui->endPOSITION->setValue(value);
}

void MainWindow::clear_style(QWidget * widgetName){
        widgetName->setStyleSheet("");
}

void MainWindow::set_errstyle(QWidget * widgetName, char type='a'){
    switch (type){
        case 'a': widgetName->setStyleSheet("background-color: rgb(255, 0, 0);");break;
        case 'b': widgetName->setStyleSheet("background-color : rgb(200, 100, 100)");break;
        }
}

void MainWindow::on_mainBUTTON_released()
{
    enum conversionTYPE {ANY, TRANSITION, TRANSVERSION};
    enum placeTYPE {GEN, EXON, CDS, OTHER};
    bool optionsISvalid = true;

    QString conversionsEFURL = ui->inputfileEXportBUTTON->property("fileURL").toString();
    if (conversionsEFURL.isNull()){
        set_errstyle(ui->inputfileEXportBUTTON);
        optionsISvalid = false;
    }
    QString target = ui->targetEDIT->text();
    if (target.isEmpty()){
        set_errstyle(ui->targetEDIT, 'b');
        optionsISvalid = false;
    }
    std::vector<placeTYPE> ptypeFLAGS;
    if (ui->genPLACE->isChecked()) ptypeFLAGS.push_back(GEN);
    if (ui->exonPLACE->isChecked()) ptypeFLAGS.push_back(EXON);
    if (ui->cdsPLACE->isChecked()) ptypeFLAGS.push_back(CDS);
    if (ui->otherPLACE->isChecked()) ptypeFLAGS.push_back(OTHER);
    if (ptypeFLAGS.empty()){
        set_errstyle(ui->placeOPTIONS, 'b');
        optionsISvalid = false;
    }
    if (!optionsISvalid) {
        return;
    }

    int startPOS = ui->startPOSITION->value();
    int endPOS = ui->endPOSITION->value();

    conversionTYPE ctype;
    if (ui->anyCTYPE->isChecked()) ctype = ANY;
    else if (ui->transitionCTYPE->isChecked()) ctype = TRANSITION;
    else ctype = TRANSVERSION;

    QString anatationEFURL = ui->ncbiBUTTON->property("fileURL").toString();
    if (anatationEFURL.isNull()) {
        if (ui->popUP) {
            QPoint centerPos = this->rect().center() - ui->popUP->rect().center();
            centerPos.setY(centerPos.y() +120);
            centerPos.setX(centerPos.x() +333);
            ui->popUP->move(centerPos);
            ui->popUP->raise();
            ui->popUP->show();
        }
        if (loaderMovie) {
            loaderMovie->start();
        }

        ui->mainBUTTON->setEnabled(false);
        QFuture<bool> future = QtConcurrent::run([]() -> bool {
            if (utils::download_ncbi_genome("gcf.gff.gz")) {
                return utils::unzip();
            }
            return false;
        });
        QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);

        connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher]() {
            if (loaderMovie) {
                loaderMovie->stop();
            }
            if (ui->popUP) ui->popUP->hide();

            ui->mainBUTTON->setEnabled(true);
            if (watcher->result() == true) {
                ui->gfflabel->setText("yspex");
                ui->tabWidget->setCurrentIndex(1);
            } else {
                ui->gfflabel->setText("ошибка");

            }
            watcher->deleteLater();
        });

        watcher->setFuture(future);
    }
    else{
        std::string res = "file";
        ui->gfflabel->setText(QString::fromUtf8(res));
        ui->tabWidget->setCurrentIndex(1);
    }
}

void MainWindow::on_endPOSITION_valueChanged(int value)
{
    is_endposValid(value);
}

void MainWindow::on_startPOSITION_valueChanged(int value)
{
    is_endposValid(ui->endPOSITION->value());
}

void MainWindow::on_anatationEXportBUTTON_released()
{
    QString fileURL = QFileDialog::getOpenFileName(
        this,
        tr("Выбирите файл"),
        "",
        tr("Anatation files (*.gff3)")
    );


    if(fileURL.isNull()){
        ui->anatationEXportBUTTON->setText(default_message_file_export);
        ui->ncbiBUTTON->setEnabled(false);
        ui->ncbiBUTTON->setChecked(true);
    }
    else{
        ui->ncbiBUTTON->setProperty("fileURL", fileURL);
        QString fileName = fileURL.split('/').last();
        fileName = fileName.slice(0, fileName.length()-5);
        ui->anatationEXportBUTTON->setText(fileName);
        ui->ncbiBUTTON->setEnabled(true);
        ui->ncbiBUTTON->setChecked(false);
    }
}

void MainWindow::on_ncbiBUTTON_clicked(bool checked)
{
    if(checked) ui->anatationEXportBUTTON->setText(default_message_file_export);
    ui->ncbiBUTTON->setEnabled(false);
    ui->ncbiBUTTON->setProperty("fileURL", "");
}

void MainWindow::on_inputfileEXportBUTTON_released()
{
    clear_style(ui->inputfileEXportBUTTON);
    QString fileURL = QFileDialog::getOpenFileName(
        this,
        tr("Выбирите файл"),
        "",
        tr("Conversions files (*.vcf)")
    );

    if(fileURL.isNull())
        ui->inputfileEXportBUTTON->setText(default_message_file_export);

    else{
        ui->inputfileEXportBUTTON->setProperty("fileURL", fileURL);
        QString fileName = fileURL.split('/').last();
        fileName = fileName.slice(0, fileName.length()-4);
        ui->inputfileEXportBUTTON->setText(fileName);
    }
}

void MainWindow::on_targetEDIT_editingFinished(){
    clear_style(ui->targetEDIT);
}

void MainWindow::on_genPLACE_released()
{
    clear_style(ui->placeOPTIONS);
}

void MainWindow::on_exonPLACE_released()
{
    clear_style(ui->placeOPTIONS);
}

void MainWindow::on_cdsPLACE_released()
{
    clear_style(ui->placeOPTIONS);
}

void MainWindow::on_otherPLACE_released()
{
    clear_style(ui->placeOPTIONS);
}

