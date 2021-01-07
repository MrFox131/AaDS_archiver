#include "mainwindow.h"
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QList>
#include <QTextStream>
#include <QFileDialog>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QTimer>
#include <QEventLoop>
#include <QPushButton>
#include <QTreeWidget>
#include <cstdlib>
#include <QWindow>
#include <QMap>
#include <QErrorMessage>
#include <QMessageBox>
#include <string>
#include <stdio.h>
extern "C" {
#include <unarchiver.h>
#include <archiver.h>
}
#include <windows.h>
#include <QLibrary>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QMenu *file = menuBar()->addMenu("&Файл");
    QAction *show_files = new QAction("&Показать файлы архива");
    connect(show_files, &QAction::triggered, this, &MainWindow::get_archive_structure_triggered);
    QAction *archive_file = new QAction("&Заархивировать файл");
    connect(archive_file, &QAction::triggered, this, &MainWindow::archive_file_triggered);
    QAction *archive_folder = new QAction("&Заархивировать папку");
    connect(archive_folder, &QAction::triggered, this, &MainWindow::archive_folder_triggered);
    QAction *unarchivate_files = new QAction("&Разархивировать");
    connect(unarchivate_files, &QAction::triggered, this, &MainWindow::unarchive_triggered);
    QList<QAction*> actions = {show_files, archive_file, archive_folder, unarchivate_files};
    file->addActions(actions);
    resize(500, 400);
    groupBox = new QGroupBox;
    vLayout = new QVBoxLayout;
    RLE = new QRadioButton("RLE");
    haffman = new QRadioButton("Haffman algorithm");
    confirm = new QPushButton("Confirm");
    vLayout->addWidget(RLE);
    vLayout->addWidget(haffman);
    vLayout->addWidget(confirm);
    groupBox->setLayout(vLayout);
    archivating_method_window = new QWidget;
    archivating_method_window->setLayout(vLayout);
    tree = new QTreeWidget;
}

void MainWindow::unarchive_triggered(){
    auto input_archive_name = QFileDialog::getOpenFileName(this, tr("Open File"), "./", "Archives (*.my_archiver_child)").toStdString();
    auto output_folder_name = QFileDialog::getExistingDirectory(this, tr("Save Directory")).toStdString();
    auto file = fopen(input_archive_name.c_str(), "rb");
    fseek(file, 0, SEEK_END);
    if(ftell(file)==0){
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Архив поврежден");
        fclose(file);
        return;
    }
    unarchivate(input_archive_name.c_str(), output_folder_name.c_str());
}

void MainWindow::archive_file_triggered(){
    auto input_file_name = QFileDialog::getOpenFileName(this, tr("Open File"), "./", "Any file (*)").toStdString();
    auto output_archive_name = QFileDialog::getSaveFileName(this, tr("Save archive"), "./", "Archives (*.my_archiver_child)").toStdString();
    if(input_file_name == "" or output_archive_name == ""){
        return;
    }
    archivating_method_window->setLayout(vLayout);
    archivating_method_window->show();
    QEventLoop loop;
    connect(confirm, &QPushButton::clicked, &loop, &QEventLoop::quit);
    loop.exec();
    archivating_method_window->hide();
    int error_code = archivate(input_file_name.c_str(), output_archive_name.c_str(), RLE->isChecked()?2:1);
    switch (error_code) {
    case -1: {
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Директория для сохранения архива недоступна для записи от имени текущего пользователя");
        auto archive = fopen(output_archive_name.c_str(), "wb");
        fclose(archive);
        break;
    }
    case -2: {
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Один из архивируемых файлов недоступен для чтения от имени текущего пользователя");
        auto archive = fopen(output_archive_name.c_str(), "wb");
        fclose(archive);
        break;
    }
    case -3: {
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Путь к архивиируемой папке или файлу содержит кириллические символы.");
        auto archive = fopen(output_archive_name.c_str(), "wb");
        fclose(archive);
        break;
    }
    default: {
        QMessageBox messageBox;
        messageBox.information(0, "Success", "Архивация успешно завершена");
    }
    }

}

void MainWindow::archive_folder_triggered(){
    auto input_file_name = QFileDialog::getExistingDirectory(this, tr("Open Directory")).toStdString();
    auto output_archive_name = QFileDialog::getSaveFileName(this, tr("Save archive"), "./", "Archives (*.my_archiver_child)").toStdString();
    if(input_file_name == "" or output_archive_name == ""){
        return;
    }
    archivating_method_window->show();
    QEventLoop loop;
    connect(confirm, &QPushButton::clicked, &loop, &QEventLoop::quit);
    loop.exec();
    archivating_method_window->hide();
    int error_code = archivate(input_file_name.c_str(), output_archive_name.c_str(), RLE->isChecked()?2:1);
    switch (error_code) {
    case -1: {
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Директория для сохранения архива недоступна для записи от имени текущего пользователя");
        auto archive = fopen(output_archive_name.c_str(), "wb");
        fclose(archive);
        break;
    }
    case -2: {
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Один из архивируемых файлов недоступен для чтения от имени текущего пользователя");
        auto archive = fopen(output_archive_name.c_str(), "wb");
        fclose(archive);
        break;
    }
    case -3: {
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Путь к архивиируемой папке или файлу содержит кириллические символы.");
        auto archive = fopen(output_archive_name.c_str(), "wb");
        fclose(archive);
        break;
    }
    default: {
        QMessageBox messageBox;
        messageBox.information(0, "Success", "Архивация успешно завершена");
    }
    }
}

void MainWindow::get_archive_structure_triggered(){
    auto archive_path = QFileDialog::getOpenFileName(this, tr("Open Archive"), "./", "Archives (*.my_archiver_child)").toStdString();
    auto file = fopen(archive_path.c_str(), "rb");
    fseek(file, 0, SEEK_END);
    if(ftell(file)==0){
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Архив поврежден");
        return;
    }
    fseek(file, 0, SEEK_SET);
    as = get_archive_structure(file);
    tree->clear();
    tree->setColumnCount(2);
    tree->headerItem()->setText(0, "Имя");
    tree->headerItem()->setText(1, "Размер сжатого файла");
    QMap<QString, QTreeWidgetItem*> items;
    for(int i=0; i<as->files_counter; i++){
        QStringList filename = QString(as->files[i].name).split('\\');
        if (filename.length() == 1){
            filename = filename[0].split('/');
        }
        QString current_path = "";
        QTreeWidgetItem *current_item = tree->invisibleRootItem();
        for (int j=0; j<filename.length(); j++){
            current_path += filename[j];
            if (items[current_path] == nullptr){
                items[current_path] = new QTreeWidgetItem(current_item);
                items[current_path]->setText(0, filename[j]);
            }
            current_item = items[current_path];
            if (j==filename.length()-1){
                current_item->setText(1, QString::number(as->files[i].compressed_size));
            }
        }
    }
    setCentralWidget(tree);
    fclose(file);
}

MainWindow::~MainWindow()
{
}

