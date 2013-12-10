#include <QDebug>
#include <QFile>

#include "file.h"
#include "util.h"

File::File(QString fullPathName, QObject *parent) : QObject(parent) {
    this->fullPathName = fullPathName;
    this->shortName = fullPathName.mid(fullPathName.lastIndexOf("/") + 1);
    this->nameWithExtension = this->shortName;
    this->shortName = this->shortName.left(shortName.indexOf("."));
    buildKeywordsLists();
    openFile();
}

void File::buildKeywordsLists() {
    this->keywords = shortName.split(QRegExp("\\s+"));
    QStringList newKeywords;

    for (int i = 0; i < keywords.size(); i++) {
        QString currentKeyword = keywords[i];
        QString correctedKeyword = Util::normalizeKeyword(currentKeyword);

        if (!correctedKeyword.isEmpty())
            newKeywords.append(correctedKeyword);
    }

    this->keywords = newKeywords;

    for (int i = 0; i < keywords.size(); i++) {
        QString keywordHash = Util::hashName(keywords[i]);
        qDebug() << keywords[i] << " " << keywordHash;
        this->keywordsID.push_back(keywordHash);
    }
}

void File::openFile() {
    QFile file(this->fullPathName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error in reading file";
        return;
    }

    this->fileSize = file.size();
    this->contents = file.readAll();
    this->fileID = Util::hashFileContents(this->contents);
    qDebug() << "file ID is: " << this->fileID;
}
