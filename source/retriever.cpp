/*
 * Copyright (c) 2011, Leonardo Korndorfer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
**/

#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QProcess>
#include <QSettings>
#include <QVariant>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>

#include "header/retriever.h"
#include "header/task.h"
#include "header/options.h"
#include "ui_retriever.h"


Retriever::Retriever(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Retriever)
{
    ui->setupUi(this);

    this->taskList = new QList <Task *>;
    this->options  = new Options();
    this->settings = new QSettings(QSettings::IniFormat,
				   QSettings::UserScope,
				   "leok.me",
				   "Retriever");

    this->proc = new QProcess(this);
    connect(this->proc, SIGNAL(finished(int)),
	    this, SLOT(checkProcTermination(int)));

    connect(this->ui->buttonNewTask, SIGNAL(clicked()),
	    this, SLOT(createNewTask()));
    connect(this->ui->buttonDeleteTask, SIGNAL(clicked()),
	    this, SLOT(deleteTask()));
    connect(this->ui->buttonEditTask, SIGNAL(clicked()),
	    this, SLOT(editTask()));
    connect(this->ui->menuFileSave, SIGNAL(triggered()),
	    this, SLOT(saveAll()));
    connect(this->ui->buttonSyncTask, SIGNAL(clicked()),
	    this, SLOT(syncAllTasks()));
    connect(this->ui->buttonSyncSelectedTask, SIGNAL(clicked()),
	    this, SLOT(syncSelectedTask()));

    connect(this->ui->menuOptionsShow, SIGNAL(triggered()),
	    this->options, SLOT(show()));

    connect(this->ui->menuFileExportSettings, SIGNAL(triggered()),
	    this, SLOT(exportConfiguration()));
    connect(this->ui->menuFileImportSettings, SIGNAL(triggered()),
	    this, SLOT(importConfiguration()));

    connect( proc, SIGNAL(readyReadStandardOutput()),
		this, SLOT(readFromStdout()));
    connect( proc, SIGNAL(readyReadStandardError()),
		this, SLOT(readFromStderr()));

    connect(this->ui->menuFileQuit, SIGNAL(triggered()),
	    qApp, SLOT(quit()));


    /* set application icon */
    QIcon icon(":/images/resources/golden-transparent_43x43x32");
    this->setWindowIcon(icon);

    this->loadAll();

    createActions();
    createTrayIcon();
    trayIcon->show();
}

Retriever::~Retriever()
{
    delete ui;
}

/**
 * @fn          void Retriever::createTrayIcon(void)
 * @brief       Creates try icon
 *
 * @details     Create the tray icon and associate the functions that it will
 *              do.
 */
void Retriever::createTrayIcon()
{
	trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(restoreAction);

	QIcon *icon = new QIcon(":/images/resources/golden-transparent_43x43x32");

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(*icon);
	trayIcon->setContextMenu(trayIconMenu);
	trayIcon->setToolTip("Retriever");
}

/**
 * @fn          void Retriever::createActions(void)
 * @brief       Creates try icon actions
 *
 * @details     Create the actions for the tray
 */
void Retriever::createActions()
{
	restoreAction = new QAction(tr("&Restore"), this);
	connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));
}

/**
 * @fn          void Retriever::closeEvent(QCloseEvent *event)
 * @brief       Handle close to tray
 */
void Retriever::closeEvent(QCloseEvent *event)
{
	if (trayIcon->isVisible()) {
		hide();
		event->ignore();
	}
	else
		event->accept();
}

int Retriever::getFreeId(void)
{
	int free_id = 0, i;
	for (i = 0; i < this->taskList->size(); i++)
	{
		if (this->taskList->at(i)->getId() == free_id)
		{
			free_id++;
			i = 0;
		}
	}

	if (--i == this->taskList->size() && this->taskList->size() != 0)
		free_id++;

	return free_id;
}

Task * Retriever::createNewTask(void)
{
	Task *t = new Task(this);
	t->setId(getFreeId());
	this->taskList->append(t);

	connect(t, SIGNAL(signalChangedTask()),
		this, SLOT(refreshTaskList()));

	t->show();
	return t;
}

