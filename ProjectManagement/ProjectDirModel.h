#ifndef PROJECTDIRMODEL_H
#define PROJECTDIRMODEL_H

#include <QDirModel>
#include <QVariant>
#include <QString>

class ProjectDirModel : public QDirModel
{
    Q_OBJECT

private:
    QString projectName;

public:
    explicit ProjectDirModel(QObject *parent, const QString& projectName);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:

public slots:

};

#endif // PROJECTDIRMODEL_H
