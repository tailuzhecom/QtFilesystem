#include "properties_dialog.h"
#include "ui_properties_dialog.h"

Properties_Dialog::Properties_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Properties_Dialog)
{
    ui->setupUi(this);
    setWindowTitle("Properties");
}

void Properties_Dialog::setText(QString name, QString type, QString size, QString path, QString time)
{
    ui->name_lineEdit->setText(name);
    ui->type_lineEdit->setText(type);
    ui->size_lineEdit->setText(size);
    ui->path_lineEdit->setText(path);
    ui->time_lineEdit->setText(time);
}

void Properties_Dialog::setBlockNum(int block_num)
{
    m_block_num = block_num;
}

int Properties_Dialog::getBlockNum()
{
    return m_block_num;
}

Properties_Dialog::~Properties_Dialog()
{
    delete ui;
}

void Properties_Dialog::on_pushButton_clicked()
{
    emit setName(ui->name_lineEdit->text());
    this->hide();
}

void Properties_Dialog::on_pushButton_2_clicked()
{
    this->hide();
}
