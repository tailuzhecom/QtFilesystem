#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QTreeView>
#include <QTreeWidget>
#include <QMap>
#include <QListWidgetItem>
#include <QContextMenuEvent>

#include <vector>
#include <string>
#include <map>

#include "filesystem.h"
#include "editor.h"
#include "properties_dialog.h"
#include "login_dialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void updateDirView();
    void contextMenuEvent(QContextMenuEvent* e);
    ~MainWindow();

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_back_pushButton_clicked();
    void saveContent(QString);
    void open_action_slot();
    void flesh_action_slot();
    void create_file_action_slot();
    void create_dir_action_slot();
    void delete_action_slot();
    void properties_action_slot();
    void format_action_slot();
    void name_changed(QString name);


private:
    Ui::MainWindow *ui;

    FileSystem m_fs;
    QMap<QString, QVector<QString> > m_file_in_current_dir;
    QString m_abs_trace;
    Editor* m_editor;
    Properties_Dialog* m_properties_dialog;
    Login_Dialog* login_dialog;

    QAction* open_action;
    QAction* delete_action;
    QAction* create_file_action;
    QAction* create_dir_action;
    QAction* flesh_action;
    QAction* properties_action;
    QAction* format_action;
};

#endif // MAINWINDOW_H
