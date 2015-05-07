#include "ProjectDirModel.h"

ProjectDirModel::ProjectDirModel(QObject *parent, const QString& projectName) :
    QDirModel(parent), projectName(projectName)
{
}

QVariant ProjectDirModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role != Qt::DisplayRole)
        {
            return QVariant();
        }
    }
    if (section == 0)
    {
        return QString("Current Project: "  + projectName);
    }
    else return QString("");
}
