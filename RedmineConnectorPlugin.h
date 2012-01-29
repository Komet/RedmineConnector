#ifndef REDMINECONNECTOR_H
#define REDMINECONNECTOR_H

#include "RedmineConnectorGlobal.h"
#include <extensionsystem/iplugin.h>

namespace RedmineConnector {
namespace Internal {

class RedmineConnectorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    
public:
    RedmineConnectorPlugin();
    ~RedmineConnectorPlugin();
    
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();
    static RedmineConnectorPlugin* instance();

private:
    static RedmineConnectorPlugin *_instance;
};

} // namespace Internal
} // namespace RedmineConnector

#endif // REDMINECONNECTOR_H

