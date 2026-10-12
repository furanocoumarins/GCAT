#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_mainBUTTON_released();

    void on_endPOSITION_valueChanged(int value);

    void on_startPOSITION_valueChanged(int value);

    void on_anatationEXportBUTTON_released();

    void on_ncbiBUTTON_clicked(bool checked);

    void on_inputfileEXportBUTTON_released();

    void on_targetEDIT_editingFinished();

    void on_genPLACE_released();

    void on_exonPLACE_released();

    void on_cdsPLACE_released();

    void on_otherPLACE_released();

private:
    Ui::MainWindow *ui;

    void is_endposValid(int value);

    void set_errstyle(QWidget * widgetName, char type);
    void clear_style(QWidget * widgetName);

    QString default_message_file_export = "Выбрать файл";
};
#endif // MAINWINDOW_H
