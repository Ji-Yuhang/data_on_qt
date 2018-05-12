#ifndef NEWSMODEL_H
#define NEWSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QDateTime>
#include <QDebug>
#include <QVariant>
#include <QUrl>
#include <QImage>
struct News {
    QString image_url;
    QString title;
    QString content;
    QDateTime created_at;
    QString source;
    QImage image;
};

Q_DECLARE_METATYPE(News)

class NewsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    NewsModel(QObject *parent = 0);
    void addNews(const News& one_new);
    bool refresh_image(const QUrl& url, const QImage& image);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;


private:
    QList<News> list_;
};
QDebug operator<<(QDebug debug, const News &n);


#endif // NEWSMODEL_H
