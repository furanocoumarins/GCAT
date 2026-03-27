#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_mainBUTTON_released()
{
    enum conversionTYPE {ANY, TRANSITION, TRANSVERSION};
    enum placeTYPE {GEN, EXON, CDS, OTHER};
    bool optionsISvalid = false;

    QString conversionsEFURL = ui->inputfileEXportBUTTON->property("fileURL").toString();
    if (conversionsEFURL.isNull()){

    }
    QString anatationEFURL = ui->ncbiBUTTON->property("fileURL").toString();
    if (anatationEFURL.isNull()){

    }
    QString target = ui->targetEDIT->text();
    if (target.isNull()){

    }

    int startPOS = ui->startPOSITION->value();
    int endPOS = ui->endPOSITION->value();

    conversionTYPE ctype;
    if (ui->anyCTYPE->isChecked()) ctype = ANY;
    else if (ui->transitionCTYPE->isChecked()) ctype = TRANSITION;
    else ctype = TRANSVERSION;

    std::vector<placeTYPE> ptypeFLAGS;
    if (ui->genPLACE->isChecked()) ptypeFLAGS.push_back(GEN);
    if (ui->exonPLACE->isChecked()) ptypeFLAGS.push_back(EXON);
    if (ui->cdsPLACE->isChecked()) ptypeFLAGS.push_back(CDS);
    if (ui->otherPLACE->isChecked()) ptypeFLAGS.push_back(OTHER);
    if (ptypeFLAGS.empty()){

    }
}

void MainWindow::is_endposValid(int value){
    int start = ui->startPOSITION->value();
    value = value < start ? ++start : value;
    ui->endPOSITION->setValue(value);
}

void MainWindow::on_endPOSITION_valueChanged(int value)
{
    is_endposValid(value);
}

void MainWindow::on_startPOSITION_valueChanged(int value)
{
    is_endposValid(value);
}

void MainWindow::on_anatationEXportBUTTON_released()
{
    QString fileURL = QFileDialog::getOpenFileName(
         this,
         tr("Выбирите файл"), "/home",
         tr("Anatation files (*.gff3)"
    ));
    ui->ncbiBUTTON->setProperty("fileURL", fileURL);
    QString fileName = fileURL.split('/').last();
    fileName = fileName.slice(0, fileName.length()-5);

    if(fileName.isNull()){
         ui->anatationEXportBUTTON->setText(default_message_file_export);
         ui->ncbiBUTTON->setEnabled(false);
         ui->ncbiBUTTON->setChecked(true);
    }
    else{
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
    QString fileURL = QFileDialog::getOpenFileName(
         this,
         tr("Выбирите файл"), "/home",
         tr("Conversions files (*.vcf)"
           ));
    ui->inputfileEXportBUTTON->setProperty("fileURL", fileURL);
    QString fileName = fileURL.split('/').last();
    fileName = fileName.slice(0, fileName.length()-4);

    if(fileName.isNull())
         ui->inputfileEXportBUTTON->setText(default_message_file_export);
    else
         ui->inputfileEXportBUTTON->setText(fileName);
}

