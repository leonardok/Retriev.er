#include <QDateTime>
#include <QDebug>

#include "header/monitor.h"
#include "header/task.h"

Monitor::Monitor(QList <Task *> *task_list)
{
	this->task_list = task_list;
}

void Monitor::run(void)
{
	while(true)
	{
		this->sleep(30);
		this->checkTasks();
	}
}

void Monitor::checkTasks()
{
	int i = 0;
	QString curr_date_time = QDateTime::currentDateTime().toString("yyyyMMddhhmm");
	QString scheduled_time;

	for (i = 0; i < task_list->size(); i++)
	{
		if (task_list->at(i)->opt_enable_schedule == false)
		{
			qDebug() << "Schedule not enabled";
			continue;
		}
		scheduled_time = task_list->at(i)->opt_schedule.toString("yyyyMMddhhmm");
		if (scheduled_time.toInt() <= curr_date_time.toInt())
		{
			qDebug() << "Task must run now";
			switch (task_list->at(i)->opt_recurrence)
			{
				case Task::RECUR_HOUR:
					task_list->at(i)->setDateTime(task_list->at(i)->opt_schedule.addSecs(3600));
					break;
				case Task::RECUR_DAY:
					task_list->at(i)->setDateTime(task_list->at(i)->opt_schedule.addDays(1));
					break;
				case Task::RECUR_WEEK:
					task_list->at(i)->setDateTime(task_list->at(i)->opt_schedule.addDays(7));
					break;
				case Task::RECUR_MONTH:
					task_list->at(i)->setDateTime(task_list->at(i)->opt_schedule.addMonths(1));
					break;
				default:
					qCritical() << "Not known option";
					return;
			}

			emit signalSyncTaskAt(i);
		}
		else
		{
			qDebug() << "Nothing at " << curr_date_time << ". Task will run on " << scheduled_time;
		}
	}
}
