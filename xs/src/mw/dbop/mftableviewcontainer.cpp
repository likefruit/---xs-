#include "mftableviewcontainer.h"


MFTableViewContainer::MFTableViewContainer(QString tableName, QString md, QWidget *parent) :
    MFWorkArea(_workFilter = new MFWorkFilter(tableName), _table = new MFTableView(tableName), parent)
{
    _md = md;
    connect(_workFilter, SIGNAL(addFilter()), this, SLOT(addFilter()));

    _bottomLeftLayout->addWidget(_delete = new QPushButton("删除"));
    _bottomLeftLayout->addWidget(_insert = new QPushButton("增加"));
    _bottomLeftLayout->addWidget(_submit = new QPushButton("提交"));
    _bottomLeftLayout->addWidget(_revert = new QPushButton("撤销"));

    _bottomRightLayout->addWidget(_import = new QPushButton("导入"));
    _bottomRightLayout->addWidget(_export_ = new QPushButton("导出"));

    connect(_delete, SIGNAL(clicked()), this, SLOT(deleteRow()));
    connect(_insert, SIGNAL(clicked()), this, SLOT(insertRow()));
    connect(_submit, SIGNAL(clicked()), this, SLOT(submit()));
    connect(_revert, SIGNAL(clicked()), this, SLOT(revert()));
    connect(_import, SIGNAL(clicked()), this, SLOT(slotImport()));
    connect(_export_, SIGNAL(clicked()), this, SLOT(slotExport()));




    /************************************************
         * title        |      our log
         * filter area
         *
         *
         *
         * table area
         *
         *
         *
         *
         * ---------------------
         *  left layout        | right layout
         *************************************************/
}


//   QMap<int, QString> _fields;

void MFTableViewContainer::addFieldDict(QString humanName, QString machineName, int index, bool b)
{
    //    _fields.insert(index, humanName);
    this->_table->_model->setHeaderData(index, Qt::Horizontal, humanName);

    if(b)
        this->_workFilter->_filter->addField(humanName, machineName);
}

void MFTableViewContainer::OnCreate()
{
    addFilter();
    _table->resizeColumnsToContents();
}
void MFTableViewContainer::deleteRow()
{
    QItemSelectionModel* selections = _table->selectionModel();
    QModelIndexList selectList = selections->selectedIndexes();

    foreach(QModelIndex index, selectList)
    {
        _table->_model->removeRow(index.row());
    }

    //    _table->_model->submitAll();
}
void MFTableViewContainer::insertRow()
{
    _table->_model->insertRecord(-1, QSqlRecord());
    _table->scrollToBottom();
    _table->setCurrentIndex(_table->_model->index(_table->_model->rowCount()-1, 0));
}
void MFTableViewContainer::revert()
{
    _table->_model->revertAll();
    reset();
    _table->_model->select();
    _table->resizeColumnsToContents();
}
void MFTableViewContainer::submit()
{
    if(!MFHaveAuth(_md))
    {
        QMessageBox::warning(this, "提交失败", "你没有权限操作这个模块");
        return;
    }

    if(!this->_table->_model->database().transaction())
    {
        QMessageBox::warning(this, "回滚也失败", "事务不支持");
    }
    if(!this->_table->_model->submitAll())
    {
        if(!this->_table->_model->database().rollback())
        {
            QMessageBox::warning(this, "回滚失败", "回滚失败，请联系系统管理员");
        }
        QMessageBox::warning(this, "提交失败", "数据提交失败，请检查是否有重合数据导致，或联系系统管理员");
        return;
    }
    this->_table->_model->database().commit();

    reset();
    _table->resizeColumnsToContents();
}


void MFTableViewContainer::addFilter()
{
    if(this->_workFilter == NULL || this->_workFilter->_filter == NULL)
        return;
    MFFilter::filter* f;
    if(canAddFilter())
    {
        f = this->_workFilter->_filter->addFilter();
        newFilterAdded(f);
    }
}

void MFTableViewContainer::slotImport()
{
    QString filename = QFileDialog::getOpenFileName(this, "Please select one cvs file", QString(),
                                                    tr("CSV文件 (*.csv)"));
    if(filename.length() == 0)
        return;
    QStringList total;
    QFile file(filename);
    if(!file.open(QFile::ReadOnly))
    {
        return;
    }


    for(;;)
    {
        QByteArray buf = file.readLine();
        if(buf.length() == 0)
            break;
        total << MFConvertFromGB2312(buf.data()).trimmed();
    }

    file.close();

    foreach(QString line, total)
    {
        line = line.trimmed();
        QStringList data = line.split(",");
        if(data.size() != _table->_model->columnCount())
        {
            mylog << data.size() <<" " <<  _table->_model->columnCount();
            QMessageBox::warning(this, "警告", "数据导入失败，是不是点错了？或者检查一下文件格式");
            return;
        }

        if(!_table->_model->insertRecordByValuesAndCommit(data))
        {
            QMessageBox::warning(this, "警告", QString("有数据导入失败：%1").arg(data.join(",")));
        }
    }
    QMessageBox::information(this, "提示", "数据导入完毕");
    _table->_model->select();
}

void MFTableViewContainer::slotExport()
{
    QString filename = QFileDialog::getSaveFileName(this, "Please select one cvs file", QString(),
                                                    tr("CSV文件 (*.csv)"));
    if(filename.length() == 0)
        return;
    QStringList strList;
    QStringList result;
    for(int i=0; i<_table->_model->rowCount(); i++)
    {
        strList.clear();
        for (int j=0; j<_table->_model->columnCount(); j++)
        {
            strList << _table->_model->getData(i, j).toString();
        }
        result << strList.join(",");
    }

    QFile file(filename);
    file.open(QFile::WriteOnly|QFile::Truncate);

    foreach(QString line, result)
    {
        line += "\r\n";
        file.write(MFConvertToGB2312(line));
    }

    file.close();

    QMessageBox::information(this, "提示", "数据导出成功");
}
