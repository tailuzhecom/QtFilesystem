#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDebug>
#include <QStringList>
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    login_dialog = new Login_Dialog(this);
    this->hide();
    login_dialog->show();
    setWindowTitle("FileSystem");
    ui->back_pushButton->setIcon(QIcon(":/img/pic/back.ico"));
    m_editor = new Editor;
    m_abs_trace = "/root";
    m_properties_dialog = new Properties_Dialog(this);
    updateDirView();

    open_action = new QAction("open");
    delete_action = new QAction("delete");
    create_file_action = new QAction("create file");
    create_dir_action = new QAction("create directoriy");
    flesh_action = new QAction("flesh");
    properties_action = new QAction("properties");
    format_action = new QAction("format");
    this->hide();

    connect(m_editor, SIGNAL(sendContent(QString)), this, SLOT(saveContent(QString)));
    connect(open_action, SIGNAL(triggered()), this, SLOT(open_action_slot()));
    connect(flesh_action, SIGNAL(triggered()), this, SLOT(flesh_action_slot()));
    connect(create_file_action, SIGNAL(triggered()), this, SLOT(create_file_action_slot()));
    connect(create_dir_action, SIGNAL(triggered()), this, SLOT(create_dir_action_slot()));
    connect(delete_action, SIGNAL(triggered()), this, SLOT(delete_action_slot()));
    connect(properties_action, SIGNAL(triggered()), this, SLOT(properties_action_slot()));
    connect(m_properties_dialog, SIGNAL(setName(QString)), this, SLOT(name_changed(QString)));
    connect(format_action, SIGNAL(triggered()), this, SLOT(format_action_slot()));
    connect(login_dialog, SIGNAL(pass()), this, SLOT(show()));
}

//Update the current dir's file in the listWidget
void MainWindow::updateDirView()
{
    ui->listWidget->clear();
    m_file_in_current_dir.clear();
    ui->trace_lineEdit->setText(m_abs_trace);
    std::vector<std::vector<std::string> > nameList = m_fs.dir();
    for(const auto& iter : nameList) {
        QListWidgetItem* item =  new QListWidgetItem;
        item->setText(QString::fromStdString(iter[0]));
        if(iter[1] == "FILE")
            item->setIcon(QIcon(":/img/pic/file.ico"));
        else if(iter[1] == "DIR")
            item->setIcon(QIcon(":/img/pic/dir.ico"));
        ui->listWidget->addItem(item);
        for(const auto& fileString: iter)
            m_file_in_current_dir[QString::fromStdString(iter[0])].push_back(QString::fromStdString(fileString));
    }

    for(const auto& iter : m_file_in_current_dir) {
        for(const auto& content : iter)
            qDebug() << content << " ";
        qDebug() << endl;
    }
}

void MainWindow::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu* menu = new QMenu();
    menu->addAction(open_action);
    menu->addSeparator();
    menu->addAction(flesh_action);
    menu->addSeparator();
    menu->addAction(delete_action);
    menu->addSeparator();
    menu->addAction(create_file_action);
    menu->addSeparator();
    menu->addAction(create_dir_action);
    menu->addSeparator();
    menu->addAction(format_action);
    menu->addSeparator();
    menu->addAction(properties_action);

    menu->exec(e->globalPos());
    delete menu;
}



MainWindow::~MainWindow()
{
    delete ui;
}

//Double click item slot;
//If the item is dir, change current dir to the dir clicked.;
//If the item is a normal file, open it.
void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if(m_file_in_current_dir[item->data(0).toString()][1] == "DIR") {  //If the item is dir
        m_fs.changeDir(item->data(0).toString().toLatin1().data());
        m_abs_trace += "/" + item->data(0).toString();
        updateDirView();
    }
    else if(m_file_in_current_dir[item->data(0).toString()][1] == "FILE") {  //If the item is normal file
        std::string content;
        m_fs.read(item->data(0).toString().toLatin1().data(), content);
        m_editor->show();
        m_editor->setFileName(item->data(0).toString());
        m_editor->setText(QString::fromStdString(content));
    }
}

