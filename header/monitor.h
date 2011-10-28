#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <QList>
#include <QThread>
#include "header/task.h"


class Monitor : public QThread
{
	Q_OBJECT
public:
	Monitor(QList <Task *> *);
	void run();
	void checkTasks(void);

private:
	QList <Task *> *task_list;

signals:
	void signalSyncTaskAt(int);
};

#endif // SCHEDULER_H
