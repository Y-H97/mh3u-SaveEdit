#include "qutil.hpp"

#include <QComboBox>
#include <QSpinBox>

#include <algorithm>

void populateComboBox(QComboBox *combo, const dataset_t *dataset, bool withNone)
{
    if (withNone)
        combo->addItem("(None)", 0);

    if (dataset == NULL)
        return;

    for (uint32_t i = 0; i < dataset->size(); i++)
    {
        combo->addItem(QString(dataset->at(i).identifier.c_str()), dataset->at(i).count);
    }
}

QSpinBox *makeSpinBox(QWidget *parent, int minimum, int maximum)
{
    QSpinBox *spinBox = new QSpinBox(parent);
    spinBox->setMinimum(minimum);
    spinBox->setMaximum(maximum);
    return spinBox;
}

bool isDummyIdentifier(const std::string &identifier)
{
    std::string s = identifier;
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s.find("DUMMY") != std::string::npos;
}
