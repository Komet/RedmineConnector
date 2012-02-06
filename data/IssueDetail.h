#ifndef ISSUEDETAIL_H
#define ISSUEDETAIL_H

#include "data/User.h"

namespace RedmineConnector {

class User;

struct IssueDetail {
    User *user;
    QDateTime createdOn;
    QString notes;

    QList<QString> details;
};

}

#endif // ISSUEDETAIL_H
