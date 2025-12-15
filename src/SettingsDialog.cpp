#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>

/**
 * @brief Constrói um novo SettingsDialog.
 *
 * Inicializa a UI e carrega as últimas configurações salvas.
 * @param parent O widget pai.
 */
SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
    setupUi();
    loadSettings();
}

/**
 * @brief Configura a interface do usuário da caixa de diálogo.
 *
 * Cria e organiza todos os widgets para a caixa de diálogo de configurações.
 */
void SettingsDialog::setupUi() {
    setWindowTitle("AI Settings");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    m_providerCombo = new QComboBox();
    m_providerCombo->addItem("Ollama (Local)", "ollama");
    m_providerCombo->addItem("OpenAI", "openai");

    m_urlEdit = new QLineEdit();
    m_apiKeyEdit = new QLineEdit();
    m_apiKeyEdit->setEchoMode(QLineEdit::Password);
    m_modelEdit = new QLineEdit();


    formLayout->addRow("AI Provider:", m_providerCombo);
    formLayout->addRow("Endpoint URL:", m_urlEdit);
    formLayout->addRow("API Key:", m_apiKeyEdit);
    formLayout->addRow("Model Name:", m_modelEdit);


    QGroupBox *settingsGroup = new QGroupBox("API Configuration");
    settingsGroup->setLayout(formLayout);
    mainLayout->addWidget(settingsGroup);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_saveButton = new QPushButton("Save");
    m_cancelButton = new QPushButton("Cancel");
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(m_providerCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsDialog::onProviderChanged);
    connect(m_saveButton, &QPushButton::clicked, this, &SettingsDialog::saveSettings);
    connect(m_cancelButton, &QPushButton::clicked, this, &SettingsDialog::reject);
}

/**
 * @brief Lida com a alteração do provedor de IA.
 *
 * Quando o provedor é alterado, este slot atualiza a UI com valores padrão
 * para o provedor selecionado. Por exemplo, ele define uma URL padrão para o Ollama.
 * @param index O novo índice da caixa de combinação de provedores.
 */
void SettingsDialog::onProviderChanged(int index) {
    QString provider = m_providerCombo->itemData(index).toString();
    if (provider == "ollama") {
        m_urlEdit->setText("http://localhost:11434/v1/chat/completions");
        m_apiKeyEdit->setText("ollama");
        m_apiKeyEdit->setEnabled(false);
    } else if (provider == "openai") {
        m_urlEdit->setText("https://api.openai.com/v1/chat/completions");
        m_apiKeyEdit->clear();
        m_apiKeyEdit->setEnabled(true);
    }
}

/**
 * @brief Salva as configurações no armazenamento persistente.
 *
 * Usa o QSettings para salvar o provedor, URL, chave de API e nome do modelo.
 */
void SettingsDialog::saveSettings() {
    QSettings settings("MySoft", "AcademicManager");
    settings.setValue("ai/provider", m_providerCombo->currentData().toString());
    settings.setValue("ai/base_url", m_urlEdit->text());
    settings.setValue("ai/api_key", m_apiKeyEdit->text());
    settings.setValue("ai/model", m_modelEdit->text());


    QMessageBox::information(this, "Settings Saved", "AI settings have been updated.");
    accept();
}

/**
 * @brief Carrega as configurações do armazenamento persistente.
 *
 * Usa o QSettings para carregar o provedor, a URL, a chave de API e o nome do modelo,
 * e preenche os campos da caixa de diálogo com esses valores.
 */
void SettingsDialog::loadSettings() {
    QSettings settings("MySoft", "AcademicManager");
    QString provider = settings.value("ai/provider", "ollama").toString();
    QString url = settings.value("ai/base_url", "http://localhost:11434/v1/chat/completions").toString();
    QString key = settings.value("ai/api_key", "ollama").toString();
    QString model = settings.value("ai/model", "llama3").toString();


    int index = m_providerCombo->findData(provider);
    if (index != -1) {
        m_providerCombo->setCurrentIndex(index);
    }

    m_urlEdit->setText(url);
    m_apiKeyEdit->setText(key);
    m_modelEdit->setText(model);
    onProviderChanged(index); // Update enabled state
}
