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
    void on_pushButton_pressed();

    void on_pushButton_released();

    void on_endPOSITION_valueChanged(int value);

    void on_startPOSITION_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    void is_endposValid(int value);
};
#endif // MAINWINDOW_H
