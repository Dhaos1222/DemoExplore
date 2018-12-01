#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "task.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //实现时钟功能
    QTimer *timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&MainWindow::timerUpdate);
    timer->start(1000);
    statusLabel = new QLabel;
    statusLabel->setMinimumSize(150,20);
    statusLabel->setFrameShape(QFrame::WinPanel);
    statusLabel->setFrameShadow(QFrame::Sunken);
    ui->statusBar->addWidget(statusLabel);
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    statusLabel->setText(str);


    //实现文件浏览基本功能
    //创建文件模型
    model = new MyDirModel;
    model->setSorting(QDir::DirsFirst|QDir::IgnoreCase|QDir::Name);//排序
    //树形视图
    ui->treeView->setModel(model);
    ui->treeView->setColumnHidden(1,true);
    ui->treeView->setColumnHidden(2,true);
    ui->treeView->setColumnHidden(3,true);
    //列表视图
    ui->listView->setModel(model);

    //以下为变量定义区域
    indexHistory.append(model->index(":/"));
    indexPoint = indexHistory.size()-1;
    //连接槽函数
    QObject::connect(ui->listView,SIGNAL(doubleClicked(QModelIndex)) ,this,SLOT(doubleClickSlot(QModelIndex)));
    QObject::connect(ui->treeView,SIGNAL(doubleClicked(QModelIndex)) ,this,SLOT(doubleClickSlot(QModelIndex)));
    QObject::connect(ui->listView,SIGNAL(clicked(QModelIndex)) ,this,SLOT(clickSlot(QModelIndex)));
    QObject::connect(ui->treeView,SIGNAL(clicked(QModelIndex)) ,this,SLOT(clickSlot(QModelIndex)));
    QObject::connect(ui->open,SIGNAL(triggered(bool)) ,this,SLOT(fileOpenSlot()));
    QObject::connect(ui->newTxt,SIGNAL(triggered(bool)) ,this,SLOT(newTxtFile()));
    QObject::connect(ui->newWord,SIGNAL(triggered(bool)) ,this,SLOT(newWordFile()));
    QObject::connect(ui->newExcel,SIGNAL(triggered(bool)) ,this,SLOT(newExelFile()));
    QObject::connect(ui->copy,SIGNAL(triggered(bool)) ,this,SLOT(fileCopySlot()));
    QObject::connect(ui->paste,SIGNAL(triggered(bool)) ,this,SLOT(filePasteSlot()));
    QObject::connect(ui->delete_2,SIGNAL(triggered(bool)) ,this,SLOT(fileDeleteSlot()));
    QObject::connect(ui->rename,SIGNAL(triggered(bool)) ,this,SLOT(fileRenameSlot()));
    QObject::connect(ui->searchButton,SIGNAL(clicked(bool)),this,SLOT(searchFileSlot()));
    QObject::connect(ui->check,SIGNAL(triggered(bool)) ,this,SLOT(openrask()));
    //按钮图标
//    ui->backButton->setStyleSheet("QPushButton{border-image: url(:/myimages/imgaes/arrow_left.png);}");
//    ui->forwardButton->setStyleSheet("QPushButton{background-image: url(:/myimages/images/arrow_right.png);}");
//    ui->returnButton->setStyleSheet("QPushButton{background-image: url(:/myimages/images/back.png);}");
}

