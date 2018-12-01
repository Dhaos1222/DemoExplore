#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "mydirmodel.h"
#include "dialog.h"
#include "searchthread.h"

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include <QDirModel>
#include <QTreeView>
#include <QModelIndex>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QVector>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QStringListModel>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void timerUpdate();

    void clickSlot(QModelIndex);//单击选中文件
    void doubleClickSlot(QModelIndex);//文件双击

    void on_backButton_clicked();

    void on_returnButton_clicked();

    void on_forwardButton_clicked();

    //右键槽函数
    void newTxtFile();
    void newExelFile();
    void newWordFile();
    void newDFile();
    void refreshSlot();
    void fileCopySlot();
    void filePasteSlot();
    void fileDeleteSlot();
    void fileRenameSlot();
    void fileOpenSlot();
    void fileDetailSlot();

    void on_version_triggered();

    void receQstring(QStringList);
    void solveSraech();
    void doubleClickInQstringList(QModelIndex);
    void clickCloseSlot();
    void openrask();  //进程管理
    void searchFileSlot();


private:
    Ui::MainWindow *ui;
    QLabel *statusLabel;
    QDirModel *model;
    QModelIndex indexNow;//记录当前浏览位置
    QModelIndex indexChoose;//记录选中文件
    QVector<QModelIndex> indexHistory;//记录所有浏览记录
    int indexPoint;//记录当前浏览位置序号
    bool coverFileIfExist;//是否复制文件
    bool isDelete;//是否是可删除文件
    QString sourceDir;//源文件位置
    QString toDir;//目的文件位置
    QStringList fileAndDirName;  //记录搜索的变量
    QAction *searchFinishAction; //后面搜索按钮要加入

    void contextMenuEvent(QContextMenuEvent *event);//鼠标右键菜单
    void getSourcePath(const QModelIndex &index);//获取选中文件地址
    void getDesPath(const QModelIndex &index);//获取粘贴文件地址
    bool pasteFile(QString sourceDir, QString toDir, bool coverFileIfExist);//粘贴文件
    bool pasteFolder(const QString &fromDir, const QString &toDir, bool coverFileIfExist);//粘贴文件夹
    bool deleteFile(const QString &folderDir);//删除文件
    bool findSameFile(QString filename,QString folderDir);//查找同名文件
};

#endif // MAINWINDOW_H
