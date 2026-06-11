#include "filewatcherworker.h"

FileWatcherWorker::FileWatcherWorker(const QString &path, QObject *parent)
    : QObject{parent}
{
    m_fileInfo = new QFileInfo(path);
}

void FileWatcherWorker::doWorkFC()
{
    QString r;
    if (m_fileInfo->isFile() && m_fileInfo->exists()) {
        r.append(QString("%1 | <font color='green'>%2</font> | %3").arg(m_fileInfo->absoluteFilePath()).arg(QChar(0x2713)).arg(m_fileInfo->lastModified().toString()));
    } else {
        r.append(QString("%1 | <font color='red'>%2</font>").arg(m_fileInfo->absoluteFilePath()).arg(QChar(0x2717)));
    }
    emit resultReadyFileCheck(r);
}

void FileWatcherWorker::doWorkP(const QString &newPath)
{
    m_fileInfo->setFile(newPath);
}
