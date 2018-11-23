#include "downloadmanager.h"
#include <QDebug>

DownloadManager * DownloadManager::s_instance = nullptr;

DownloadManager * DownloadManager::getInstance()
{
    if (s_instance == nullptr)
        s_instance = new DownloadManager();
    return s_instance;
}

void DownloadManager::kill()
{
    if (s_instance != nullptr)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

DownloadManager::DownloadManager() : QObject()
{

}

DownloadManager::~DownloadManager()
{

}

void DownloadManager::download(int id)
{
    qDebug() << id;
}
