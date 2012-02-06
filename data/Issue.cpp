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
    QUrl issueUrl(QString("%1/issues/%2.xml?include=journals").arg(this->m_project->repository()->server()).arg(this->id()));
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

    QString msg = this->m_issueReply->readAll();
    this->parseIssue(msg);
    emit ready(this);
}

void Issue::parseIssue(QString xml)
{
    this->m_issueDetails.clear();

    QDomDocument domDoc;
    domDoc.setContent(xml);
    QDomNodeList nl = domDoc.elementsByTagName("journal");
    for( int i=0, iN=nl.count() ; i<iN ; i++ ) {
        QList<QString> details;
        QDomNodeList dnl = nl.at(i).toElement().elementsByTagName("details").at(0).toElement().elementsByTagName("detail");
        for( int n=0, nD=dnl.size() ; n<nD ; n++ ) {
            QString detail;
            if( dnl.at(n).toElement().attribute("property").compare("attr") == 0 ) {
                QString attrName = dnl.at(n).toElement().attribute("name");
                QString trAttrName;
                QString oldValue;
                QString newValue;
                if( attrName.compare("done_ratio") == 0 ) {
                    trAttrName = tr("% erledigt");
                    oldValue = dnl.at(n).toElement().elementsByTagName("old_value").at(0).toElement().text();
                    newValue = dnl.at(n).toElement().elementsByTagName("new_value").at(0).toElement().text();
                } else if( attrName.compare("start_date") == 0 ) {
                    trAttrName = tr("Beginn");
                    oldValue = QDate::fromString(dnl.at(n).toElement().elementsByTagName("old_value").at(0).toElement().text(), "yyyy-MM-dd").toString("dd.MM.yyyy");
                    newValue = QDate::fromString(dnl.at(n).toElement().elementsByTagName("new_value").at(0).toElement().text(), "yyyy-MM-dd").toString("dd.MM.yyyy");
                } else if( attrName.compare("due_date") == 0 ) {
                    trAttrName = tr("Abgabedatum");
                    oldValue = QDate::fromString(dnl.at(n).toElement().elementsByTagName("old_value").at(0).toElement().text(), "yyyy-MM-dd").toString("dd.MM.yyyy");
                    newValue = QDate::fromString(dnl.at(n).toElement().elementsByTagName("new_value").at(0).toElement().text(), "yyyy-MM-dd").toString("dd.MM.yyyy");
                } else if( attrName.compare("assigned_to_id") == 0 ) {
                    trAttrName = tr("Zugewiesen an");
                    oldValue = this->m_project->repository()->user(dnl.at(n).toElement().elementsByTagName("old_value").at(0).toElement().text().toInt())->fullName();
                    newValue = this->m_project->repository()->user(dnl.at(n).toElement().elementsByTagName("new_value").at(0).toElement().text().toInt())->fullName();
                } else if( attrName.compare("status_id") == 0 ) {
                    trAttrName = tr("Status");
                    oldValue = this->m_project->repository()->issueStatus(dnl.at(n).toElement().elementsByTagName("old_value").at(0).toElement().text().toInt())->name();
                    newValue = this->m_project->repository()->issueStatus(dnl.at(n).toElement().elementsByTagName("new_value").at(0).toElement().text().toInt())->name();
                } else if( attrName.compare("tracker_id") == 0 ) {
                    trAttrName = tr("Tracker");
                    oldValue = this->m_project->repository()->tracker(dnl.at(n).toElement().elementsByTagName("old_value").at(0).toElement().text().toInt()).name;
                    newValue = this->m_project->repository()->tracker(dnl.at(n).toElement().elementsByTagName("new_value").at(0).toElement().text().toInt()).name;
                } else if( attrName.compare("description") == 0 ) {
                    trAttrName = tr("Beschreibung");
                    oldValue = dnl.at(n).toElement().elementsByTagName("old_value").at(0).toElement().text();
                    newValue = dnl.at(n).toElement().elementsByTagName("new_value").at(0).toElement().text();
                } else if( attrName.compare("category_id") == 0 ) {
                    trAttrName = tr("Kategorie");
                    oldValue = this->m_project->issueCategoryFromId(dnl.at(n).toElement().elementsByTagName("old_value").at(0).toElement().text().toInt())->name();
                    newValue = this->m_project->issueCategoryFromId(dnl.at(n).toElement().elementsByTagName("new_value").at(0).toElement().text().toInt())->name();
                } else if( attrName.compare("priority_id") == 0 ) {
                    trAttrName = tr("Priorität");
                    oldValue = this->m_project->repository()->getAndAddPriority(dnl.at(n).toElement().elementsByTagName("old_value").at(0).toElement().text().toInt(), tr("unbekannte Priorität"))->name();
                    newValue = this->m_project->repository()->getAndAddPriority(dnl.at(n).toElement().elementsByTagName("new_value").at(0).toElement().text().toInt(), tr("unbekannte Priorität"))->name();
                } else if( attrName.compare("estimated_hours") == 0 ) {
                    trAttrName = tr("Geschätzer Aufwand");
                    oldValue = dnl.at(n).toElement().elementsByTagName("old_value").at(0).toElement().text();
                    newValue = dnl.at(n).toElement().elementsByTagName("new_value").at(0).toElement().text();
                } else {
                    trAttrName = tr("unbekanntes Attribut");
                    oldValue = dnl.at(n).toElement().elementsByTagName("old_value").at(0).toElement().text();
                    newValue = dnl.at(n).toElement().elementsByTagName("new_value").at(0).toElement().text();
                }

                if( !oldValue.isEmpty() && !newValue.isEmpty() ) {
                    detail = tr("<b>%1</b> wurde von %2 auf <b>%3</b> gesetzt").arg(trAttrName).arg(oldValue).arg(newValue);
                } else if( !oldValue.isEmpty() ) {
                    detail = tr("<b>%1</b> %2 wurde gelöscht").arg(trAttrName).arg(oldValue);
                } else {
                    detail = tr("<b>%1</b> wurde auf <b>%2</b> gesetzt").arg(trAttrName).arg(newValue);
                }
            } else if( dnl.at(n).toElement().attribute("property").compare("attachment") == 0 ) {
                QString filename = dnl.at(n).toElement().elementsByTagName("new_value").at(0).toElement().text();
                QString url = QString("%1/attachments/%2/%3")
                                .arg(this->project()->repository()->server())
                                .arg(dnl.at(n).toElement().attribute("name"))
                                .arg(filename);
                detail = tr("<b>Datei </b><a href=\"%1\">%2</a> wurde hinzugefügt").arg(url).arg(filename);
            }


            details.append(detail);
        }
        IssueDetail issueDetail;
        issueDetail.createdOn = QDateTime::fromString(nl.at(i).toElement().elementsByTagName("created_on").at(0).toElement().text().left(19), "yyyy-MM-ddTHH:mm:ss");
        issueDetail.notes = nl.at(i).toElement().elementsByTagName("notes").at(0).toElement().text();
        issueDetail.user = this->m_project->repository()->user(nl.at(i).toElement().elementsByTagName("user").at(0).toElement().attribute("id").toInt());
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
    stream.writeStartElement("issue");
    stream.writeTextElement("status_id", QString("%1").arg(status->id()));
    stream.writeTextElement("notes", notes);
    stream.writeTextElement("issue_status_id", QString("%1").arg(status->id()));
    // stream.writeTextElement("priority_id", QString("%1").arg(priority->id()));
    stream.writeTextElement("assigned_to_id", QString("%1").arg(assignedTo->id()));
    stream.writeTextElement("tracker_id", QString("%1").arg(tracker.id));
    QString categoryString = (category->id() == 0) ? "" : QString("%1").arg(category->id());
    stream.writeTextElement("category_id", QString("%1").arg(categoryString));
    stream.writeTextElement("start_date", startDate.toString("yyyy-MM-dd"));
    if( dueDate.isValid() ) {
        stream.writeTextElement("due_date", dueDate.toString("yyyy-MM-dd"));
    }
    stream.writeTextElement("done_ratio", QString("%1").arg(doneRatio));
    stream.writeEndElement(); // issue
    stream.writeEndDocument();

    QUrl issuePutUrl(QString("%1/issues/%2.xml").arg(this->m_project->repository()->server()).arg(this->id()));
    issuePutUrl.setUserName(this->m_project->repository()->username());
    issuePutUrl.setPassword(this->m_project->repository()->password());

    QNetworkRequest request(issuePutUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");

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
    this->m_startDate = QDate::fromString(startDate, "yyyy-MM-dd");
}

void Issue::setDueDate(QString dueDate)
{
    this->m_dueDate = QDate::fromString(dueDate, "yyyy-MM-dd");
}

void Issue::setDoneRatio(float doneRatio)
{
    this->m_doneRatio = doneRatio;
}

void Issue::setCreatedOn(QString createdOn)
{
    this->m_createdOn = QDateTime::fromString(createdOn.left(19), "yyyy-MM-ddTHH:mm:ss");
}

void Issue::setUpdatedOn(QString updatedOn)
{
    this->m_updatedOn = QDateTime::fromString(updatedOn.left(19), "yyyy-MM-ddTHH:mm:ss");
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
