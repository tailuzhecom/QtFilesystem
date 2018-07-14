#ifndef EDITOR_H
#define EDITOR_H

#include <QDialog>
#include <QString>

namespace Ui {
class Editor;
}

class Editor : public QDialog
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = 0);
    void setText(QString);
    QString getFileName();
    void setFileName(QString);
    ~Editor();

private slots:
    void on_save_pushButton_clicked();

signals:
    void sendContent(QString content);

private:
    Ui::Editor *ui;
    QString fileName;
};

#endif // EDITOR_H