MainWindow::~MainWindow()
{
    delete ui;
}
//时间更新函数
void MainWindow::timerUpdate()
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    statusLabel->setText(str);
}
//单击获取文件位置
void MainWindow::clickSlot(QModelIndex a)
{
    indexChoose = a;
    isDelete = true;
}
//鼠标双击函数
void MainWindow::doubleClickSlot(QModelIndex a)
{
    indexNow = a; //存储QModelIndex打开的文件的路径
    QFileInfo file = model->fileInfo(a);
    if(file.isDir()) //判断点击文件是否为文件夹，是则打开文件夹，否则是文件，打开文件内容
    {
        indexHistory.remove(indexPoint+1,indexHistory.size()-indexPoint-1);
        indexHistory.append(a);
        indexPoint = indexHistory.size()-1;
        ui->listView->setRootIndex(a);
        QString filePath = model->filePath(a);
        ui->filePathEdit->setText(filePath);
        ui->searchLineEdit->setPlaceholderText(QStringLiteral("搜索“")+filePath+QStringLiteral("”"));
    }
    else
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(file.absoluteFilePath()));//用对应程序打开文件
    }
}
//后退按钮
void MainWindow::on_backButton_clicked()
{
    if (indexPoint > 0)
    {
        ui->listView->setRootIndex(indexHistory.at(--indexPoint));
        QString filePath = model->filePath(indexHistory.at(indexPoint));
        ui->filePathEdit->setText(filePath);
        ui->searchLineEdit->setPlaceholderText(QStringLiteral("搜索“")+filePath+QStringLiteral("”"));
    }
    //juiceButton();
    //QModelIndex ti=ui->listView->currentIndex();
    //ui->tableView->setRootIndex(ti);
}
//返回按钮
void MainWindow::on_returnButton_clicked()
{
    //找到上一级结点
    indexNow = ui->listView->model()->parent(indexNow);
    //转到上一级结点
    ui->listView->setRootIndex(indexNow);
    indexHistory.remove(indexPoint+1,indexHistory.size()-indexPoint-1);
    indexHistory.append(indexNow);
    indexPoint = indexHistory.size()-1;
    //ui->tableView->setRootIndex(indexNow);
    QString filePath = model->filePath(indexNow);
    ui->filePathEdit->setText(filePath);
    ui->searchLineEdit->setPlaceholderText(QStringLiteral("搜索“")+filePath+QStringLiteral("”"));
}

