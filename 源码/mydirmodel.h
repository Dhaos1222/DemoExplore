#ifndef MYDIRMODEL_H
#define MYDIRMODEL_H
#include <QDirModel>

class MyDirModel:public QDirModel
{
    Q_OBJECT
public:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};




#endif // MYDIRMODEL_H
