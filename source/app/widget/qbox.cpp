#include "qbox.hpp"

#include <QSignalMapper>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTabWidget>

#include <cstring>
#include <set>
#include <vector>

static bool isDummyIdentifier(const std::string &identifier)
{
    std::string s = identifier;
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s.find("DUMMY") != std::string::npos;
}

static std::vector<std::pair<uint8_t, uint16_t>> collectUnusedEquipment(MH3U_SE *mh3u)
{
    std::set<std::pair<uint8_t, uint16_t>> used;
    for (uint32_t i = 0; i < 10; i++)
    {
        for (uint32_t j = 0; j < 100; j++)
        {
            uint8_t type = mh3u->savedata->box[i][j][0];
            uint16_t id = mh3u->savedata->box[i][j][2] + (mh3u->savedata->box[i][j][3] << 8);
            if (type != 0 && id != 0)
                used.insert(std::make_pair(type, id));
        }
    }

    std::vector<std::pair<uint8_t, uint16_t>> result;
    const dataset_t *chestArmors = MH3U_DS::chestArmors();
    if (chestArmors == NULL)
        return result;

    for (uint32_t i = 0; i < chestArmors->size(); i++)
    {
        const dataitem_t &entry = chestArmors->at(i);
        if (isDummyIdentifier(entry.identifier))
            continue;

        uint16_t id = (uint16_t) entry.count;
        std::pair<uint8_t, uint16_t> item((uint8_t) MH3U_Type::ChestType, id);
        if (id != 0 && used.count(item) == 0)
        {
            result.push_back(item);
            used.insert(item);
        }
    }

    return result;
}

QBox::QBox(MH3U_SE *mh3u, QWidget *parent) : QWidget(parent)
{
    this->mh3u = mh3u;

    QSignalMapper *signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(buttonClicked(int)));

    QSignalMapper *fillMapper = new QSignalMapper(this);
    connect(fillMapper, SIGNAL(mapped(int)), this, SLOT(fillPanelX99(int)));

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

        // Füge den "Fill x99" und "Clear Tab" Buttons hinzu
        QWidget *buttonContainer = new QWidget(tab_container);
        QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
        buttonLayout->setContentsMargins(0, 0, 0, 0);
        buttonLayout->setSpacing(5);

        QPushButton *fillBtn = new QPushButton("Fill x99", buttonContainer);
        fillBtn->setFixedWidth(100);
        buttonLayout->addWidget(fillBtn);
        fillMapper->setMapping(fillBtn, i);
        connect(fillBtn, SIGNAL(clicked(bool)), fillMapper, SLOT(map()));

        QPushButton *clearBtn = new QPushButton("Clear Tab", buttonContainer);
        clearBtn->setFixedWidth(100);
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
            m_buttons[i][j] = new QPushButton(QString::number(mh3u->savedata->box[i][j][2] + mh3u->savedata->box[i][j][3] * 0x100), tab);
            m_buttons[i][j]->setFixedHeight(48);
            m_buttons[i][j]->setFixedWidth(48);

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
    this->setWindowTitle("Box editor");
}

void QBox::fillAllPanelsX99()
{
    for (uint32_t i = 0; i < 10; i++)
        fillPanelX99(i);
}

void QBox::clearAllPanels()
{
    for (uint32_t i = 0; i < 10; i++)
        clearPanel(i);
}

QBox::~QBox()
{
    this->mh3u = NULL;
}

void QBox::buttonClicked(int id)
{
    equipment_type_e newType(MH3U_Type::NoneType), oldType(MH3U_Type::NoneType);
    equipment_subtype_e subtype;

    do
    {
        oldType = (equipment_type_e) this->mh3u->savedata->box[id / 100][id % 100][0];
        subtype = MH3U_Armory::convertSubtype(oldType);

        switch (subtype)
        {
            case MH3U_Type::ArmorSubtype:
            {
                armor_t armor = MH3U_Armory::convertEquipmentToArmor(this->mh3u->savedata->box[id / 100][id % 100]);

                QArmor *qarmor = new QArmor(&armor, this);
                qarmor->setModal(true);
                qarmor->exec();
                delete qarmor;

                MH3U_Armory::convertArmorToEquipment(armor, this->mh3u->savedata->box[id / 100][id % 100]);
                break;
            }
            case MH3U_Type::CharmSubtype:
            {
                charm_t charm = MH3U_Armory::convertEquipmentToCharm(this->mh3u->savedata->box[id / 100][id % 100]);

                QCharm *qcharm = new QCharm(&charm, this);
                qcharm->setModal(true);
                qcharm->exec();
                delete qcharm;

                MH3U_Armory::convertCharmToEquipment(charm, this->mh3u->savedata->box[id / 100][id % 100]);
                break;
            }
            case MH3U_Type::WeaponSubtype:
            {
                std::cout << "Weapon!" << std::endl;

                weapon_t weapon = MH3U_Armory::convertEquipmentToWeapon(this->mh3u->savedata->box[id / 100][id % 100]);

                QWeapon *qweapon = new QWeapon(&weapon, this);
                qweapon->setModal(true);
                qweapon->exec();
                delete qweapon;

                MH3U_Armory::convertWeaponToEquipment(weapon, this->mh3u->savedata->box[id / 100][id % 100]);

                break;
            }
            default:
            {
                QEquipment *qequipment = new QEquipment(&(this->mh3u->savedata->box[id / 100][id % 100]), this);
                qequipment->setModal(true);
                qequipment->exec();
                delete qequipment;
                break;
            }
        }

        newType = (equipment_type_e) this->mh3u->savedata->box[id / 100][id % 100][0];

    } while (oldType != newType);

    m_buttons[id / 100][id % 100]->setText(QString::number(this->mh3u->savedata->box[id / 100][id % 100][2] + this->mh3u->savedata->box[id / 100][id % 100][3] * 0x100));
}

void QBox::fillPanelX99(int panelIndex)
{
    std::vector<std::pair<uint8_t, uint16_t>> unusedEquipment = collectUnusedEquipment(this->mh3u);
    size_t nextUnused = 0;

    for (uint32_t j = 0; j < 100; j++)
    {
        uint8_t type = this->mh3u->savedata->box[panelIndex][j][0];
        uint16_t id = this->mh3u->savedata->box[panelIndex][j][2] + (this->mh3u->savedata->box[panelIndex][j][3] << 8);

        if (type != 0 && id != 0)
            continue;

        if (nextUnused >= unusedEquipment.size())
            continue;

        uint8_t newType = unusedEquipment[nextUnused].first;
        uint16_t newId = unusedEquipment[nextUnused].second;
        nextUnused++;

        memset(this->mh3u->savedata->box[panelIndex][j], 0, sizeof(equipment_t));
        this->mh3u->savedata->box[panelIndex][j][0] = newType;
        this->mh3u->savedata->box[panelIndex][j][1] = 0;
        this->mh3u->savedata->box[panelIndex][j][2] = newId & 0xff;
        this->mh3u->savedata->box[panelIndex][j][3] = (newId >> 8) & 0xff;
        m_buttons[panelIndex][j]->setText(QString::number(newId));
    }
}

void QBox::clearPanel(int panelIndex)
{
    for (uint32_t j = 0; j < 100; j++)
    {
        memset(this->mh3u->savedata->box[panelIndex][j], 0, sizeof(equipment_t));
        m_buttons[panelIndex][j]->setText(QString::number(0));
    }
}
