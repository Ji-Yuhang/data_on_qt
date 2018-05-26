#include "newsmodel.h"
#include <QDebug>
NewsModel::NewsModel(QObject *parent)
    :QAbstractListModel(parent)
{

}

void NewsModel::addNews(const News &one_new)
{
    QList<News>::iterator it = list_.begin();
    QList<News>::iterator end = list_.end();

    QStringList ids;
    for(; it != end; it++){
        ids.append(it->id);
    }
    if (ids.contains(one_new.id)) {
        // 新闻已经存在，不加载
        return;
    } else {
        list_.append(one_new);
    }
    qDebug()<< "Model addNews"<< list_;
    emit layoutChanged();
}

bool NewsModel::refresh_image(const QUrl &url, const QImage &image)
{
    QList<News>::iterator it = list_.begin();
    QList<News>::iterator end = list_.end();

    for(; it != end; it++){
        if (url == QUrl(it->image_url)){
            it->image = image;
        }
    }
}

int NewsModel::rowCount(const QModelIndex &parent) const
{
    return list_.count();
}

QVariant NewsModel::data(const QModelIndex &index, int role) const
{


    if ( role == Qt::DisplayRole ) {
        int row = index.row();
        QVariant var0;
        var0.setValue(list_.at(row));
//        var0.setValue(list_.at(row).title);

        return var0;
    }
    return QVariant();
}

QDebug operator<<(QDebug debug, const News &n)
{
      QDebugStateSaver saver(debug);
      debug.nospace() << '[' << n.title << ", " << n.content << ']';

      return debug;
}
