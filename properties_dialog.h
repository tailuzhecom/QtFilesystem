#ifndef PROPERTIES_DIALOG_H
#define PROPERTIES_DIALOG_H

#include <QDialog>
#include "filesystem.h"

namespace Ui {
class Properties_Dialog;
}

class Properties_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Properties_Dialog(QWidget *parent = 0);
    void setText(QString name, QString type, QString size, QString path, QString time);
    void setBlockNum(int);
    int getBlockNum();
    ~Properties_Dialog();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

signals:
    void setName(QString);

private:
    Ui::Properties_Dialog *ui;
    int m_block_num;
};

#endif // PROPERTIES_DIALOG_H
