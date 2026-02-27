#include "PluginManager.h"

#include <QDebug>
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
    // Clear any previous error for this invocation and track whether this
    // call loaded any plugins.
    lastError_.clear();
    bool loadedThisCall = false;

    QDir dir(dirPath);
    if (!dir.exists())
    {
        lastError_ = QString("Directory does not exist: %1").arg(dirPath);
        qWarning() << "PluginManager: directory does not exist:" << dirPath;
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
    qDebug() << "PluginManager: scanning" << dirPath << "found" << files.size() << "candidates";
    for (const QFileInfo& fi : files)
    {
        QString path = fi.absoluteFilePath();
        qDebug() << "PluginManager: attempting to load plugin:" << path;
        // Allocate the loader on the heap and parent it to this manager so
        // that the loader (and loaded plugin instance) remain valid while
        // the manager exists.
        QPluginLoader* loader = new QPluginLoader(path, this);
        QObject* instance = loader->instance();
        if (!instance)
        {
            lastError_ = loader->errorString();
            qWarning() << "PluginManager: failed to instantiate plugin at" << path << ":" << loader->errorString();
            // loader failed to load; delete it to avoid keeping a useless loader
            delete loader;
            continue;
        }

        auto plugin = qobject_cast<AbstractProtocolPlugin*>(instance);
        if (!plugin)
        {
            lastError_ = QString("Plugin at %1 does not implement AbstractProtocolPlugin").arg(path);
            qWarning() << "PluginManager: plugin at" << path << "does not implement AbstractProtocolPlugin";
            loader->unload();
            delete loader;
            continue;
        }

        // Keep the loader alive so the plugin instance remains valid.
        loaders_.append(loader);
        plugins_.append(plugin);
        loadedThisCall = true;
        try
        {
            auto meta = plugin->metadata();
            qDebug() << "PluginManager: loaded plugin" << meta.id << meta.name << "from" << path;
        }
        catch (...)
        {
            qDebug() << "PluginManager: loaded plugin (metadata unavailable) from" << path;
        }
    }

    // Return whether this invocation successfully loaded any plugins. Do not
    // clear existing loaders_/plugins_ so previously loaded plugins remain
    // available.
    if (!loadedThisCall)
        qWarning() << "PluginManager: no plugins loaded from" << dirPath << "(see lastError for details)";

    return loadedThisCall;
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
