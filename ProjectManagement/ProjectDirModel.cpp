#include "ProjectDirModel.h"

ProjectDirModel::ProjectDirModel(QObject *parent, const QString& projectName) :
    QDirModel(parent), projectName(projectName)
{

}

QVariant ProjectDirModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result;

    if (orientation == Qt::Horizontal)
    {
        if (role != Qt::DisplayRole)
        {
            return QVariant();
        }

        if (role == Qt::BackgroundColorRole)
        {
          result = QColor(0,0,0);
        }
        else
        {
            result = QDirModel::headerData(section, orientation, role);
        }
    }

    if (section == 0)
    {
        return QString(projectName);
    }
    else return QString("");

    return result;
}
