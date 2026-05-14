#ifndef QBOX_HPP
#define QBOX_HPP

#include "main.hpp"

#include "qequipment.hpp"
#include "qarmor.hpp"
#include "qcharm.hpp"
#include "qweapon.hpp"

#include <QWidget>
#include <QPushButton>

class QBox : public QWidget
{
    Q_OBJECT
public:
    explicit QBox(MH3U_SE *mh3u, QWidget *parent = 0);
    ~QBox();

private:
    MH3U_SE *mh3u;
    QPushButton *m_buttons[10][100];

public slots:
    void buttonClicked(int id);
    void fillPanelX99(int panelIndex);
    void fillAllPanelsX99();
    void clearPanel(int panelIndex);
    void clearAllPanels();
};

#endif // QBOX_HPP
