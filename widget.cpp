#include "widget.h"
#include "ui_widget.h"

#include <QDateTime>
#include <QFileSystemModel>
#include <QLabel>
#include <QListWidgetItem>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QApplication>
#include <QSettings>
#include "newsmodel.h"
#include "newsdelegate.h"
#include "json.h"
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
    ,list_view_(new QListView)
    ,news_model_(new NewsModel)
    ,news_delegate_(new NewsDelegate)
    ,manager_(new QNetworkAccessManager(this))
    ,news_manager_(new QNetworkAccessManager(this))

{
    ui->setupUi(this);
//    ui->gridLayout->addWidget(new QLabel("Zhou"));

    ui->gridLayout->addWidget(list_view_);
//    ui->gridLayout->addWidget(new QLabel("Zhou"));
//    QFileSystemModel *model = new QFileSystemModel;
//    model->setRootPath(QDir::currentPath());
//    news_model_->setData()

    this->load_local_news();

    list_view_->setModel(news_model_);
    connect(list_view_, SIGNAL(activated(QModelIndex)), this, SLOT(onIndexActivated(QModelIndex)));
    connect(list_view_, SIGNAL(clicked(QModelIndex)), this, SLOT(onIndexActivated(QModelIndex)));

    QModelIndex index = news_model_->index(0);
    QImage* image = new QImage("http://n.sinaimg.cn/news/transform/250/w160h90/20180510/h-h2-haichqz4982576.jpg");
    QWidget* w = new QWidget;
    w->setLayout(new QGridLayout);
    QPixmap pic("http://n.sinaimg.cn/news/transform/250/w160h90/20180510/h-h2-haichqz4982576.jpg");
    QLabel* l = new QLabel;
    l->setPixmap(pic);
    w->layout()->addWidget(l);
    w->layout()->addWidget(new QLabel("Fuck"));
//    list_view_->setIndexWidget(index, w);
//    QListWidgetItem* delegate = new QListWidgetItem;
//    delegate->set
//    list_view_->setItemDelegateForRow(0,    delegate);
//    list_view_->setit
    list_view_->setItemDelegate(news_delegate_);
    connect(manager_, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    connect(news_manager_, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(newsReplyFinished(QNetworkReply*)));

//    connect(qApp, SIGNAL(lastWindowClosed()),
//            this, SLOT(dump)
    connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimeout()));

}

Widget::~Widget()
{
    save_url_map();
    delete ui;
}

void Widget::load_local_news()
{
    QSettings settings("url_map");
    QStringList keys = settings.allKeys();
    foreach(QString key, keys){
        QString value = settings.value(key).toString();
        url_map_.insert(QUrl(key), value);
    }

    QMap<QString, QVariant> temp_url_map = settings.value("url_map").toMap();
    QMap<QString, QVariant>::iterator it = temp_url_map.begin();
    for( ; it != temp_url_map.end(); it++){
        QString url = it.key();
        QString value = it.value().toString();
        url_map_.insert(QUrl(url), value);
    }

    async_get_sina_news();
    for(int i = 0; i < 5; ++i) {
        News one;
        one.image_url = "http://n.sinaimg.cn/news/1_img/vcg/d2808720/186/w1024h762/20180510/sgYh-haichqz3581131.jpg";
        one.title = "俄总统普京举父亲照片参加“不朽军团”游行";
        one.content = "当地时间2018年5月9日，俄罗斯莫斯科，俄罗斯总统普京和以色列总理内塔尼亚胡参加“不朽军团”游行，纪念战胜纳粹德国73周年。图为普京拿着父亲照片，参加游行。";
        one.source = "新浪图片";
        one.created_at = QDateTime::currentDateTime();
        async_load_image(QUrl(one.image_url));

        news_model_->addNews(one);
    }
    timer_.start(3000);
}

void Widget::async_load_image(const QUrl &url)
{
    if (url_map_.contains(url)) {
        QString path = url_map_.value(url);
        QImage image(path);
        news_model_->refresh_image(url, image);
        qDebug() << "load_image"<< url << "from local file";
//        news_model_
    } else {

        manager_->get(QNetworkRequest(url));
        qDebug() << "async_load_image"<< url << "from network";
    }
}

void Widget::async_get_sina_news()
{
    QUrl url("http://interface.sina.cn/wap_api/layout_col.d.json?&showcid=56261");
    news_manager_->get(QNetworkRequest(url));

}

void Widget::replyFinished(QNetworkReply *reply)
{
    QDir dir;
    dir.mkdir("images");
    dir.cd("images");
    QByteArray block = reply->readAll();
    QByteArray md5 = QCryptographicHash::hash(block, QCryptographicHash::Md5).toHex();

    QUrl url = reply->url();
    QString file_name = url.fileName();

    QFile file;
    file.setFileName(dir.filePath( md5+file_name));
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    file.write(block);
    QFileInfo info(file);
    QString path = info.absoluteFilePath();

    file.close();
    if (block.isEmpty()) {
        file.remove();
    }

    QImage image(path);
    news_model_->refresh_image(url, image);
    url_map_.insert(url, path);
    qDebug() << url << path << md5;

}

void Widget::newsReplyFinished(QNetworkReply * reply)
{
    QByteArray block = reply->readAll();
    QtJson::JsonObject obj = QtJson::parse(QString::fromUtf8(block)).toMap();
    QtJson::JsonObject result = obj["result"].toMap();
    QtJson::JsonObject data = result["data"].toMap();
    QtJson::JsonArray list = data["list"].toList();
    for (QtJson::JsonArray::iterator it = list.begin(); it != list.end(); ++it) {
        QtJson::JsonObject r_one = it->toMap();
        qDebug() <<r_one;
        News one;
        QtJson::JsonObject allPics = r_one["allPics"].toMap();
        QtJson::JsonArray pics = allPics["pics"].toList();
        if (!pics.isEmpty()){
            one.image_url = pics[0].toMap()["imgurl"].toString();
        }

//        one.image_url = "http://n.sinaimg.cn/news/1_img/vcg/d2808720/186/w1024h762/20180510/sgYh-haichqz3581131.jpg";
        one.title = r_one["title"].toString();
        one.content = r_one["stitle"].toString();
        one.source = r_one["_id"].toString();
        one.created_at = QDateTime::currentDateTime();
        async_load_image(QUrl(one.image_url));

        news_model_->addNews(one);
    }



}

void Widget::onIndexActivated(const QModelIndex &index)
{
//    qDebug()<< "onIndexActivated "<< index;
//    news_model_->data(index);
    News n = qvariant_cast<News>(index.model()->data(index));
    ui->title->setText(n.title);
    ui->detail->setText(n.content);
    ui->created_at->setText(n.created_at.toString("yyyy-MM-dd hh:mm:ss"));
    ui->source->setText(n.source);
}

void Widget::onTimeout()
{
    // 定时更新新闻数据
    QModelIndex index = list_view_->currentIndex();
    QModelIndex next_index = index.model()->sibling(index.row() + 1,0, index);
    if (!next_index.isValid()) {
//        next_index = list_view_->rootIndex();
        next_index = index.model()->sibling(0,0, index);
    }
    if (next_index.isValid()) {
        list_view_->scrollTo(next_index);
        list_view_->setCurrentIndex(next_index);
        this->onIndexActivated(next_index);
    }

//    news_model_->
}

void Widget::save_url_map()
{
    QSettings settings("url_map");
    QMap<QUrl, QString>::iterator it = url_map_.begin();
    for( ; it != url_map_.end(); it++){
        QString url = it.key().toString();
        QString value = it.value();
        settings.setValue(url, value);
    }


}
