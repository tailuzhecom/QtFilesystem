#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>
#include <fstream>
#include <string>
#include <QMessageBox>
#include <QFile>
#include <QFileDevice>
#include <QCryptographicHash>
#include <QDebug>

namespace Ui {
class Login_Dialog;
}

class Login_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Login_Dialog(QWidget *parent = 0);
    ~Login_Dialog();

signals:
    void pass();


private slots:
    void on_ok_pushButton_clicked();

    void on_cancel_pushButton_clicked();

private:
    Ui::Login_Dialog *ui;
};

#endif // LOGIN_DIALOG_H
