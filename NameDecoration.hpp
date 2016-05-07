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
