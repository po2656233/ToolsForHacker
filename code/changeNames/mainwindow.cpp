#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFileDialog>
#include <QDebug>



QFileInfoList GetFileList(QString path)
{
    QDir dir(path);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(int i = 0; i != folder_list.size(); i++)
    {
        QString name = folder_list.at(i).absoluteFilePath();
        QFileInfoList child_file_list = GetFileList(name);
        file_list.append(child_file_list);
    }
    return file_list;
}



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_curDir = tr("");
    m_showInfo = tr("");
    QRegExp regx("[0-9]+$");
    ui->textBrowser->setPlainText("展示文件信息");
    QValidator *validator = new QRegExpValidator(regx );
    ui->lineEdit_3->setValidator( validator );
    ui->lineEdit_4->setValidator( validator );
    m_infolist.clear();
    m_history.clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::GenerateNewFileNames()
{
    m_history.clear();
    if(m_curDir.isEmpty()) return;
    QString prefix = ui->lineEdit->text();
    QString suffix = ui->lineEdit_2->text();

    int number = ui->lineEdit_3->text().toInt();
    int intervalX = ui->lineEdit_4->text().toInt();
    m_infolist = GetFileList(m_curDir);

    QString fileName = "";
    QString strSuffix = "";
    foreach (auto file, m_infolist) {
        if(file.isFile())
        {
            strSuffix =suffix.isEmpty()? file.suffix():suffix;
            if(prefix.isEmpty())
            {
                fileName = file.dir().path()+tr("/%1%2.%3").arg(file.baseName()).arg(number).arg(strSuffix);
            }
            else
            {
                fileName =  file.dir().path()+tr("/%1%2.%3").arg(prefix).arg(number).arg(strSuffix);
            }
            m_history.insert(file.absoluteFilePath(),fileName);
        }
        number += intervalX;
    }
}




void MainWindow::on_pushButton_clicked()
{
    ui->lineEdit_5->clear();


    QString dir = QFileDialog::getExistingDirectory(this,tr("请选择目录"),m_curDir);
    if(dir.isEmpty()) return;
    m_curDir = dir;

    m_showInfo = tr("将要修改的文件:\n");
    GenerateNewFileNames();
    ui->lineEdit_5->setText(m_curDir);
    QMapIterator<QString, QString> i(m_history);
    while (i.hasNext()) {
        i.next();
        m_showInfo = m_showInfo + i.key()+tr(" \n");
    }

    ui->textBrowser->setPlainText(m_showInfo);
}


void MainWindow::on_pushButton_3_clicked()
{
    m_showInfo = tr("正在还原文件:\n");
    QMapIterator<QString, QString> i(m_history);
    while (i.hasNext()) {
        i.next();
        QFileInfo file(i.value());
        if(file.isFile())
        {
            if(QFile::rename(i.value(), i.key()))
            {
                m_showInfo += i.value() + tr("\n-->") + i.key() + tr("成功\n\n");
            }
            else
            {
                m_showInfo += i.value() + tr("\n-->") + i.key() + tr("失败----------------\n\n");
            }
        }
    }
    ui->textBrowser->setPlainText(m_showInfo);
    ui->lineEdit_5->clear();
}

void MainWindow::on_pushButton_2_clicked()
{
    GenerateNewFileNames();
    m_showInfo = tr("预生成的文件:\n");
    QMapIterator<QString, QString> i(m_history);
    while (i.hasNext()) {
        i.next();
        QFileInfo file(i.key());
        if(file.isFile())
        {
            m_showInfo += i.key() + tr(" -->\n") + i.value()+tr("\n\n");
        }
    }
    ui->textBrowser->setPlainText(m_showInfo);
}

void MainWindow::on_pushButton_4_clicked()
{
    m_showInfo = tr("正在批量修改文件名:\n");
    QMapIterator<QString, QString> i(m_history);
    while (i.hasNext()) {
        i.next();
        QFileInfo file(i.key());
        if(file.isFile())
        {

            if(QFile::rename(i.key(), i.value()))
            {
                m_showInfo += i.key() + tr("\n-->\n") + i.value() + tr("成功!\n");
            }
            else
            {
                m_showInfo += i.key() + tr("\n-->") + i.value() + tr("失败----------------\n\n");
            }
        }
    }
    ui->textBrowser->setPlainText(m_showInfo);

    ui->lineEdit_5->clear();
    ui->lineEdit_5->setPlaceholderText("当前目录为空");
}
