#ifndef DAUNTLESS_ASS_FILEDONWLOADER_HPP
#define DAUNTLESS_ASS_FILEDONWLOADER_HPP

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <unordered_map>

class FileDownloader : public QObject
{
    Q_OBJECT
  public:
    void download(const QUrl &url, const QString &destination, const QString &backupDestination);
    void downloadBlocking(const QUrl &url, const QString &destination,
                          const QString &backupDestination);

  private slots:
    void fileDownloaded(QNetworkReply *pReply, const QString &destination);

  private:
    std::unordered_map<QString, bool> downloaded;
    QNetworkAccessManager manager;
};

#endif // !DAUNTLESS_ASS_FILEDONWLOADER_HPP