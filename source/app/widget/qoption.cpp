#include "qoption.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

QOption::QOption(QWidget *parent) : QDialog(parent)
{
    m_languageButton = new QComboBox();
    m_languageButton->addItem("English", LANG_EN);
    m_languageButton->addItem("French", LANG_FR);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(new QLabel("Language", this), 0, 0);
    layout->addWidget(m_languageButton, 1, 0);
    this->setLayout(layout);
    this->setWindowTitle("MH3U - Options");

    this->load();
}


void QOption::closeEvent(QCloseEvent *)
{
    this->save();
}


void QOption::load()
{
    m_languageButton->setCurrentIndex(m_languageButton->findData(MH3U_DS::lang()));
}

void QOption::save()
{
    lang_t selected = (lang_t) m_languageButton->currentData().toUInt();

    if (selected != MH3U_DS::lang() && !MH3U_DS::readData(selected))
    {
        QMessageBox::warning(this, "MH3U - Options",
            "Failed to load the data files for the selected language.\n\n"
            "The language has not been changed.");
    }
}
