#include "ExamplePlugin.h"
#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>
#include <iostream>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QDir dir(QCoreApplication::applicationDirPath());
#if defined(Q_OS_WIN)
    QString libName = "example_plugin.dll";
#elif defined(Q_OS_MAC)
    QString libName = "libexample_plugin.dylib";
#else
    QString libName = "libexample_plugin.so";
#endif

    QString path = dir.filePath(libName);
    QPluginLoader loader(path);
    QObject* obj = loader.instance();
    if (!obj)
    {
        std::cerr << "Failed to load plugin: " << loader.errorString().toStdString() << "\n";
        return 2;
    }

    auto plugin = qobject_cast<PluginInterface::AbstractProtocolPlugin*>(obj);
    if (!plugin)
    {
        std::cerr << "Plugin did not implement interface\n";
        return 3;
    }

    auto meta = plugin->metadata();
    std::cout << "Plugin id: " << meta.id.toStdString() << "\n";

    auto schema = plugin->configSchema();
    std::cout << "Schema fields: " << schema.size() << "\n";

    QVariantMap cfg;
    plugin->validate(cfg);

    auto inst = plugin->create(cfg);
    if (!inst)
    {
        std::cerr << "Plugin returned null instance\n";
        return 4;
    }
    inst->connect();
    auto data = inst->poll();
    std::cout << "Polled value count: " << data.size() << "\n";
    inst->disconnect();

    return 0;
}
