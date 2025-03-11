#include "filewatcher.h"

FileWatcher::FileWatcher(const QString &path, QObject *parent) : QObject(parent), m_path(path)
{
    m_fileInfo = new QFileInfo(m_path);
}

void FileWatcher::operate()
{
    auto lm{m_fileInfo->lastModified()};
    m_fileInfo->refresh();
    if (lm != m_fileInfo->lastModified())
    {
        const QString pathAndTime{QString("%1 - %2").arg(m_path).arg(m_fileInfo->lastModified().toString())};
        emit(onFileChanged(pathAndTime));
    }
}

void FileWatcher::fileChanged(const QString &path)
{
    auto lm{m_fileInfo->lastModified()};

    m_fileInfo->refresh();
    if (lm != m_fileInfo->lastModified())
    {
        const QString pathAndTime{QString("%1 - %2").arg(path).arg(m_fileInfo->lastModified().toString())};
        emit(onFileChanged(pathAndTime));
    }
}
