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

#include "NameDecoration.hpp"
#include <QChar>

NameDecoration::NameDecoration(): m_foldernum(0)
{
}

//
//  Setters
//

void NameDecoration::setPath(const QString& path)
{
    // Avoid to generate the folder list if the path didn't change
    if (m_path == path)
        return;

    m_path = path;
    rebuildFolderList();
}

void NameDecoration::setFolderNum(int num)
{
    m_foldernum = num;
    updateDecoration();
}

void NameDecoration::setSeparator(const QString& separator)
{
    m_separator = separator;
    updateDecoration();
}

void NameDecoration::setPrefix(const QString& prefix)
{
    m_prefix = prefix;
    updateDecoration();
}

//
//  Getter
//

QString NameDecoration::decoration()
{
    return m_decoration;
}

//
//  Decoration building
//

void NameDecoration::updateDecoration()
{
    m_decoration = m_prefix;
    for (int i = 0; i < m_foldernum; i++)
        m_decoration = m_folderlist.at(i) + m_separator + m_decoration;

    emit decorationChanged();
}

void NameDecoration::rebuildFolderList()
{
    // Strip the path by removing the drive under Windows, and the last slash if one exist
    // It supports only local paths
    QString tmpstr = m_path;

#ifdef _WIN32
    tmpstr = tmpstr.mid(2);
#endif
    if (tmpstr.at(tmpstr.size() - 1) == QChar('/'))
        tmpstr = tmpstr.mid(0, tmpstr.size() - 1);

    // Retrieve each folder and create the list.
    // The folders of a path /a/b/c are stored [c, b, a]
    m_folderlist.clear();
    while (!tmpstr.isEmpty()) {
        int i = tmpstr.lastIndexOf(QChar('/'));
        m_folderlist << tmpstr.mid(i + 1);
        tmpstr = tmpstr.left(i);
    }

    // Update the max folder number and the decoration string
    emit maxFolderChanged(m_folderlist.size());
    updateDecoration();
}
