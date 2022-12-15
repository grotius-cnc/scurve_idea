#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <thread>
#include <jog.h>
#include <opengl.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void update();

private slots:
    void on_pushButton_jog_x_plus_pressed();

    void on_pushButton_jog_x_plus_released();

    void on_pushButton_jog_x_min_pressed();

    void on_pushButton_jog_x_min_released();

    void on_lineEdit_vel_max_textChanged(const QString &arg1);

    void on_pushButton_reset_pressed();

private:
    Ui::MainWindow *ui;
    //! Thread.
    std::thread *thread;
    //! Jog.
    jog *myJog;
    //! OpenGl.
    opengl *myOpenGl;
};
#endif
