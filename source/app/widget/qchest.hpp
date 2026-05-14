#ifndef QCHEST_HPP
#define QCHEST_HPP

#include "main.hpp"

#include "qitem.hpp"

#include <QWidget>
#include <QPushButton>

class QChest : public QWidget
{
    Q_OBJECT
public:
    explicit QChest(MH3U_SE *mh3u, QWidget *parent = 0);
    ~QChest();

private:
    MH3U_SE *mh3u;
    QPushButton *m_buttons[10][100];

public slots:
    void buttonClicked(int id);
    void fillPanelX99(int panelIndex);
    void fillAllPanelsX99();
    void fillPanelRange(int panelIndex);
    void clearPanel(int panelIndex);
    void clearAllPanels();
};

#endif // QCHEST_HPP
