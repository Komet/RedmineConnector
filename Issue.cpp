#include "Issue.h"

namespace RedmineConnector {

Issue::Issue(Project *project) :
    QObject(project)
{
    this->_project = project;
}

Issue::~Issue()
{
}


/*** GETTER ***/

int Issue::id()
{
    return this->_id;
}

int Issue::priorityId()
{
    return this->_priorityId;
}

QString Issue::priority()
{
    return this->_priority;
}

QString Issue::subject()
{
    return this->_subject;
}

QString Issue::description()
{
    return this->_description;
}

QDate Issue::startDate()
{
    return this->_startDate;
}

QDate Issue::dueDate()
{
    return this->_dueDate;
}

float Issue::doneRatio()
{
    return this->_doneRatio;
}

QDateTime Issue::createdOn()
{
    return this->_createdOn;
}

QDateTime Issue::updatedOn()
{
    return this->_updatedOn;
}

User* Issue::author()
{
    return this->_author;
}

User* Issue::assignedTo()
{
    return this->_assignedTo;
}

IssueStatus* Issue::issueStatus()
{
    return this->_issueStatus;
}

/*** SETTER ***/

void Issue::setId(int id)
{
    this->_id = id;
}

void Issue::setPriorityId(int priorityId)
{
    this->_priorityId = priorityId;
}

void Issue::setPriority(QString priority)
{
    this->_priority = priority;
}

void Issue::setSubject(QString subject)
{
    this->_subject = subject;
}

void Issue::setDescription(QString description)
{
    this->_description = description;
}

void Issue::setStartDate(QString startDate)
{
    this->_startDate = QDate::fromString(startDate, "yyyy-MM-dd");
}

void Issue::setDueDate(QString dueDate)
{
    this->_dueDate = QDate::fromString(dueDate, "yyyy-MM-dd");
}

void Issue::setDoneRatio(float doneRatio)
{
    this->_doneRatio = doneRatio;
}

void Issue::setCreatedOn(QString createdOn)
{
    this->_createdOn = QDateTime::fromString(createdOn.left(19), "yyyy-MM-ddTHH:mm:ss");
}

void Issue::setUpdatedOn(QString updatedOn)
{
    this->_updatedOn = QDateTime::fromString(updatedOn.left(19), "yyyy-MM-ddTHH:mm:ss");
}

void Issue::setAuthor(User *author)
{
    this->_author = author;
}

void Issue::setAssignedTo(User *assignedTo)
{
    this->_assignedTo = assignedTo;
}

void Issue::setIssueStatus(IssueStatus *issueStatus)
{
    this->_issueStatus = issueStatus;
}

}