void Retriever::editTask(void)
{
	int selected_row = this->ui->taskList->currentIndex().row();
	if (selected_row < 0)
	{
		qDebug() << "Nothing to edit";

		QMessageBox msgBox;
		msgBox.setText("Please selecte an entry.");
		msgBox.exec();
	}
	else
	{
		qDebug() << "Selected " << this->taskList->at(selected_row)->title;
		this->taskList->at(selected_row)->show();
	}
	return;
}

void Retriever::showTask(QModelIndex index)
{
	this->taskList->at(index.row())->show();
	return;
}

void Retriever::refreshTaskList(void)
{
	int i = 0;
	Task *t;

	this->ui->taskList->clear();

	for (i = 0; i < this->taskList->size(); i++)
	{
		t = this->taskList->at(i);

		QListWidgetItem *item = t->generateItem();
		this->ui->taskList->addItem(item);

		QIcon icon(":/icons/resources/TangoIcons/software-update-available.png");
		item->setIcon(icon);

		connect(this->ui->taskList, SIGNAL(doubleClicked(QModelIndex)),
			this, SLOT(showTask(QModelIndex)));
	}

	return;
}

/**
 * @fn          void Retriever::saveAll(void)
 * @brief       Saves all the configuration for the retriever
 *
 * @details     Saves.
 *
 * @todo        Is it usefull to create a file manipulation class?
 */
void Retriever::saveAll(void)
{
	this->saveConfiguration();
	return;
}

void Retriever::loadAll(void)
{
	this->loadConfiguration();
	return;
}

/**
 * @fn          void Retriever::saveConfiguration(void)
 * @brief       Saves all the configuration for the retriever
 *
 * @details     Saves the task list into a file. This file will be written as
 *              a csv file. This is better for edditing and reading, as we only
 *              need a split in the file and we're cool.
 *              I also like to be editing files outside the application. This
 *              method serves to separate the saving and file manipulation from
 *              the rest of the application too.
 *
 * @todo        Is it usefull to create a file manipulation class?
 */
void Retriever::saveConfiguration(void)
{
	int i;

	this->settings->setValue("options/rsync_path", options->rsync_path);
	this->settings->setValue("tasks/size", this->taskList->size());

	this->settings->beginWriteArray("tasks");
	for (i = 0; i < this->taskList->size(); i++)
	{
		this->settings->setArrayIndex(i);
		this->settings->setValue("title", this->taskList->at(i)->title);
		this->settings->setValue("id", this->taskList->at(i)->id);
		this->settings->setValue("from", this->taskList->at(i)->from);
		this->settings->setValue("to", this->taskList->at(i)->to);
		this->settings->setValue("opt_remote", this->taskList->at(i)->opt_remote);
		this->settings->setValue("opt_delete", this->taskList->at(i)->opt_delete);
		this->settings->setValue("opt_compress", this->taskList->at(i)->opt_compress);
		this->settings->setValue("opt_recursive", this->taskList->at(i)->opt_recursive);
		this->settings->setValue("opt_show_progress", this->taskList->at(i)->opt_show_progress);
		this->settings->setValue("opt_schedule",
					 this->taskList->at(i)->opt_schedule.toString("yyyy-M-d hh:mm:ss"));
	}
	this->settings->endArray();

	return;
}

void Retriever::loadConfiguration(void)
{
	options->setRsyncPath(this->settings->value("options/rsync_path").toString());

	int task_list_size = this->settings->beginReadArray("tasks");
	for (int i = 0; i < task_list_size; i++)
	{
		this->settings->setArrayIndex(i);
		Task *t = new Task(this);

		t->setTitle(this->settings->value("title").toString());
		t->setId(this->settings->value("id").toInt());
		t->setFrom(this->settings->value("from").toString());
		t->setTo(this->settings->value("to").toString());
		t->setUseRemote(this->settings->value("opt_remote").toBool());
		t->setRecursive(this->settings->value("opt_recursive").toBool());

		qDebug() << t->title;

		this->taskList->append(t);
	}
	this->settings->endArray();

	this->refreshTaskList();
}

void Retriever::readFromStdout(void)
{
	QString msg = proc->readAllStandardOutput();
	this->ui->console->append(msg);
}

void Retriever::readFromStderr(void)
{
	this->ui->console->append(proc->readAllStandardError());
}

void Retriever::checkProcTermination(int termination_code)
{
	if (proc->exitStatus() != QProcess::NormalExit)
	{
		this->ui->console->clear();
		this->ui->console->append("Command failed!!");
	}

	this->ui->console->append("Command done!");
	return;
}

