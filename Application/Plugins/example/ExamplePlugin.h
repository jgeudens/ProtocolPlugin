#pragma once

#include "PluginInterface/PluginInterface.h"
#include "PluginInterface/QtPluginInterface.h"
#include <QObject>
#include <QVariantMap>

namespace ExamplePlugin {

class ExampleInstance : public PluginInterface::AbstractProtocolInstance
{
public:
    void connect() override
    {
    }
    QVariantMap poll() override
    {
        return QVariantMap{ { "value", 42 } };
    }
    void disconnect() override
    {
    }
};

class ExamplePluginImpl : public QObject, public PluginInterface::AbstractProtocolPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.yourcompany.ProtocolPlugin")
    Q_INTERFACES(PluginInterface::AbstractProtocolPlugin)

public:
    PluginInterface::PluginMetadata metadata() const override;
    PluginInterface::ConfigSchema configSchema() const override;
    void validate(const QVariantMap& config) const override;
    std::unique_ptr<PluginInterface::AbstractProtocolInstance> create(const QVariantMap& config) override;
};

} // namespace ExamplePlugin
