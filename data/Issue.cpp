#include "Issue.h"

#include <QDebug>
#include <QDomDocument>
#include <QMessageBox>
#include <QXmlStreamWriter>

namespace RedmineConnector {

Issue::Issue(Project *project) :
    QObject(project)
{
    this->m_project = project;
    this->m_issueCategory = 0;
    this->m_issueStatus = 0;
}

Issue::~Issue()
{
}

void Issue::load()
{
    QUrl issueUrl(QString::fromLatin1("%1/issues/%2.xml?include=journals").arg(this->m_project->repository()->server()).arg(this->id()));
    issueUrl.setUserName(this->m_project->repository()->username());
    issueUrl.setPassword(this->m_project->repository()->password());

    this->m_issueReply = this->m_project->qnam()->get(QNetworkRequest(issueUrl));
    connect(this->m_issueReply, SIGNAL(finished()), this, SLOT(issueReadyRead()));
}

void Issue::issueReadyRead()
{
    if( this->m_issueReply->error() != QNetworkReply::NoError ) {
        emit ready(this);
        return;
    }

    QString msg = QString::fromLatin1(this->m_issueReply->readAll().constData());
    this->parseIssue(msg);
    emit ready(this);
}

void Issue::parseIssue(QString xml)
{
    this->m_issueDetails.clear();

    QDomDocument domDoc;
    domDoc.setContent(xml);
    QDomNodeList nl = domDoc.elementsByTagName(QLatin1String("journal"));
    for( int i=0, iN=nl.count() ; i<iN ; i++ ) {
        QList<QString> details;
        QDomNodeList dnl = nl.at(i).toElement().elementsByTagName(QLatin1String("details")).at(0).toElement().elementsByTagName(QLatin1String("detail"));
        for( int n=0, nD=dnl.size() ; n<nD ; n++ ) {
            QString detail;
            if( dnl.at(n).toElement().attribute(QLatin1String("property")).compare(QLatin1String("attr")) == 0 ) {
                QString attrName = dnl.at(n).toElement().attribute(QLatin1String("name"));
                QString trAttrName;
                QString oldValue;
                QString newValue;
                if( attrName.compare(QLatin1String("done_ratio")) == 0 ) {
                    trAttrName = tr("% Done");
                    oldValue = dnl.at(n).toElement().elementsByTagName(QLatin1String("old_value")).at(0).toElement().text();
                    newValue = dnl.at(n).toElement().elementsByTagName(QLatin1String("new_value")).at(0).toElement().text();
                } else if( attrName.compare(QLatin1String("start_date")) == 0 ) {
                    trAttrName = tr("Start Date");
                    oldValue = QDate::fromString(dnl.at(n).toElement().elementsByTagName(QLatin1String("old_value")).at(0).toElement().text(), QLatin1String("yyyy-MM-dd")).toString(Qt::SystemLocaleShortDate);
                    newValue = QDate::fromString(dnl.at(n).toElement().elementsByTagName(QLatin1String("new_value")).at(0).toElement().text(), QLatin1String("yyyy-MM-dd")).toString(Qt::SystemLocaleShortDate);
                } else if( attrName.compare(QLatin1String("due_date")) == 0 ) {
                    trAttrName = tr("Due Date");
                    oldValue = QDate::fromString(dnl.at(n).toElement().elementsByTagName(QLatin1String("old_value")).at(0).toElement().text(), QLatin1String("yyyy-MM-dd")).toString(Qt::SystemLocaleShortDate);
                    newValue = QDate::fromString(dnl.at(n).toElement().elementsByTagName(QLatin1String("new_value")).at(0).toElement().text(), QLatin1String("yyyy-MM-dd")).toString(Qt::SystemLocaleShortDate);
                } else if( attrName.compare(QLatin1String("assigned_to_id")) == 0 ) {
                    trAttrName = tr("Assignee");
                    oldValue = this->project()->repository()->user(dnl.at(n).toElement().elementsByTagName(QLatin1String("old_value")).at(0).toElement().text().toInt())->fullName();
                    newValue = this->project()->repository()->user(dnl.at(n).toElement().elementsByTagName(QLatin1String("new_value")).at(0).toElement().text().toInt())->fullName();
                } else if( attrName.compare(QLatin1String("status_id")) == 0 ) {
                    trAttrName = tr("Status");
                    oldValue = this->m_project->repository()->issueStatus(dnl.at(n).toElement().elementsByTagName(QLatin1String("old_value")).at(0).toElement().text().toInt())->name();
                    newValue = this->m_project->repository()->issueStatus(dnl.at(n).toElement().elementsByTagName(QLatin1String("new_value")).at(0).toElement().text().toInt())->name();
                } else if( attrName.compare(QLatin1String("tracker_id")) == 0 ) {
                    trAttrName = tr("Tracker");
                    oldValue = this->project()->tracker(dnl.at(n).toElement().elementsByTagName(QLatin1String("old_value")).at(0).toElement().text().toInt()).name;
                    newValue = this->project()->tracker(dnl.at(n).toElement().elementsByTagName(QLatin1String("new_value")).at(0).toElement().text().toInt()).name;
                } else if( attrName.compare(QLatin1String("description")) == 0 ) {
                    trAttrName = tr("Description");
                    oldValue = dnl.at(n).toElement().elementsByTagName(QLatin1String("old_value")).at(0).toElement().text();
                    newValue = dnl.at(n).toElement().elementsByTagName(QLatin1String("new_value")).at(0).toElement().text();
                } else if( attrName.compare(QLatin1String("category_id")) == 0 ) {
                    trAttrName = tr("Category");
                    oldValue = this->project()->issueCategoryFromId(dnl.at(n).toElement().elementsByTagName(QLatin1String("old_value")).at(0).toElement().text().toInt())->name();
                    newValue = this->project()->issueCategoryFromId(dnl.at(n).toElement().elementsByTagName(QLatin1String("new_value")).at(0).toElement().text().toInt())->name();
                } else if( attrName.compare(QLatin1String("priority_id")) == 0 ) {
                    trAttrName = tr("Priority");
                    oldValue = this->project()->repository()->getAndAddPriority(dnl.at(n).toElement().elementsByTagName(QLatin1String("old_value")).at(0).toElement().text().toInt(), tr("Unkown Priority"))->name();
                    newValue = this->project()->repository()->getAndAddPriority(dnl.at(n).toElement().elementsByTagName(QLatin1String("new_value")).at(0).toElement().text().toInt(), tr("Unkown Priority"))->name();
                } else if( attrName.compare(QLatin1String("estimated_hours")) == 0 ) {
                    trAttrName = tr("Estimated Time");
                    oldValue = dnl.at(n).toElement().elementsByTagName(QLatin1String("old_value")).at(0).toElement().text();
                    newValue = dnl.at(n).toElement().elementsByTagName(QLatin1String("new_value")).at(0).toElement().text();
                } else {
                    trAttrName = tr("Unknown Attribute");
                    oldValue = dnl.at(n).toElement().elementsByTagName(QLatin1String("old_value")).at(0).toElement().text();
                    newValue = dnl.at(n).toElement().elementsByTagName(QLatin1String("new_value")).at(0).toElement().text();
                }

                if( !oldValue.isEmpty() && !newValue.isEmpty() ) {
                    detail = tr("<b>%1</b> changed from %2 to <b>%3</b>").arg(trAttrName).arg(oldValue).arg(newValue);
                } else if( !oldValue.isEmpty() ) {
                    detail = tr("<b>%1</b> %2 was deleted").arg(trAttrName).arg(oldValue);
                } else {
                    detail = tr("<b>%1</b> set to <b>%2</b>").arg(trAttrName).arg(newValue);
                }
            } else if( dnl.at(n).toElement().attribute(QLatin1String("property")).compare(QLatin1String("attachment")) == 0 ) {
                QString filename = dnl.at(n).toElement().elementsByTagName(QLatin1String("new_value")).at(0).toElement().text();
                QString url = QString::fromLatin1("%1/attachments/%2/%3")
                                .arg(this->project()->repository()->server())
                                .arg(dnl.at(n).toElement().attribute(QLatin1String("name")))
                                .arg(filename);
                detail = tr("<b>File</b> <a href=\"%1\">%2</a> added").arg(url).arg(filename);
            }


            details.append(detail);
        }
        IssueDetail issueDetail;
        issueDetail.createdOn = QDateTime::fromString(nl.at(i).toElement().elementsByTagName(QLatin1String("created_on")).at(0).toElement().text().left(19), QLatin1String("yyyy-MM-ddTHH:mm:ss"));
        issueDetail.notes = nl.at(i).toElement().elementsByTagName(QLatin1String("notes")).at(0).toElement().text();
        issueDetail.user = this->m_project->repository()->user(nl.at(i).toElement().elementsByTagName(QLatin1String("user")).at(0).toElement().attribute(QLatin1String("id")).toInt());
        issueDetail.details = details;
        this->m_issueDetails.append(issueDetail);
    }
}

void Issue::setChangeset(IssueStatus *status, Priority *priority, User *assignedTo, Tracker tracker, IssueCategory *category, QDate startDate, QDate dueDate, int doneRatio, QString notes)
{
    Q_UNUSED(priority);

    this->m_assignedTo = assignedTo;
    this->m_doneRatio = (float)doneRatio;
    this->m_dueDate = dueDate;
    this->m_startDate = startDate;
    // this->m_priority = priority;
    this->m_issueStatus = status;
    this->m_tracker = tracker;
    this->m_issueCategory = category;

    QString xml;
    QXmlStreamWriter stream(&xml);
    stream.writeStartDocument();
    stream.writeStartElement(QLatin1String("issue"));
    stream.writeTextElement(QLatin1String("status_id"), QString::fromLatin1("%1").arg(status->id()));
    stream.writeTextElement(QLatin1String("notes"), notes);
    stream.writeTextElement(QLatin1String("issue_status_id"), QString::fromLatin1("%1").arg(status->id()));
    // stream.writeTextElement("priority_id", QString("%1").arg(priority->id()));
    stream.writeTextElement(QLatin1String("assigned_to_id"), QString::fromLatin1("%1").arg(assignedTo->id()));
    stream.writeTextElement(QLatin1String("tracker_id"), QString::fromLatin1("%1").arg(tracker.id));
    QString categoryString = (category->id() == 0) ? QString() : QString::fromLatin1("%1").arg(category->id());
    stream.writeTextElement(QLatin1String("category_id"), QString::fromLatin1("%1").arg(categoryString));
    stream.writeTextElement(QLatin1String("start_date"), startDate.toString(QLatin1String("yyyy-MM-dd")));
    if( dueDate.isValid() ) {
        stream.writeTextElement(QLatin1String("due_date"), dueDate.toString(QLatin1String("yyyy-MM-dd")));
    }
    stream.writeTextElement(QLatin1String("done_ratio"), QString::fromLatin1("%1").arg(doneRatio));
    stream.writeEndElement(); // issue
    stream.writeEndDocument();

    QUrl issuePutUrl(QString::fromLatin1("%1/issues/%2.xml").arg(this->m_project->repository()->server()).arg(this->id()));
    issuePutUrl.setUserName(this->m_project->repository()->username());
    issuePutUrl.setPassword(this->m_project->repository()->password());

    QNetworkRequest request(issuePutUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml"));

    this->m_issuePutReply = this->m_project->qnam()->put(request, xml.toUtf8());
    connect(this->m_issuePutReply, SIGNAL(finished()), this, SLOT(issuePutReadyRead()));
}

void Issue::issuePutReadyRead()
{
    if( this->m_issuePutReply->error() != QNetworkReply::NoError ) {
        QMessageBox::warning(0, tr("Redmine Connector"), this->m_issuePutReply->errorString(), QMessageBox::Ok);
    }
    emit ready(this);
}

/*** GETTER ***/

Project* Issue::project()
{
    return this->m_project;
}

int Issue::id()
{
    return this->m_id;
}

int Issue::priorityId()
{
    return this->m_priorityId;
}

QString Issue::priorityName()
{
    return this->m_priorityName;
}

Priority* Issue::priority()
{
    return this->m_priority;
}

QString Issue::subject()
{
    return this->m_subject;
}

QString Issue::description()
{
    return this->m_description;
}

QDate Issue::startDate()
{
    return this->m_startDate;
}

QDate Issue::dueDate()
{
    return this->m_dueDate;
}

float Issue::doneRatio()
{
    return this->m_doneRatio;
}

QDateTime Issue::createdOn()
{
    return this->m_createdOn;
}

QDateTime Issue::updatedOn()
{
    return this->m_updatedOn;
}

User* Issue::author()
{
    return this->m_author;
}

User* Issue::assignedTo()
{
    return this->m_assignedTo;
}

IssueStatus* Issue::issueStatus()
{
    if( this->m_issueStatus == 0 ) {
        this->m_issueStatus = new IssueStatus(this->m_project->repository());
    }
    return this->m_issueStatus;
}

IssueCategory* Issue::issueCategory()
{
    if( this->m_issueCategory == 0 ) {
        this->m_issueCategory = new IssueCategory(this->m_project);
    }
    return this->m_issueCategory;
}

Tracker Issue::tracker()
{
    return this->m_tracker;
}

QList<IssueDetail> Issue::issueDetails()
{
    return this->m_issueDetails;
}

/*** SETTER ***/

void Issue::setId(int id)
{
    this->m_id = id;
}

void Issue::setPriorityId(int priorityId)
{
    this->m_priorityId = priorityId;
}

void Issue::setPriorityName(QString name)
{
    this->m_priorityName = name;
}

void Issue::setPriority(Priority *priority)
{
    this->m_priority = priority;
}

void Issue::setSubject(QString subject)
{
    this->m_subject = subject;
}

void Issue::setDescription(QString description)
{
    this->m_description = description;
}

void Issue::setStartDate(QString startDate)
{
    this->m_startDate = QDate::fromString(startDate, QLatin1String("yyyy-MM-dd"));
}

void Issue::setDueDate(QString dueDate)
{
    this->m_dueDate = QDate::fromString(dueDate, QLatin1String("yyyy-MM-dd"));
}

void Issue::setDoneRatio(float doneRatio)
{
    this->m_doneRatio = doneRatio;
}

void Issue::setCreatedOn(QString createdOn)
{
    this->m_createdOn = QDateTime::fromString(createdOn.left(19), QLatin1String("yyyy-MM-ddTHH:mm:ss"));
}

void Issue::setUpdatedOn(QString updatedOn)
{
    this->m_updatedOn = QDateTime::fromString(updatedOn.left(19), QLatin1String("yyyy-MM-ddTHH:mm:ss"));
}

void Issue::setAuthor(User *author)
{
    this->m_author = author;
}

void Issue::setAssignedTo(User *assignedTo)
{
    this->m_assignedTo = assignedTo;
}

void Issue::setIssueStatus(IssueStatus *issueStatus)
{
    this->m_issueStatus = issueStatus;
}

void Issue::setIssueCategory(IssueCategory *issueCategory)
{
    this->m_issueCategory = issueCategory;
}

void Issue::setTracker(Tracker tracker)
{
    this->m_tracker = tracker;
}

}
