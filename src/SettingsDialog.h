#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

class QComboBox;
class QLineEdit;
class QPushButton;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    void onProviderChanged(int index);
    void saveSettings();
    void loadSettings();

private:
    QComboBox *m_providerCombo;
    QLineEdit *m_urlEdit;
    QLineEdit *m_apiKeyEdit;
    QLineEdit *m_modelEdit;
    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

    void setupUi();
};

#endif // SETTINGSDIALOG_H
