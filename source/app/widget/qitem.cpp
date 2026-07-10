#include "qitem.hpp"

#include "qutil.hpp"

#include <QHBoxLayout>

QItem::QItem(item_t *item, QWidget *parent) : QDialog(parent)
{
    this->item = item;

    m_id = new QComboBox(this);
    populateComboBox(m_id, MH3U_DS::items());

    m_count = makeSpinBox(this, 0x0000, 0xffff);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_id);
    layout->addWidget(m_count);
    this->setLayout(layout);
    this->setWindowTitle("Single item editor");

    this->load();
}

void QItem::load()
{
    m_id->setCurrentIndex(m_id->findData(item->id));
    m_count->setValue(item->count);
}

void QItem::save()
{
    if (!item)
        return;

    item->id = (uint16_t) m_id->currentData().toInt();
    item->count = m_count->value();
}

void QItem::closeEvent(QCloseEvent *event)
{
    save();
    QDialog::closeEvent(event);
}
