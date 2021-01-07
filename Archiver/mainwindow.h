#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QPushButton>
extern "C" {
#include <unarchiver.h>
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Archive_structure* as;
    QTreeWidget *tree;
    QGroupBox *groupBox;
    QVBoxLayout *vLayout;
    QRadioButton *RLE;
    QRadioButton *haffman;
    QPushButton *confirm;
    QWidget *archivating_method_window;
private slots:
    void get_archive_structure_triggered();
    void archive_file_triggered();
    void archive_folder_triggered();
    void unarchive_triggered();
};
#endif // MAINWINDOW_H
