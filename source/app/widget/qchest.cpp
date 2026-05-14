#include "qchest.hpp"

#include <QSignalMapper>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QDialog>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

#include <algorithm>
#include <set>
#include <vector>

static bool isDummyIdentifier(const std::string &identifier)
{
    std::string s = identifier;
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s.find("DUMMY") != std::string::npos;
}

static bool isIgnoredChestId(uint16_t id)
{
    return (id >= 1 && id <= 8) || (id >= 90 && id <= 144) || (id >= 1112 && id <= 1277);
}

static bool isIgnoredJewelItem(uint16_t id, const std::string &identifier)
{
    if (id <= 1300)
        return false;

    std::string s = identifier;
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s.find("JWL") != std::string::npos || s.find("JUWEL") != std::string::npos;
}

static std::vector<uint16_t> collectUnusedItemIds(MH3U_SE *mh3u)
{
    std::set<uint16_t> used;

    for (uint32_t i = 0; i < 3; i++)
    {
        for (uint32_t j = 0; j < 8; j++)
        {
            uint16_t id = mh3u->savedata->inventory[i][j].id;
            if (id != 0)
                used.insert(id);
        }
    }

    for (uint32_t i = 0; i < 4; i++)
    {
        for (uint32_t j = 0; j < 8; j++)
        {
            uint16_t id = mh3u->savedata->pouch[i][j].id;
            if (id != 0)
                used.insert(id);
        }
    }

    for (uint32_t i = 0; i < 10; i++)
    {
        for (uint32_t j = 0; j < 100; j++)
        {
            uint16_t id = mh3u->savedata->chest[i][j].id;
            if (id != 0)
                used.insert(id);
        }
    }

    std::vector<uint16_t> result;
    const dataset_t *items = MH3U_DS::items();
    if (items == NULL)
        return result;

    for (uint32_t i = 0; i < items->size(); i++)
    {
        const dataitem_t &entry = items->at(i);
        if (isDummyIdentifier(entry.identifier))
            continue;

        uint16_t id = (uint16_t) entry.count;
        if (id == 0 || isIgnoredChestId(id) || isIgnoredJewelItem(id, entry.identifier) || used.count(id) != 0)
            continue;

        result.push_back(id);
        used.insert(id);
    }

    return result;
}

static std::vector<uint16_t> collectUnusedItemIdsInRange(MH3U_SE *mh3u, uint16_t minId, uint16_t maxId)
{
    std::set<uint16_t> used;

    for (uint32_t i = 0; i < 3; i++)
    {
        for (uint32_t j = 0; j < 8; j++)
        {
            uint16_t id = mh3u->savedata->inventory[i][j].id;
            if (id != 0)
                used.insert(id);
        }
    }

    for (uint32_t i = 0; i < 4; i++)
    {
        for (uint32_t j = 0; j < 8; j++)
        {
            uint16_t id = mh3u->savedata->pouch[i][j].id;
            if (id != 0)
                used.insert(id);
        }
    }

    for (uint32_t i = 0; i < 10; i++)
    {
        for (uint32_t j = 0; j < 100; j++)
        {
            uint16_t id = mh3u->savedata->chest[i][j].id;
            if (id != 0)
                used.insert(id);
        }
    }

    std::vector<uint16_t> result;
    const dataset_t *items = MH3U_DS::items();
    if (items == NULL)
        return result;

    for (uint32_t i = 0; i < items->size(); i++)
    {
        const dataitem_t &entry = items->at(i);
        if (isDummyIdentifier(entry.identifier))
            continue;

        uint16_t id = (uint16_t) entry.count;
        if (id < minId || id > maxId || id == 0 || isIgnoredChestId(id) || isIgnoredJewelItem(id, entry.identifier) || used.count(id) != 0)
            continue;

        result.push_back(id);
        used.insert(id);
    }

    return result;
}

