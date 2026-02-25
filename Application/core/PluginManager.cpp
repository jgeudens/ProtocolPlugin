#include "PluginManager.h"

#include <QDir>
#include <QFileInfo>
#include <QPluginLoader>

namespace ProtocolCore {

PluginManager::PluginManager(QObject* parent) : QObject(parent)
{
}

PluginManager::~PluginManager()
{
    // QPluginLoader instances are owned by the QObject instances we stored
    // Unload is automatic when the loader is destroyed; we only kept plugin
    // instances (QObjects) to keep them alive for the lifetime of the manager.
    for (QObject* o : loaderOwners_)
    {
        delete o;
    }
}

bool PluginManager::loadPluginsFromDir(const QString& dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists())
    {
        lastError_ = QString("Directory does not exist: %1").arg(dirPath);
        return false;
    }

    QStringList nameFilters;
#if defined(Q_OS_WIN)
    nameFilters << "*.dll";
#elif defined(Q_OS_MAC)
    nameFilters << "*.dylib" << "*.so";
#else
    nameFilters << "*.so";
#endif

    QFileInfoList files = dir.entryInfoList(nameFilters, QDir::Files);
    for (const QFileInfo& fi : files)
    {
        QString path = fi.absoluteFilePath();
        QPluginLoader loader(path);
        QObject* instance = loader.instance();
        if (!instance)
        {
            lastError_ = loader.errorString();
            continue;
        }

        auto plugin = qobject_cast<AbstractProtocolPlugin*>(instance);
        if (!plugin)
        {
            lastError_ = QString("Plugin at %1 does not implement AbstractProtocolPlugin").arg(path);
            loader.unload();
            continue;
        }

        // Keep the instance alive by transferring ownership to our vector
        loaderOwners_.append(instance);
        plugins_.append(plugin);
        // Note: QPluginLoader must stay alive to keep symbols loaded; by
        // storing the QObject returned by instance(), the loader's internal
        // data remains valid. We don't keep the loader object here.
    }

    return !plugins_.isEmpty();
}

QVector<AbstractProtocolPlugin*> PluginManager::plugins() const
{
    return plugins_;
}

QString PluginManager::lastError() const
{
    return lastError_;
}

} // namespace ProtocolCore
