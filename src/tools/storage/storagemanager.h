#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <QPixmap>
#include <QString>

class QObject;
class ImgUploader;
class CaptureTool;

class StorageManager
{
public:
    explicit StorageManager();

    CaptureTool* imgUploaderTool(const QString& imgUploaderType,
                                 QObject* parent = nullptr);
    const QString& storageUrl(const QString& imgUploaderType);
    const QString& storageDefault();
    bool storageLocked();

private:
    // class members
    QString m_qstr;
};

#endif // STORAGEMANAGER_H
