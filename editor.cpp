#include "editor.h"
#include "ui_editor.h"

#include <QDebug>

Editor::Editor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Editor)
{
    ui->setupUi(this);
    setWindowTitle("Young's Simple Editor");
    ui->save_pushButton->setIcon(QIcon(":/img/pic/save.ico"));
    setWindowIcon(QIcon(":/img/pic/edit.ico"));
}

void Editor::setText(QString content)
{
    ui->textEdit->setText(content);
}

QString Editor::getFileName()
{
    return fileName;
}

Editor::~Editor()
{
    delete ui;
}

void Editor::on_save_pushButton_clicked()
{
    emit sendContent(ui->textEdit->toPlainText());
    qDebug() << ui->textEdit->toPlainText() << endl;
}

void Editor::setFileName(QString name)
{
    fileName = name;
}
