#ifndef QUTIL_HPP
#define QUTIL_HPP

#include "main.hpp"

#include <string>

class QComboBox;
class QSpinBox;
class QWidget;

// Fills a combo box from a dataset, using each entry's identifier as the label
// and its count as the associated data. When withNone is true a leading
// "(None)" entry mapped to 0 is inserted first.
void populateComboBox(QComboBox *combo, const dataset_t *dataset, bool withNone = true);

// Creates a QSpinBox parented to parent with the given inclusive range.
QSpinBox *makeSpinBox(QWidget *parent, int minimum, int maximum);

// True when the identifier denotes an unused/placeholder ("DUMMY") entry.
bool isDummyIdentifier(const std::string &identifier);

#endif // QUTIL_HPP
