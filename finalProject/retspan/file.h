#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QString>
#include <QStringList>

class File : public QObject
{
    Q_OBJECT
public:
    explicit File(QString fullPathName, QObject *parent = 0);
    QString fullPathName;
    QString shortName;
    QString nameWithExtension;

    QStringList keywords;
    QStringList keywordsID;

    QByteArray contents;
    QString fileID;
    int fileSize;

signals:

public slots:

private:
    void buildKeywordsLists();
    void openFile();
};

#endif // FILE_H
