/************************************************************************
**
**  Copyright (C) 2019 KKevin B. Hendricks, Stratford Ontario, Canada
**  Copyright (C) 2012 John Schember <john@nachtimwald.com>
**  Copyright (C) 2012 Dave Heiland
**
**  This file is part of Sigil.
**
**  Sigil is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  Sigil is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Sigil.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#pragma once
#ifndef SELECTFOLDER_H
#define SELECTFOLDER_H

#include <QtWidgets/QDialog>

#include "ResourceObjects/Resource.h"
#include "BookManipulation/Book.h"

#include "ui_SelectFolder.h"

class SelectFolder: public QDialog
{
    Q_OBJECT

public:
    SelectFolder(QString group, QSharedPointer<Book> book, QWidget *parent = 0);

    void SetList();

    QString GetFolder();

private slots:
    void WriteSettings();

private:
    void SetSelectedText();

    void ReadSettings();
    void connectSignalsSlots();

    QString m_SelectedText;

    QString m_group;

    QSharedPointer<Book> m_Book;

    Ui::SelectFolder ui;
};

#endif // SELECTFOLDER_H