void MainWindow::on_forwardButton_clicked()
{
    if (indexPoint+1 != indexHistory.size())
    {
        ui->listView->setRootIndex(indexHistory.at(++indexPoint));
        QString filePath = model->filePath(indexHistory.at(indexPoint));
        ui->filePathEdit->setText(filePath);
        ui->searchLineEdit->setPlaceholderText(QStringLiteral("搜索“")+filePath+QStringLiteral("”"));
    }
    //juiceButton();
    //QModelIndex ti=ui->listView->currentIndex();
    //ui->tableView->setRootIndex(ti);
}
//获取选中文件路径
void MainWindow::getSourcePath(const QModelIndex &index)
{
    sourceDir = model->fileInfo(index).absoluteFilePath();
    sourceDir.replace("/","\\");
    isDelete = true;
}
//获取粘贴文件路径
void MainWindow::getDesPath(const QModelIndex &index)
{
    toDir = model->fileInfo(index).absoluteFilePath();
    toDir.replace("/","\\");
    QFileInfo file(sourceDir);
    toDir += "/"+file.fileName();
    isDelete = true;
}
//实现粘贴文件功能
bool MainWindow::pasteFile(QString sourceDir, QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\","/");
    sourceDir.replace("\\","/");
    if (sourceDir == toDir){
        return true;
    }
    if (!QFile::exists(sourceDir)){
        return false;
    }
    QDir *createfile     = new QDir;
    bool exist = createfile->exists(toDir);
    if (exist){
        if(coverFileIfExist){
            createfile->remove(toDir);
        }
    }//end if
    QFileInfo file(sourceDir);
    if(!QFile::copy(sourceDir, toDir))
    {
        return false;
    }
    return true;
}
//实现粘贴文件夹功能
bool MainWindow::pasteFolder(const QString &fromDir, const QString &toDir, bool coverFileIfExist)
{
        QFileInfo file(fromDir);
        QDir sourceDir(fromDir);
        QDir targetDir(toDir);//+"/"+file.fileName());

        if(!targetDir.exists()){    /*< 如果目标目录不存在，则进行创建 */
            if(!targetDir.mkdir(targetDir.absolutePath()))
                return false;
        }
        QFileInfoList fileInfoList = sourceDir.entryInfoList();
        foreach(QFileInfo fileInfo, fileInfoList){
            if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
                continue;

            if(fileInfo.isDir()){    /*< 当为目录时，递归的进行copy */
                if(!targetDir.mkdir(fileInfo.fileName()))
                        return false;
                if(!pasteFolder(fileInfo.filePath(),
                    targetDir.filePath(fileInfo.fileName()),
                    coverFileIfExist))
                    return false;
            }
            else{            /*< 当允许覆盖操作时，将旧文件进行删除操作 */
                if(coverFileIfExist && targetDir.exists(fileInfo.fileName())){
                    targetDir.remove(fileInfo.fileName());
                }

                /// 进行文件copy
                if(!QFile::copy(fileInfo.filePath(),
                    targetDir.filePath(fileInfo.fileName()))){
                        return false;
                }
            }
        }
        return true;
}
//删除文件功能
bool MainWindow::deleteFile(const QString &folderDir)
{
    QDir dir(folderDir);
    QFileInfoList fileList;
    QFileInfo curFile;
    if(!dir.exists())  {return false;}//文件不存，则返回false
    fileList=dir.entryInfoList(QDir::Dirs|QDir::Files
                               |QDir::Readable|QDir::Writable
                               |QDir::Hidden|QDir::NoDotAndDotDot
                               ,QDir::Name);
    while(fileList.size()>0)
    {
        int infoNum=fileList.size();
        for(int i=infoNum-1;i>=0;i--)
        {
            curFile=fileList[i];
            if(curFile.isFile())//如果是文件，删除文件
            {
                QFile fileTemp(curFile.filePath());
                fileTemp.remove();
                fileList.removeAt(i);
            }
            if(curFile.isDir())//如果是文件夹
            {
                QDir dirTemp(curFile.filePath());
                QFileInfoList fileList1=dirTemp.entryInfoList(QDir::Dirs|QDir::Files
                                                              |QDir::Readable|QDir::Writable
                                                              |QDir::Hidden|QDir::NoDotAndDotDot
                                                              ,QDir::Name);
                if(fileList1.size()==0)//下层没有文件或文件夹
                {
                    dirTemp.rmdir(".");
                    fileList.removeAt(i);
                }
                else//下层有文件夹或文件
                {
                    for(int j=0;j<fileList1.size();j++)
                    {
                        if(!(fileList.contains(fileList1[j])))
                            fileList.append(fileList1[j]);
                    }
                }
            }
        }
    }
    return true;
}
//查找同名文件（新建txt.word.exel用到）
bool MainWindow::findSameFile(QString filename,QString folderDir)
{

    QDir dir(folderDir);
    QFileInfoList fileList;
    QFileInfo curFile;
    if(!dir.exists())
    {return false;}//文件不存，则返回false
    fileList=dir.entryInfoList(QDir::Dirs|QDir::Files
                               |QDir::Readable|QDir::Writable
                               |QDir::Hidden|QDir::NoDotAndDotDot
                               ,QDir::Name);
    int infoNum=fileList.size();
    for(int i=infoNum-1;i>=0;i--)
    {
        curFile = fileList[i];
        if(0 == filename.compare(curFile.fileName()))
        {
            return true;
        }
    }
    return false;
}
//鼠标右键菜单
void MainWindow::contextMenuEvent(QContextMenuEvent *)
{
    QMenu *menu = new QMenu(this);

    //添加右键菜单
    QMenu *createFilemenu = new QMenu(QStringLiteral("新建"),this);
    QAction *newFolderAction = new QAction(QStringLiteral("新建文件夹"),this);
    QAction *newTxtAction = new QAction(QStringLiteral("文本文档"),this);
    QAction *newWordAction = new QAction(QStringLiteral("Word文档"),this);
    QAction *newExcelAction = new QAction(QStringLiteral("Excel表格"),this);

    menu->addAction(createFilemenu->menuAction());
    createFilemenu->addAction(newFolderAction);
    createFilemenu->addSeparator();
    createFilemenu->addAction(newTxtAction);
    createFilemenu->addAction(newExcelAction);
    createFilemenu->addAction(newWordAction);


    QAction *fileopen = menu->addAction(QStringLiteral("打开"));
    QAction *refresh = menu->addAction(QStringLiteral("刷新"));
    QAction *filecopy = menu->addAction(QStringLiteral("复制"));
    QAction *filepaste = menu->addAction(QStringLiteral("粘贴"));
    QAction *filedelete = menu->addAction(QStringLiteral("删除"));
    QAction *filerename = menu->addAction(QStringLiteral("重命名"));
    QAction *filedetail = menu->addAction(QStringLiteral("详细信息"));

    menu->move(cursor().pos());
    menu->show();

    if(!coverFileIfExist)
         filepaste->setEnabled(false);
     else
         filepaste->setEnabled(true);

    if(isDelete){
        filedelete->setEnabled(true);
    }else{
        filedelete->setEnabled(false);
    }
    //连接右键菜单按钮与相应功能槽函数
    connect(filecopy, SIGNAL(triggered(bool)), this, SLOT(fileCopySlot()));
    connect(filepaste, SIGNAL(triggered(bool)), this, SLOT(filePasteSlot()));
    connect(refresh, SIGNAL(triggered(bool)), this, SLOT(refreshSlot()));
    connect(filedelete, SIGNAL(triggered(bool)), this, SLOT(fileDeleteSlot()));
    connect(newFolderAction,SIGNAL(triggered()),this,SLOT(newDFile()));
    connect(newTxtAction,SIGNAL(triggered()),this,SLOT(newTxtFile()));
    connect(newExcelAction,SIGNAL(triggered()),this,SLOT(newExelFile()));
    connect(newWordAction,SIGNAL(triggered()),this,SLOT(newWordFile()));
    connect(fileopen,SIGNAL(triggered()),this,SLOT(fileOpenSlot()));
    connect(filerename, SIGNAL(triggered(bool)), this, SLOT(fileRenameSlot()));
    connect(filedetail, SIGNAL(triggered(bool)), this, SLOT(fileDetailSlot()));
}
//刷新槽函数
void MainWindow::refreshSlot(){
    model->refresh();
}
void MainWindow::fileCopySlot()
{
    getSourcePath(indexChoose);
    coverFileIfExist = true;
}
//粘贴槽函数
void MainWindow::filePasteSlot()
{
    getDesPath(indexNow);
    QFileInfo temp(sourceDir);
    if(temp.isDir())
    {
        QDir creatDir(toDir);
        creatDir.mkdir(temp.fileName());
        pasteFolder(sourceDir,toDir,coverFileIfExist);
    }
    else
    {
        pasteFile(sourceDir,toDir,coverFileIfExist);
    }
    coverFileIfExist = false;
    model->refresh();
}
//删除槽函数
void MainWindow::fileDeleteSlot()
{
   QModelIndex a =ui->listView->currentIndex();
   QString deleteFilePath = model->fileInfo(a).absoluteFilePath();
   //qDebug()<<deleteFilePath;
    if (!a.isValid()) {
            return;
        }
        bool ok;
        QDir temp(model->fileInfo(a).absolutePath());
        if (model->fileInfo(a).isDir())
        {
            ok = deleteFile(deleteFilePath);
            temp.rmdir(model->fileInfo(a).fileName());
        }
        else
        {
            ok = temp.remove(model->fileInfo(a).fileName());
        }
        if (!ok)
        {
            QMessageBox::information(this,
                             tr("删除"),
                             tr("删除文件 %1 失败").arg(model->fileName(a)));
        }
        isDelete = false;
        model->refresh();
}
//打开槽函数
void MainWindow::fileOpenSlot()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));//用对应程序打开文件
    }
}
//重命名槽函数
void MainWindow::fileRenameSlot()
{
    QModelIndex a=ui->listView->currentIndex();
    model->setReadOnly(false);
    QFileInfo f=model->fileInfo(a);
        if (!a.isValid()) {
            return;
        }
        QString newName= QInputDialog::getText(this,
                                                tr("重命名文件"),
                                                tr("新文件名"));
        QString startext = f.suffix();
        QFileInfo fi(newName);
        QString endext=fi.suffix();
        if (newName.isEmpty())
        {
            QMessageBox::warning(this,tr("警告"),tr("新文件名为空!!!"));
            return;
        }
        else
        {
           if(startext!=endext)
           {
               if(endext==NULL)
           {
                   newName=newName+"."+startext;
               model->setData(a, newName, Qt::EditRole);
           }
           else{
             QMessageBox::StandardButton res= QMessageBox::warning(NULL,
                                                        "警告", "后缀名改变可能会导致文件不可用！",
                                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                        if(res==QMessageBox::No)
                          return;
                        if(res==QMessageBox::Yes)
                             //model->setData(index, value, Qt::EditRole);
           model->setData(a, newName, Qt::EditRole);
           }
           }
           else
                   model->setData(a,newName,Qt::EditRole);
        }
}
//详细信息槽函数
void MainWindow::fileDetailSlot()
{
    QDialog *finfo=new QDialog();
    QModelIndex fi=ui->listView->currentIndex();
    QFileInfo fin=model->fileInfo(fi);
    QLabel *name=new QLabel(tr("文件名 :"));
    QIcon ficon=model->fileIcon(fi);
    QLabel *path=new QLabel(tr("文件路径 :"));
    QLabel *size=new QLabel(tr("文件大小 :"));
    QLabel *creat=new QLabel(tr("创始时间 :"));
    QLabel *lasttime=new QLabel(tr("最近阅读 :"));
    QLabel *lastmodified=new QLabel(tr("最近修改 :"));

    QLabel *nameline =new QLabel(fin.fileName());
    QLabel *pathline = new QLabel(fin.filePath());
    QLabel *sizeline = new QLabel(QString::number(fin.size())+" B");
    QLabel *creatline = new QLabel(fin.created().toString("yyyy-MM-dd,hh:mm:ss"));
    QLabel *lTimeline = new QLabel(fin.lastRead().toString("yyyy-MM-dd,hh:mm:ss"));
    QLabel *lModifiedline = new QLabel(fin.lastModified().toString("yyyy-MM-dd,hh:mm:ss"));

    QVBoxLayout *fInfoName =new QVBoxLayout;
    fInfoName->addWidget(name);
    fInfoName->addWidget(path);
    fInfoName->addWidget(size);
   // fInfoName->addItem(spaceitem1);
    fInfoName->addWidget(creat);
    fInfoName->addWidget(lasttime);
    fInfoName->addWidget(lastmodified);

    QVBoxLayout *fInfoData =new QVBoxLayout;
    fInfoData->addWidget(nameline);
    fInfoData->addWidget(pathline);
    fInfoData->addWidget(sizeline);
   // fInfoData->addItem(spaceitem1);
    fInfoData->addWidget(creatline);
    fInfoData->addWidget(lTimeline);
    fInfoData->addWidget(lModifiedline);

    QHBoxLayout *datalayout =new QHBoxLayout;
    datalayout->addLayout(fInfoName);
    datalayout->addLayout(fInfoData);
    finfo->setLayout(datalayout);
    finfo->show();

}
//新建txt文件槽函数
void MainWindow::newTxtFile()
{
    QString path = model->filePath(indexNow)+"/";
    path.replace("/","\\");
    if(!findSameFile(tr("Note.txt"),path))
    {
        QFile file(path+tr("Note.txt"));
        file.open(QIODevice::WriteOnly);
        file.close();
        model->refresh();
    }
    else
    {
        int i = 1;
        QString num;
        while(1)
        {
            QString filename = tr("Note")+num.setNum(i)+".txt";
            if(!findSameFile(filename,path))
            {
                QFile file(path+filename);
                file.open(QIODevice::WriteOnly);
                file.close();
                model->refresh();
                return;
            }
            i++;
        }
    }
}
//新建word文件槽函数
void MainWindow::newWordFile()
{
    QString path = model->filePath(indexNow)+"/";
    path.replace("/","\\");
   if(!findSameFile(tr("Word.doc"),path))
   {
       QFile file(path+tr("Word.doc"));
       file.open(QIODevice::WriteOnly);
       file.close();
       model->refresh();
   }
   else
   {
       int i = 1;
       QString num;
       while(1)
       {
           QString filename = tr("Word")+num.setNum(i)+".doc";
           if(!findSameFile(filename,path))
           {
               QFile file(path+filename);
               file.open(QIODevice::WriteOnly);
               file.close();
               model->refresh();
               return;
           }
           i++;
       }
   }
}
//新建excel文件槽函数
void MainWindow::newExelFile()
{
    QString path = model->filePath(indexNow)+"/";
    path.replace("/","\\");
   if(!findSameFile(tr("Excel.xls"),path))
   {
       QFile file(path+tr("Excel.xls"));
       file.open(QIODevice::WriteOnly);
       file.close();
       model->refresh();
   }
   else
   {
       int i = 1;
       QString num;
       while(1)
       {
           QString filename = tr("Excel")+num.setNum(i)+".xls";
           if(!findSameFile(filename,path))
           {
               QFile file(path+filename);
               file.open(QIODevice::WriteOnly);
               file.close();
               model->refresh();
               return;
           }
           i++;
       }
   }
}
//新建文件夹槽函数
void MainWindow::newDFile()
{
    QModelIndex index;
    if(indexNow == ui->listView->model()->parent(indexChoose))
        index = ui->listView->model()->parent(indexChoose);
    else
        index = ui->listView->currentIndex();
        if (!index.isValid()) {
            return;
        }
        QString dirName = QInputDialog::getText(this,
                                                tr("新建文件夹"),
                                                tr("文件夹名"));
        QDir temp(model->filePath(index));

        //qDebug()<<model->filePath(index);
        if (!dirName.isEmpty()) {
            if (!temp.mkdir(dirName)) {
                QMessageBox::information(this,
                                         tr("新建文件夹"),
                                         tr("新建文件夹失败"));
            }
        }
        model->refresh();
}
//版本说明槽函数
void MainWindow::on_version_triggered()
{
    Dialog *newdialog =new Dialog;
    newdialog->show();
}
//搜索功能
void MainWindow::searchFileSlot()
{
    QString path = model->filePath(indexHistory.at(indexPoint));
    QString searchText = ui->searchLineEdit->text();
    ui->searchLineEdit->setText(QStringLiteral("正在搜索..."));
    SearchThread *searchThread1 = new SearchThread(path,searchText);
    connect(searchThread1,SIGNAL(sendData(QStringList)),this,SLOT(receQstring(QStringList)));
    searchThread1->start();
}

void MainWindow::receQstring(QStringList fileStringNew)
{
    if (!fileStringNew.isEmpty())
    {
    fileAndDirName = fileStringNew;
    ui->searchLineEdit->setText(QStringLiteral("搜索完毕，点击图标查看"));
    searchFinishAction = new QAction(QIcon(":/myimages/imgaes/showSearch.png"),QStringLiteral("搜索完毕,显示"),this);
    ui->searchLineEdit->addAction(searchFinishAction,QLineEdit::TrailingPosition);
    connect(searchFinishAction,SIGNAL(triggered(bool)),this,SLOT(solveSraech()));
    }
    else
        ui->searchLineEdit->setText(QStringLiteral("搜索无此文件，请检查"));
}

void MainWindow::solveSraech()
{
    searchFinishAction->setIcon(QIcon(":/myimages/imgaes/close.png"));
    searchFinishAction->setText(QStringLiteral("关闭搜索界面"));
    ui->searchLineEdit->setText(QStringLiteral("点击“×”关闭此页面"));
    QStringListModel *sModel = new QStringListModel;
    ui->listView->setModel(sModel);
    ui->listView->setViewMode(QListView::ListMode);
//    ui->listView->setStyleSheet("border-top-color:rgb(220,220,220); border-top-width:1px; border-top-style:solid; "
//                                "border-right-width:1px; border-right-color:rgb(248,248,248); border-right-style:solid;"
//                                "font-size:20px");
//    ui->listView->setIconSize(QSize(26,60));
//    ui->listView->setGridSize(QSize(26,60));
    sModel->setStringList(fileAndDirName);
    this->setContextMenuPolicy(Qt::NoContextMenu);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    disconnect(searchFinishAction,SIGNAL(triggered(bool)),this,SLOT(solveSraech()));
    connect(searchFinishAction,SIGNAL(triggered(bool)),this,SLOT(clickCloseSlot()));
    disconnect(ui->listView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickSlot(QModelIndex)));
    connect(ui->listView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickInQstringList(QModelIndex)));
}

void MainWindow::clickCloseSlot()
{
    //refreshView();
    //checkView();
    ui->listView->setModel(model);
    searchFinishAction->setVisible(false);
    ui->searchLineEdit->clear();
    ui->listView->setRootIndex(indexHistory.at(indexPoint));
    disconnect(ui->listView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickInQstringList(QModelIndex)));
    connect(ui->listView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickSlot(QModelIndex)));
}

void MainWindow::doubleClickInQstringList(QModelIndex a)
{
    clickCloseSlot();
    QString newPath = a.data().toString();
    int i;
    for (i=0; i<newPath.length(); i++)
    {
        if (newPath.at(i)=='\n')
            break;
    }
    newPath=newPath.remove(0,i+1);
    if (QFileInfo(newPath).isDir())
    ui->listView->setRootIndex(model->index(newPath));
    else if(QFileInfo(newPath).isFile())
    {
       QDesktopServices::openUrl(QUrl::fromLocalFile(newPath));
       ui->listView->setCurrentIndex(model->index(newPath));
    }
}

void MainWindow::openrask()
{
    Task *newTask =new Task;
    newTask->show();
}

