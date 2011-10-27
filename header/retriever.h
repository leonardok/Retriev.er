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

#ifndef RETRIEVER_H
#define RETRIEVER_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QProcess>
#include <QSettings>

#include "header/task.h"
#include "header/options.h"

namespace Ui {
    class Retriever;
}

class Retriever : public QMainWindow
{
    Q_OBJECT

public:
	explicit Retriever(QWidget *parent = 0);
	~Retriever();

	void createTrayIcon(void);
	void createActions(void);
	void showTrayIcon(void);


	QAction *restoreAction;
	QAction *quitAction;
	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu;



public slots:
	Task * createNewTask(void);
	void editTask(void);
	void refreshTaskList(void);
	int  getFreeId(void);
	void showTask(QModelIndex);
	void syncAllTasks(void);
	void syncSelectedTask(void);
	void deleteTask(void);
	void readFromStdout(void);
	void readFromStderr(void);
	void checkProcTermination(int);

	void saveAll(void);
	void loadAll(void);



signals:
	void signalRefreshTaskList();


private:
    Ui::Retriever *ui;
    QList <Task *> *taskList;
    Options *options;
    QProcess *proc;
    QSettings *settings;

    void closeEvent(QCloseEvent *event);
    void saveConfiguration(void);
    void loadConfiguration(void);
};

#endif // RETRIEVER_H
