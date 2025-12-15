#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

class QComboBox;
class QLineEdit;
class QPushButton;

/**
 * @brief The SettingsDialog class provides a dialog for configuring AI provider settings.
 *
 * This dialog allows the user to select an AI provider (e.g., Ollama, OpenAI),
 * and set the corresponding endpoint URL, API key, and model name.
 * The settings are persisted using QSettings.
 */
class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructs a new SettingsDialog.
     * @param parent The parent widget.
     */
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    /**
     * @brief Handles the change of the AI provider in the combo box.
     *
     * Updates the URL and API key fields with default values for the selected provider.
     * @param index The index of the selected item in the combo box.
     */
    void onProviderChanged(int index);

    /**
     * @brief Saves the current settings to QSettings.
     */
    void saveSettings();

    /**
     * @brief Loads the settings from QSettings and populates the dialog fields.
     */
    void loadSettings();

private:
    QComboBox *m_providerCombo;
    QLineEdit *m_urlEdit;
    QLineEdit *m_apiKeyEdit;
    QLineEdit *m_modelEdit;
    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

    /**
     * @brief Sets up the user interface of the dialog.
     */
    void setupUi();
};

#endif // SETTINGSDIALOG_H
