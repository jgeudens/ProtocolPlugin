#pragma once

#include "PluginInterface/PluginInterface.h"
#include "PluginInterface/QtPluginInterface.h"
#include <QObject>
#include <QPluginLoader>
#include <QString>
#include <QVector>

namespace PluginInterface {

class PluginManager : public QObject
{
    Q_OBJECT

public:
    explicit PluginManager(QObject* parent = nullptr);
    ~PluginManager() override;

    // Load plugins from directory (shared libs). Returns true if any plugins loaded.
    bool loadPluginsFromDir(const QString& dirPath);

    QVector<AbstractProtocolPlugin*> plugins() const;
    QString lastError() const;

private:
    QVector<QPluginLoader*> loaders_; // keep QPluginLoader instances alive
    QVector<AbstractProtocolPlugin*> plugins_;
    QString lastError_;
};

} // namespace PluginInterface
