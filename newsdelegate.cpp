#include "newsdelegate.h"

#include <QPainter>
#include <QSpinBox>
#include <QUrl>
#include "newsmodel.h"
NewsDelegate::NewsDelegate(QObject *parent)
    :QItemDelegate(parent)
{

}
QWidget *NewsDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
    QSpinBox *editor = new QSpinBox(parent);
    editor->setMinimum(0);
    editor->setMaximum(100);

    return editor;
}

QSize NewsDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
            return QSize(200, 60); // very dumb value
}

void NewsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    News n = qvariant_cast<News>(index.model()->data(index));
    QRect image_rect = option.rect;
    QRect title_rect = option.rect;

    image_rect.setWidth(60);
    title_rect.setLeft(60);
    title_rect.setHeight(30);
    QRect stitle_rect = option.rect;
//    stitle_rect.setTopLeft(QPoint(60,30));
    stitle_rect.setTop(title_rect.top() + 30);
    stitle_rect.setLeft(60);

//    QUrl url(n.image_url);
//    QImage image; //建立QImage类对象image
    painter->drawImage(image_rect, n.image);
    painter->drawText(title_rect, Qt::AlignCenter, n.title);
    painter->drawText(stitle_rect, Qt::AlignCenter, n.content);
//    qDebug() << title_rect << stitle_rect;
}
void NewsDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();

    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}
void NewsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}
void NewsDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