//Back button
void MainWindow::on_back_pushButton_clicked()
{
    if(m_fs.changeDir("..") == -1)
        qDebug() << "Change dir failed." << endl;
    int i = 0;
    for(i = m_abs_trace.size() - 1; i >= 0; i--) {
        if(m_abs_trace[i] == '/')
            break;
    }
    qDebug() << "/ index : " <<  i << endl;
    if(i != 0)
        m_abs_trace = m_abs_trace.left(i);
    updateDirView();
}

//Save the content in editor
void MainWindow::saveContent(QString content)
{
    QString fileName = m_editor->getFileName();
    m_fs.write(fileName.toLatin1().data(), content.toLatin1().data(), COVER);
    updateDirView();
}

void MainWindow::open_action_slot()
{
    QListWidgetItem* item =  ui->listWidget->currentItem();
    if(item == NULL)
        return;
    if(m_file_in_current_dir[item->data(0).toString()][1] == "DIR") {  //If the item is dir
        m_fs.changeDir(item->data(0).toString().toLatin1().data());
        m_abs_trace += "/" + item->data(0).toString();
        updateDirView();
    }
    else if(m_file_in_current_dir[item->data(0).toString()][1] == "FILE") {  //If the item is normal file
        std::string content;
        m_fs.read(item->data(0).toString().toLatin1().data(), content);
        m_editor->show();
        m_editor->setFileName(item->data(0).toString());
        m_editor->setText(QString::fromStdString(content));
    }
}

void MainWindow::flesh_action_slot()
{
    updateDirView();
}

void MainWindow::create_file_action_slot()
{
    bool isOk;
    QString fileName = QInputDialog::getText(this, "Input", "Please input the file's name", QLineEdit::Normal, NULL, &isOk);
    if(isOk) {
        int create_status = m_fs.createFile(fileName.toLatin1().data());
        if(create_status == -1) {
            QMessageBox::warning(this, "Error", "The storage has used up.");
        }
        else if(create_status == -2) {
            QMessageBox::warning(this, "Error", "The file already exists.");
        }
        else {
            updateDirView();
        }
    }
}

void MainWindow::create_dir_action_slot()
{
    bool isOk;
    QString dirName = QInputDialog::getText(this, "Input", "Please input the directory's name", QLineEdit::Normal, NULL, &isOk);
    int create_status = m_fs.mkdir(dirName.toLatin1().data());
    if(create_status == -1) {
        QMessageBox::warning(this, "Error", "The storage has used up.");
    }
    else if(create_status == -2) {
        QMessageBox::warning(this, "Error", "The directory already exists.");
    }
    else {
        updateDirView();
    }
}

void MainWindow::delete_action_slot()
{
    QListWidgetItem* item = ui->listWidget->currentItem();
    if(item == NULL)
        return;
    m_fs.deleteFile(item->data(0).toString().toLatin1().data(), m_fs.getChildren(), false);
    updateDirView();
}

void MainWindow::properties_action_slot()
{
    QListWidgetItem* item = ui->listWidget->currentItem();
    if(item == NULL)
        return;
    QVector<QString> item_info = m_file_in_current_dir[item->data(0).toString()];
    m_properties_dialog->setText(item_info[0], item_info[1], item_info[2], m_abs_trace+"/"+item->data(0).toString(), item_info[3]);
    m_properties_dialog->setBlockNum(m_fs.getBlockNumByName(item->data(0).toString().toLatin1().data()));
    m_properties_dialog->show();
}

void MainWindow::format_action_slot()
{
    auto button = QMessageBox::information(this, "warning", "Do you want to format?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(button == QMessageBox::Yes) {
        m_fs.format();
        updateDirView();
    }
}

void MainWindow::name_changed(QString name)
{
    qDebug() << "name changed" << endl;
    int block_num = m_properties_dialog->getBlockNum();
    if(m_fs.setName(name.toLatin1().data(), block_num) == -1)
        QMessageBox::warning(this, "ERROR", "The file already exists.");
    else
        updateDirView();
}


