#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QLabel>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Settings");
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Settings Dialog Placeholder"));
    resize(300, 200);
}

SettingsDialog::~SettingsDialog() {}
