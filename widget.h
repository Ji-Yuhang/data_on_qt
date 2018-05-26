#ifndef WIDGET_H
#define WIDGET_H

#include <QListView>
#include <QTimer>
#include <QUrl>
#include <QWidget>
class NewsModel;
class NewsDelegate;
class QNetworkAccessManager;
class QNetworkReply;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void load_local_news();
    void async_load_image(const QUrl& url);
    void async_get_sina_news();
    void async_get_server_news();

public slots:
    void replyFinished(QNetworkReply*);
    void sinaNewsReplyFinished(QNetworkReply*);
    void newsReplyFinished(QNetworkReply*);

    void onIndexActivated(const QModelIndex &index);
    void onTimeout();
    void onRefreshDataTimeout();


private:
    void save_url_map();
private:
    Ui::Widget *ui;
    QListView * list_view_;
    NewsModel* news_model_;
    NewsDelegate* news_delegate_;

    QMap<QUrl, QString> url_map_;
    QNetworkAccessManager* manager_;
    QNetworkAccessManager* sina_news_manager_;
    QNetworkAccessManager* news_manager_;

    QTimer timer_;
    QTimer refresh_data_timer_;

//    QNetWork
};

#endif // WIDGET_H
