#include "ExamplePlugin.h"

namespace ExamplePlugin {

PluginInterface::PluginMetadata ExamplePluginImpl::metadata() const
{
    return PluginInterface::PluginMetadata{ "example.simple", "Example Simple Plugin", "0.1", "1.0" };
}

PluginInterface::ConfigSchema ExamplePluginImpl::configSchema() const
{
    PluginInterface::ConfigField f;
    f.name = "dummy";
    f.type = PluginInterface::FieldType::Integer;
    f.defaultValue = 0;
    f.required = false;

    return PluginInterface::ConfigSchema{ f };
}

void ExamplePluginImpl::validate(const QVariantMap& /*config*/) const
{
    // no-op for example
}

std::unique_ptr<PluginInterface::AbstractProtocolInstance> ExamplePluginImpl::create(const QVariantMap& /*config*/)
{
    return std::make_unique<ExampleInstance>();
}

} // namespace ExamplePlugin
