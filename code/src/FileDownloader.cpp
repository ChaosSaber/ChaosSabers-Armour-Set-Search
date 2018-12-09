#include "FileDownloader.hpp"
#include<chrono> 
#include <thread>
#include <QFile>
#include <fstream>
#include <iostream>

void FileDownloader::download(const QUrl &url, const QString &destination,
                              const QString &backupDestination)
{
    downloaded[destination] = false;

    // backup
    std::ifstream src(destination.toStdString(), std::ios::binary);
    if (src)
    {
        std::ofstream dst(backupDestination.toStdString(), std::ios::binary);
        dst << src.rdbuf();
    }

    QNetworkRequest request(url);
    connect(&manager, &QNetworkAccessManager::finished,
            [this, destination](QNetworkReply *reply) { fileDownloaded(reply, destination); });
    manager.get(request);
}

void FileDownloader::downloadBlocking(const QUrl &url, const QString &destination,
                                      const QString &backupDestination)
{
    download(url, destination, backupDestination);
    while (!downloaded[destination])
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
}

void FileDownloader::fileDownloaded(QNetworkReply *reply, const QString &destination)
{
    downloaded[destination] = true;
    QFile config(destination);

    if (!config.open(QIODevice::WriteOnly))
    {
        std::cout << "Couldn't open config file: " << destination.toStdString() << std::endl;
        return;
    }
    config.write(reply->readAll());
    reply->deleteLater();
}