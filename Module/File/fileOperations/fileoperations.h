#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <QObject>

class FileOperations : public QObject {
    Q_OBJECT
public:
    explicit FileOperations(QObject *parent = nullptr);

signals:

public slots:
    void createAFile(const QString &saveRout, const QString &fileNm);
    void fileRename(const QString &filePath, const QString &curFileNm, const QString &changedNm);
    void delFiles(const QString &fileDir);
};

#endif // FILEOPERATIONS_H
