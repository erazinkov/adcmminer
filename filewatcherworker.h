#ifndef FILEWATCHERWORKER_H
#define FILEWATCHERWORKER_H

#include <QObject>
#include <QFileInfo>

class FileWatcherWorker : public QObject
{
    Q_OBJECT
public:
    explicit FileWatcherWorker(const QString &path, QObject *parent = nullptr);

public slots:
    void doWorkFC();
    void doWorkP(const QString &);

signals:
    void resultReadyFileCheck(const QString &, const QString &, const bool &);
private:
    QString m_path;
    QFileInfo *m_fileInfo;
};

#endif // FILEWATCHERWORKER_H
