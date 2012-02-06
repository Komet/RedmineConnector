#ifndef REDMINECONNECTORCONSTANTS_H
#define REDMINECONNECTORCONSTANTS_H

namespace RedmineConnector {

namespace Constants {
    const char * const SETTINGS_CATEGORY = "RedmineConnector.Settings";
    const int NETWORK_TIMEOUT_VALUE = 180;
}

namespace RepositoryActions {
    const int GET_ALL_PROJECTS = 1;
    const int GET_PROJECTS     = 2;
    const int GET_ISSUES       = 3;
}

} // namespace RedmineConnector

#endif // REDMINECONNECTORCONSTANTS_H

