#include "login_dialog.h"
#include "ui_login_dialog.h"


Login_Dialog::Login_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login_Dialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Login");
    ui->passwd_lineEdit->setEchoMode(QLineEdit::Password);
}

Login_Dialog::~Login_Dialog()
{
    delete ui;
}

//Ok button
void Login_Dialog::on_ok_pushButton_clicked()
{
    QString userName = ui->user_lineEdit->text();
    QString passwd = ui->passwd_lineEdit->text();
    QByteArray ba, bb;
    QCryptographicHash md(QCryptographicHash::Md5);
    ba.append(passwd);
    md.addData(ba);
    bb = md.result();
    passwd = "";
    passwd.append(bb.toHex());
    passwd[11] = 'e';
    qDebug() << passwd << endl;
    std::ifstream ifs("login.txt");
    std::string str_user;
    std::string str_passwd;
    if(ifs) {
        while(std::getline(ifs, str_user) && std::getline(ifs, str_passwd)) {
            if(userName.toStdString() == str_user && passwd.toStdString() == str_passwd) {
                pass();
                this->hide();
                return;
            }
        }
    }
    QMessageBox::information(this, "Error", "Username or password not correct.");
}

//Cancel button
void Login_Dialog::on_cancel_pushButton_clicked()
{
    QApplication::exit();
}
