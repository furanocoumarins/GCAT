#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtConcurrent>
#include <QFutureWatcher>

#include <QFileDialog>
#include "../../utils/utils.h"
#include "../../widgets/samtools.h"

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    QString cat = QCoreApplication::applicationDirPath() + "/assets/cat-cute.gif";
    QString shark = QCoreApplication::applicationDirPath() + "/assets/shark.gif";

    loaderMovie = new QMovie(cat, QByteArray(), this);
    loaderMovie_2 = new QMovie(shark, QByteArray(), this);


    if (ui->popUP) {
        ui->popUP->setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint);
        ui->popUP->setFixedSize(300, 300);

        if (ui->loaderLabel) {
            ui->loaderLabel->setGeometry(0, 0, 300, 300);
            ui->loaderLabel->setStyleSheet("background: transparent; border: none;");

            ui->loaderLabel->setMovie(loaderMovie);
            ui->loaderLabel->setScaledContents(true);
        }

        ui->popUP->hide();
    }

    if (ui->popUP_2) {
        ui->popUP_2->setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint);
        ui->popUP_2->setFixedSize(300, 300);

        if (ui->loaderLabel_2) {
            ui->loaderLabel_2->setGeometry(0, 0, 300, 300);
            ui->loaderLabel_2->setStyleSheet("background: transparent; border: none;");

            ui->loaderLabel_2->setMovie(loaderMovie_2);
            ui->loaderLabel_2->setScaledContents(true);
        }
        ui->popUP_2->hide();
    }

    homedir = Utils::getHomeDirectory()/".gcat";
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
    bool optionsISvalid = true;

    conversionsEFURL = ui->inputfileEXportBUTTON->property("fileURL").toString().toStdString();
    if (conversionsEFURL.empty()){
        set_errstyle(ui->inputfileEXportBUTTON);
        optionsISvalid = false;
    }

    ptypeFLAGS.clear();
    if (ui->genPLACE->isChecked()) ptypeFLAGS.push_back(Ui::GEN);
    if (ui->exonPLACE->isChecked()) ptypeFLAGS.push_back(Ui::EXON);
    if (ui->cdsPLACE->isChecked()) ptypeFLAGS.push_back(Ui::CDS);
    if (ui->otherPLACE->isChecked()) ptypeFLAGS.push_back(Ui::OTHER);
    if (ptypeFLAGS.empty()){
        set_errstyle(ui->placeOPTIONS, 'b');
        optionsISvalid = false;
    }
    if (!optionsISvalid) {
        return;
    }
    chromosome = ui->chrBOX->currentIndex() + 1;
    startPOS = ui->startPOSITION->value();
    endPOS = ui->endPOSITION->value();

    if (ui->anyCTYPE->isChecked()) ctype = Ui::ANY;
    else if (ui->transitionCTYPE->isChecked()) ctype = Ui::TRANSITION;
    else ctype = Ui::TRANSVERSION;


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
            if (Utils::download_ncbi_genome("gcf.gff.gz")) {
                return Utils::unzip();
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
                ui->tabWidget->setCurrentIndex(1);

                QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
                if (ui->popUP_2) {
                    QPoint centerPos = this->rect().center() - ui->popUP_2->rect().center();
                    centerPos.setY(centerPos.y());
                    centerPos.setX(centerPos.x());
                    ui->popUP_2->move(centerPos);
                    ui->popUP_2->raise();
                    ui->popUP_2->show();
                }
                if (loaderMovie_2) {
                    loaderMovie_2->start();
                }

                QFuture<std::string> filterFuture = QtConcurrent::run(QThreadPool::globalInstance(), [this]() -> std::string {
                    try {
                        return smt::FilterVcf(chromosome, startPOS, endPOS, ptypeFLAGS, conversionsEFURL, "qwe"); //option 5 name out
                    } catch (char const* e) {
                        return std::string("error: ") + e;
                    } catch (const std::string& e) {
                        return "error: " + e;
                    }
                });
                QFutureWatcher<std::string> *filterWatcher = new QFutureWatcher<std::string>(this);

                connect(filterWatcher, &QFutureWatcher<std::string>::finished, this, [this, filterWatcher]() {
                    if (loaderMovie_2) {
                        loaderMovie_2->stop();
                    }
                    if (ui->popUP_2) ui->popUP_2->hide();
                    std::string res = filterWatcher->result();
                    QFile file(QString::fromStdString(res));
                        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            QTextStream in(&file);
                            ui->resultText->setText(in.readAll());
                            file.close();
                        }
                    filterWatcher->deleteLater();
                });

                filterWatcher->setFuture(filterFuture);
            } else {
                ui->resultText->setText("ошибка");

            }
            watcher->deleteLater();
        });

        watcher->setFuture(future);
    }
    else{
        anatationEFURL = "qew";
        ui->resultText->setText(anatationEFURL);
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

