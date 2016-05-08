// Files Renamer., a program to sort and rename picture files
// Copyright (C) 2016 Martial Demolins AKA Folco

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef NAMEDECORATION_HPP
#define NAMEDECORATION_HPP

#include <QObject>
#include <QString>
#include <QStringList>

class NameDecoration: public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(NameDecoration)

    public:
        NameDecoration();
        QString decoration();

    public slots:
        void setPath(const QString& path);
        void setFolderNum(int num);
        void setSeparator(const QString& separator);
        void setPrefix(const QString& prefix);

    signals:
        void maxFolderChanged(int max);
        void decorationChanged();

    private:
        QString m_path;
        QStringList m_folderlist;
        int m_foldernum;
        QString m_separator;
        QString m_prefix;
        QString m_decoration;

        void updateDecoration();
        void rebuildFolderList();
};

#endif // NAMEDECORATION_HPP