void Retriever::syncAllTasks(void)
{
	Task *t;
	QString *commandLine = new QString();
	for (int i = 0; i < this->taskList->size(); i++)
	{
		t = this->taskList->at(i);

		commandLine->append("rsync ");
		commandLine->append(t->from.toAscii());
		commandLine->append(" ");
		commandLine->append(t->to.toAscii());

		this->ui->console->append(commandLine->toAscii());

		qDebug() << "Syncyng task with command line:\n";
		qDebug() << t->title;
	}
}

void Retriever::syncSelectedTask(void)
{
	Task *t;
	QStringList arguments;
	QString rsyncpath = "";

	QString *commandLine = new QString();

	this->ui->console->clear();

	int selected_row = this->ui->taskList->currentIndex().row();
	if (selected_row < 0)
	{
		qDebug() << "Nothing to sync";

		QMessageBox msgBox;
		msgBox.setText("Please selecte an entry.");
		msgBox.exec();

		return;
	}

	qDebug() << "Selected " << this->taskList->at(selected_row)->title;

	t = this->taskList->at(selected_row);

	arguments << "-v";

	if (t->opt_recursive == true)
		arguments << "--recursive";

	qDebug() << t->opt_remote << "reomte";
	if (t->opt_remote == true)
		arguments << "-e ssh";


	/* If its windows we will use the cygpath app */
#ifdef Q_OS_WIN32
	QString cygpath, directory_from, directory_to;
	QProcess *cygpath_proc;
	QStringList arg_list;

	cygpath = "\"";
	cygpath.append(this->options->rsync_path);
	cygpath.append("\\cygpath.exe\"");
	cygpath_proc = new QProcess(this);

	arg_list << t->from;
	cygpath_proc->start(cygpath, arg_list);
	cygpath_proc->waitForFinished();
	directory_from = cygpath_proc->readAll();
	directory_from.remove(QChar('\n'), Qt::CaseInsensitive);

	arg_list.clear();
	arg_list << t->to;
	cygpath_proc->start(cygpath, arg_list);
	cygpath_proc->waitForFinished();
	directory_to = cygpath_proc->readAll();
	directory_to.remove(QChar('\n'), Qt::CaseInsensitive);

	arguments << directory_from.toAscii();
	arguments << directory_to.toAscii();

	rsyncpath = "\"";
	rsyncpath.append(this->options->rsync_path);
	rsyncpath.append("\\rsync.exe\"");

#else
	arguments << t->from.toAscii();
	arguments << t->to.toAscii();

	/* for linux and OSX say its in path */
	rsyncpath = "rsync";
#endif

	qDebug() << "Syncyng task with command line:\n";
	qDebug() << arguments;

	this->ui->console->append(commandLine->toAscii());

	qDebug() << rsyncpath;

	proc->start(rsyncpath, arguments);

	return;
}

void Retriever::deleteTask(void)
{
	int selected_row = this->ui->taskList->currentIndex().row();
	if (selected_row < 0)
	{
		qDebug() << "Nothing to delete";

		QMessageBox msgBox;
		msgBox.setText("Please select an entry.");
		msgBox.exec();
	}
	else
	{
		qDebug() << "Selected " << this->taskList->at(selected_row)->title;
		this->taskList->removeAt(selected_row);
		this->refreshTaskList();
	}

	return;
}

void Retriever::exportConfiguration(void)
{
	qDebug() << this->settings->fileName();

	QString to_file = QDir::toNativeSeparators(
		QFileDialog::getSaveFileName(this,
		tr("Save as..."), QDir::homePath())
	);

	QFile::copy(this->settings->fileName(), to_file);

	return;
}

void Retriever::importConfiguration(void)
{
	qDebug() << this->settings->fileName();

	QString from_file = QDir::toNativeSeparators(
		QFileDialog::getOpenFileName(this,
		tr("Chose file..."), QDir::homePath())
	);

	qDebug() << from_file;

	if (from_file.size() > 5)
	{
		QFile::remove(this->settings->fileName());
		QFile::copy(from_file, this->settings->fileName());

		QMessageBox msgBox;
		msgBox.setText("Please restart the application for the changes take effect.");
		msgBox.exec();
	}

	return;
}













