#pragma once

#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QVector>
#include <memory>

namespace ProtocolCore {

struct PluginMetadata
{
    QString id;
    QString name;
    QString version;
    QString apiVersion;
};

enum class FieldType
{
    String,
    Integer,
    Double,
    Boolean,
    Enum
};

struct ConfigField
{
    QString name;
    FieldType type{};
    QVariant defaultValue;
    bool required{ false };
    QVariantMap constraints; // min/max/options/regex/etc.
};

using ConfigSchema = QVector<ConfigField>;

class AbstractProtocolInstance
{
public:
    virtual ~AbstractProtocolInstance() = default;

    virtual void connect() = 0;
    virtual QVariantMap poll() = 0;
    virtual void disconnect() = 0;
};

class AbstractProtocolPlugin
{
public:
    virtual ~AbstractProtocolPlugin() = default;

    virtual PluginMetadata metadata() const = 0;
    virtual ConfigSchema configSchema() const = 0;
    virtual void validate(const QVariantMap& config) const = 0;

    virtual std::unique_ptr<AbstractProtocolInstance> create(const QVariantMap& config) = 0;
};

} // namespace ProtocolCore
