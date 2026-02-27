#include "PluginInterface/PluginInterface.h"
#include "PluginInterface/QtPluginInterface.h"
#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>
#include <QTest>
#include <QVariantMap>

class ExamplePluginTest : public QObject
{
    Q_OBJECT

private slots:
    void testPluginLoadsAndRuns()
    {
        QDir dir(QCoreApplication::applicationDirPath());
#if defined(Q_OS_WIN)
        QString libName = "example_plugin.dll";
#elif defined(Q_OS_MAC)
        QString libName = "libexample_plugin.dylib";
#else
        QString libName = "libexample_plugin.so";
#endif

        // Try to locate the built plugin under a nearby Plugins/example directory
        QString path;
        QDir searchDir(QCoreApplication::applicationDirPath());
        for (int i = 0; i < 8; ++i)
        {
            QString candidate = QDir(searchDir.filePath("Plugins/example")).filePath(libName);
            if (QFile::exists(candidate))
            {
                path = candidate;
                break;
            }
            if (!searchDir.cdUp())
                break;
        }
        if (path.isEmpty())
            path = dir.filePath(libName);

        QPluginLoader loader(path);

        // Diagnostic output to help locate load failures
        qDebug() << "Trying plugin path:" << path;
        qDebug() << "Exists:" << QFile::exists(path);

        QObject* obj = loader.instance();
        QVERIFY2(obj,
                 qPrintable(QString("%1 (exists=%2)").arg(loader.errorString(), QFile::exists(path) ? "yes" : "no")));

        auto plugin = qobject_cast<PluginInterface::AbstractProtocolPlugin*>(obj);
        QVERIFY(plugin);

        auto meta = plugin->metadata();
        QVERIFY(!meta.id.isEmpty());

        auto schema = plugin->configSchema();
        QVERIFY(schema.size() >= 0);

        QVariantMap cfg;
        plugin->validate(cfg);

        auto inst = plugin->create(cfg);
        QVERIFY(inst);

        inst->connect();
        auto data = inst->poll();
        QVERIFY(data.size() >= 0);
        inst->disconnect();
    }
};

QTEST_MAIN(ExamplePluginTest)

#include "ExamplePluginTest.moc"
