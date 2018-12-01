#include "mydirmodel.h"


//重写headerData
QVariant MyDirModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Horizontal)
    {
        switch(section)
        {
        case 0:
            return "名称";
        case 1:
            return "大小";
        case 2:
            return "文件类型";
        case 3:
            return "修改时间";
        }
    }

     return QAbstractItemModel::headerData(section, orientation, role);
}
