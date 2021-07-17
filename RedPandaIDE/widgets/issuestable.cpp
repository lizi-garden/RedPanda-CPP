#include "issuestable.h"
#include <QHeaderView>


IssuesTable::IssuesTable(QWidget *parent):
    QTableView(parent)
{
    mModel = new IssuesModel(this);
    this->setModel(mModel);
    this->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);
    this->setColumnWidth(0,200);
    this->setColumnWidth(1,45);
    this->setColumnWidth(2,45);
}

IssuesModel *IssuesTable::issuesModel()
{
    return mModel;
}

void IssuesTable::setErrorColor(QColor color)
{
    mModel->setErrorColor(color);
}

void IssuesTable::setWarningColor(QColor color)
{
    mModel->setWarningColor(color);
}

IssuesModel::IssuesModel(QObject *parent):
    QAbstractTableModel(parent)
{

}

void IssuesModel::addIssue(PCompileIssue issue)
{
    beginInsertRows(QModelIndex(),mIssues.size(),mIssues.size());
    mIssues.push_back(issue);
    endInsertRows();
}

void IssuesModel::clearIssues()
{
    if (mIssues.size()>0) {
        beginRemoveRows(QModelIndex(),0,mIssues.size()-1);
        mIssues.clear();
        endRemoveRows();
    }
}

void IssuesModel::setErrorColor(QColor color)
{
    mErrorColor = color;
}

void IssuesModel::setWarningColor(QColor color)
{
    mWarningColor = color;
}

PCompileIssue IssuesModel::issue(int row)
{
    if (row<0 || row>=static_cast<int>(mIssues.size())) {
        return PCompileIssue();
    }

    return mIssues[row];
}

int IssuesModel::count()
{
    return mIssues.size();
}

void IssuesTable::addIssue(PCompileIssue issue)
{
    mModel->addIssue(issue);
}

PCompileIssue IssuesTable::issue(const QModelIndex &index)
{
    return issue(index.row());
}

PCompileIssue IssuesTable::issue(const int row)
{
    return mModel->issue(row);
}

int IssuesTable::count()
{
    return mModel->count();
}

void IssuesTable::clearIssues()
{
    mModel->clearIssues();
}

int IssuesModel::rowCount(const QModelIndex &) const
{
    return mIssues.size();
}

int IssuesModel::columnCount(const QModelIndex &) const
{
    return 4;
}

QVariant IssuesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row()<0 || index.row() >= static_cast<int>(mIssues.size()))
        return QVariant();
    PCompileIssue issue = mIssues[index.row()];
    if (!issue)
        return QVariant();
    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case 0:
            return issue->filename;
        case 1:
            if (issue->line>0)
                return issue->line;
            else
                return "";
        case 2:
            if (issue->column>0)
                return issue->column;
            else
                return "";
        case 3:
            return issue->description;
        default:
            return QVariant();
        }
    case Qt::ForegroundRole:
        switch(issue->type) {
        case CompileIssueType::Error:
            return mErrorColor;
        case CompileIssueType::Warning:
            return mWarningColor;
        default:
            return QVariant();
        }
    case Qt::FontRole: {
        QFont newFont=((IssuesTable *)parent())->font();
        switch(issue->type) {
        case CompileIssueType::Error:
        case CompileIssueType::Warning:
        {
            newFont.setBold(true);
            break;
        }
        default:
            newFont.setBold(issue->line == 0);
        }
        return newFont;
    }
    default:
        return QVariant();
    }
}

QVariant IssuesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role ==  Qt::DisplayRole) {
        switch(section) {
        case 0:
            return tr("Filename");
        case 1:
            return tr("Line");
        case 2:
            return tr("Col");
        case 3:
            return tr("Description");
        }
    }
    return QVariant();
}
