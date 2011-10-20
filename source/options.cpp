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

#include <QDir>
#include <QFileDialog>
#include <QDebug>

#include "header/options.h"
#include "ui_options.h"

Options::Options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Options)
{
    ui->setupUi(this);

    connect(this->ui->button, SIGNAL(accepted()),
	    this, SLOT(save()));
    connect(this->ui->rsyncPathBrowse, SIGNAL(clicked()),
	    this, SLOT(getRsyncPath()));
}

Options::~Options()
{
    delete ui;
}

void Options::save(void)
{
	this->rsync_path = this->ui->rsyncPath->text();
	this->close();

	return;
}

void Options::setRsyncPath(QString path)
{
	this->ui->rsyncPath->setText(path);
	this->rsync_path = path;
}


void Options::getRsyncPath(void)
{
	this->ui->rsyncPath->setText(this->browse());
}


QString Options::browse(void)
{
	QString startingFolderPath;
#ifdef Q_OS_WIN32
	startingFolderPath = qgetenv("PROGRAMFILES");
#else
	startingFolderPath = QDir::homePath();
#endif
	QString directory = QDir::toNativeSeparators(
		QFileDialog::getExistingDirectory(this,
		tr("Find Files"), startingFolderPath)
	);
	qDebug() << directory;


	return directory;
}