QChest::QChest(MH3U_SE *mh3u, QWidget *parent) : QWidget(parent)
{
    this->mh3u = mh3u;

    QSignalMapper *signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(buttonClicked(int)));

    QSignalMapper *fillMapper = new QSignalMapper(this);
    connect(fillMapper, SIGNAL(mapped(int)), this, SLOT(fillPanelX99(int)));

    QSignalMapper *rangeMapper = new QSignalMapper(this);
    connect(rangeMapper, SIGNAL(mapped(int)), this, SLOT(fillPanelRange(int)));

    QSignalMapper *clearMapper = new QSignalMapper(this);
    connect(clearMapper, SIGNAL(mapped(int)), this, SLOT(clearPanel(int)));

    QTabWidget *tabs = new QTabWidget(this);
    QWidget *tab;
    QGridLayout *tab_layout;
    QWidget *tab_container;
    QVBoxLayout *container_layout;
    for (uint32_t i = 0; i < 10; i++)
    {
        tab_container = new QWidget(this);
        container_layout = new QVBoxLayout(tab_container);
        tabs->addTab(tab_container, QString("Panel ") + QString::number(i+1));

        // Füge die "Fill x99" und "Fill Range" Buttons hinzu
        QWidget *buttonContainer = new QWidget(tab_container);
        QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
        buttonLayout->setContentsMargins(0, 0, 0, 0);
        buttonLayout->setSpacing(5);

        QPushButton *fillBtn = new QPushButton("Fill x99", buttonContainer);
        fillBtn->setFixedWidth(100);
        buttonLayout->addWidget(fillBtn);
        fillMapper->setMapping(fillBtn, i);
        connect(fillBtn, SIGNAL(clicked(bool)), fillMapper, SLOT(map()));

        QPushButton *rangeBtn = new QPushButton("Fill Range", buttonContainer);
        rangeBtn->setFixedWidth(100);
        buttonLayout->addWidget(rangeBtn);
        rangeMapper->setMapping(rangeBtn, i);
        connect(rangeBtn, SIGNAL(clicked(bool)), rangeMapper, SLOT(map()));
        QPushButton *clearBtn = new QPushButton("Clear Tab", buttonContainer);
        clearBtn->setMaximumWidth(80);
        buttonLayout->addWidget(clearBtn);
        clearMapper->setMapping(clearBtn, i);
        connect(clearBtn, SIGNAL(clicked(bool)), clearMapper, SLOT(map()));
        buttonLayout->addStretch(1);
        container_layout->addWidget(buttonContainer);

        tab = new QWidget(this);
        tab_layout = new QGridLayout(tab);
        tab->setLayout(tab_layout);
        container_layout->addWidget(tab);

        for (uint32_t j = 0; j < 100; j++)
        {
            m_buttons[i][j] = new QPushButton(QString::number(mh3u->savedata->chest[i][j].id), tab);
            m_buttons[i][j]->setFixedHeight(32);
            m_buttons[i][j]->setFixedWidth(32);

            tab_layout->addWidget(m_buttons[i][j], j / 10, j % 10);

            signalMapper->setMapping(m_buttons[i][j], i * 100 + j);
            connect(m_buttons[i][j], SIGNAL(clicked(bool)), signalMapper, SLOT(map()));
        }
    }

    QHBoxLayout *topLayout = new QHBoxLayout();
    QPushButton *fillAllBtn = new QPushButton("Fill All Tabs", this);
    fillAllBtn->setFixedWidth(120);
    topLayout->addWidget(fillAllBtn);
    connect(fillAllBtn, SIGNAL(clicked(bool)), this, SLOT(fillAllPanelsX99()));

    QPushButton *clearAllBtn = new QPushButton("Clear All Tabs", this);
    clearAllBtn->setFixedWidth(120);
    topLayout->addWidget(clearAllBtn);
    connect(clearAllBtn, SIGNAL(clicked(bool)), this, SLOT(clearAllPanels()));

    topLayout->addStretch(1);

    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addLayout(topLayout);
    main_layout->addWidget(tabs);
    this->setLayout(main_layout);
    this->setWindowTitle("Chest editor");
}

