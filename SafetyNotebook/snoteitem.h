//
// Created by jone on 2020/11/9.
//

#ifndef SECNOTE_SNOTEITEM_H
#define SECNOTE_SNOTEITEM_H

#include <QtGui/QStandardItem>

QT_BEGIN_NAMESPACE
class QUuid;
QT_END_NAMESPACE

enum SNoteItemType {
    Partition = 0,
    Note = 1
};

class SNoteItem : public QStandardItem {
public:
    SNoteItem();

    SNoteItem(const QUuid &id, const QString &title, const QString &content, SNoteItemType type = Note);

    QVariant data(int role = Qt::UserRole + 1) const override;

    QUuid id() const;

    QString title() const;

    QString content() const;

    SNoteItemType sNoteItemType() const;

    void setSNoteItemType(SNoteItemType type);

    void setId(const QUuid &id);

    void setTitle(const QString &title);

    void setContent(const QString &content);

    SNoteItem *child(int row) const;

    static QByteArray toJson(const SNoteItem &item);

    static QJsonObject SNoteItemToJsonObject(const SNoteItem &item);

    static SNoteItem *fromJson(const QByteArray &byteArray);

    static SNoteItem *JsonObjectToSNoteItem(const QJsonObject &obj);

    static SNoteItem *makeRootItem();

private:
    QUuid* _id{};
    QString _title;
    QString _content;
    QList<SNoteItem *> _childrens;
    SNoteItemType _type;
};


#endif //SECNOTE_SNOTEITEM_H
