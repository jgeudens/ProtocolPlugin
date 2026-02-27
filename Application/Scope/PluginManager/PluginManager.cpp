#include "PluginManager.h"

#include <QDir>
#include <QFileInfo>
#include <QPluginLoader>

namespace PluginInterface {

PluginManager::PluginManager(QObject* parent) : QObject(parent)
{
}

PluginManager::~PluginManager()
{
    // Ensure each loader unloads its plugin. We store QPluginLoader* in
    // `loaders_` so the loaders (and therefore plugin instances) remain
    // valid for the lifetime of the manager. Call unload() here to
    // explicitly unload plugin libraries; Qt parenting will handle
    // deletion of the loader objects themselves.
    for (QPluginLoader* loader : loaders_)
    {
        if (loader)
        {
            loader->unload();
        }
        delete loader;
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
        // Allocate the loader on the heap and parent it to this manager so
        // that the loader (and loaded plugin instance) remain valid while
        // the manager exists.
        QPluginLoader* loader = new QPluginLoader(path, this);
        QObject* instance = loader->instance();
        if (!instance)
        {
            lastError_ = loader->errorString();
            // loader failed to load; delete it to avoid keeping a useless loader
            delete loader;
            continue;
        }

        auto plugin = qobject_cast<AbstractProtocolPlugin*>(instance);
        if (!plugin)
        {
            lastError_ = QString("Plugin at %1 does not implement AbstractProtocolPlugin").arg(path);
            loader->unload();
            delete loader;
            continue;
        }

        // Keep the loader alive so the plugin instance remains valid.
        loaders_.append(loader);
        plugins_.append(plugin);
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

} // namespace PluginInterface