void QChest::fillAllPanelsX99()
{
    for (uint32_t i = 0; i < 10; i++)
        fillPanelX99(i);
}

void QChest::clearAllPanels()
{
    for (uint32_t i = 0; i < 10; i++)
        clearPanel(i);
}

QChest::~QChest()
{
    this->mh3u = NULL;
}

void QChest::fillPanelX99(int panelIndex)
{
    std::vector<uint16_t> unusedIds = collectUnusedItemIds(this->mh3u);
    size_t nextUnused = 0;

    for (uint32_t j = 0; j < 100; j++)
    {
        item_t &slot = this->mh3u->savedata->chest[panelIndex][j];

        if (slot.id != 0 && !isIgnoredChestId(slot.id))
        {
            slot.count = 99;
            continue;
        }

        // Treat ignored IDs as empty slots so they can be replaced
        if (slot.id != 0 && isIgnoredChestId(slot.id))
        {
            slot.id = 0;
            slot.count = 0;
            m_buttons[panelIndex][j]->setText(QString::number(0));
        }

        if (nextUnused >= unusedIds.size())
            continue;

        slot.id = unusedIds[nextUnused];
        slot.count = 99;
        m_buttons[panelIndex][j]->setText(QString::number(slot.id));
        nextUnused++;
    }
}

void QChest::fillPanelRange(int panelIndex)
{
    QDialog dialog(this);
    dialog.setWindowTitle("Select ID Range");

    QSpinBox *minSpin = new QSpinBox(&dialog);
    minSpin->setMinimum(1);
    minSpin->setMaximum(65535);
    minSpin->setValue(1);

    QSpinBox *maxSpin = new QSpinBox(&dialog);
    maxSpin->setMinimum(1);
    maxSpin->setMaximum(65535);
    maxSpin->setValue(100);

    QHBoxLayout *spinLayout = new QHBoxLayout();
    spinLayout->addWidget(new QLabel("Min ID:", &dialog));
    spinLayout->addWidget(minSpin);
    spinLayout->addWidget(new QLabel("Max ID:", &dialog));
    spinLayout->addWidget(maxSpin);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->addLayout(spinLayout);
    mainLayout->addWidget(buttonBox);

    if (dialog.exec() == QDialog::Accepted)
    {
        uint16_t minId = (uint16_t) minSpin->value();
        uint16_t maxId = (uint16_t) maxSpin->value();

        std::vector<uint16_t> unusedIds = collectUnusedItemIdsInRange(this->mh3u, minId, maxId);
        size_t nextUnused = 0;

        for (uint32_t j = 0; j < 100; j++)
        {
            item_t &slot = this->mh3u->savedata->chest[panelIndex][j];

if (slot.id != 0 && !isIgnoredChestId(slot.id))
        {
            slot.count = 99;
            continue;
        }

        if (slot.id != 0 && isIgnoredChestId(slot.id))
        {
            slot.id = 0;
            slot.count = 0;
            m_buttons[panelIndex][j]->setText(QString::number(0));
            }

            if (nextUnused >= unusedIds.size())
                continue;

            slot.id = unusedIds[nextUnused];
            slot.count = 99;
            m_buttons[panelIndex][j]->setText(QString::number(slot.id));
            nextUnused++;
        }
    }
}

void QChest::clearPanel(int panelIndex)
{
    for (uint32_t j = 0; j < 100; j++)
    {
        item_t &slot = this->mh3u->savedata->chest[panelIndex][j];
        slot.id = 0;
        slot.count = 0;
        m_buttons[panelIndex][j]->setText(QString::number(0));
    }
}

void QChest::buttonClicked(int id)
{
    // std::cout << "Button " << id << " clicked!" << std::endl;

    QItem *qitem = new QItem(&(this->mh3u->savedata->chest[id / 100][id % 100]), this);
    qitem->setModal(true);
    qitem->exec();

    m_buttons[id / 100][id % 100]->setText(QString::number(this->mh3u->savedata->chest[id / 100][id % 100].id));
}
