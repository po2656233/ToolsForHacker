#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QStringList>
#include <QTextStream>
#include <QDebug>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

QFileInfoList GetFileList(QString path)
{
    QDir dir(path);
    if(!dir.exists())
        return QFileInfoList();
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    //过滤文件
    QStringList fileList;
    fileList<<"*.proto";
    dir.setNameFilters(fileList);
    QFileInfoList file_list = dir.entryInfoList(fileList, QDir::Files|QDir::AllEntries);

    for(int i = 0; i < folder_list.size(); i++)
    {
        QString name = folder_list.at(i).absoluteFilePath();
        QFileInfoList child_file_list = GetFileList(name);
        file_list.append(child_file_list);
    }

    return file_list;
}



void Widget::on_pushButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,tr("导入文件列表"),QDir::currentPath());

    QStringList names;
    QFileInfoList fList = GetFileList(dir);
    foreach (QFileInfo info, fList) {
        names<<info.fileName();
        fileNames<<info.filePath();
    }
    //fileNames =  QFileDialog::getOpenFileNames(this,tr("导入文件列表"),QDir::currentPath(),tr("*.proto"));
    QString strContent = names.join("\n");
    ui->textBrowser_file->setText(strContent);
}

void Widget::on_pushButton_2_clicked()
{
    if (fileNames.empty()) return;

    QString line;
    QString strDest;
    // QRegExp sep("?<=message.*?(?={)");
    QString strV = ui->lineEdit->text();



    QFile fileTxt("outProto.txt");
    if (!fileTxt.open( QIODevice::ReadWrite | QIODevice::Text))
        return;

    QTextStream out(&fileTxt);
    foreach (QString name, fileNames) {
        QFile file(name);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            break;


        QTextStream in(&file);
        while (!in.atEnd()) {
            line = in.readLine();

            if(line.contains("message") && line.contains("{"))
            {
                line.replace("message","");
                line.replace("{","");
                line.replace(" ","");

                qDebug()<<line;
                strDest = strV;
                strDest.replace("*",line);
                out << strDest << "\n";
            }
        }
        file.close();
    }
       //提取message
    ui->textBrowserShow->setText(out.readAll());
    fileTxt.close();



}
