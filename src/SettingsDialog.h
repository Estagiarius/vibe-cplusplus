#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

class QComboBox;
class QLineEdit;
class QPushButton;

/**
 * @brief A classe SettingsDialog fornece uma caixa de diálogo para configurar as definições do provedor de IA.
 *
 * Esta caixa de diálogo permite ao utilizador selecionar um provedor de IA (por exemplo, Ollama, OpenAI),
 * e definir o URL do ponto de extremidade, a chave da API e o nome do modelo correspondentes.
 * As definições são mantidas através de QSettings.
 */
class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constrói um novo SettingsDialog.
     * @param parent O widget pai.
     */
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    /**
     * @brief Lida com a alteração do provedor de IA na caixa de combinação.
     *
     * Atualiza os campos de URL e chave de API com valores padrão para o provedor selecionado.
     * @param index O índice do item selecionado na caixa de combinação.
     */
    void onProviderChanged(int index);

    /**
     * @brief Salva as configurações atuais no QSettings.
     */
    void saveSettings();

    /**
     * @brief Carrega as configurações do QSettings e preenche os campos da caixa de diálogo.
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
     * @brief Configura a interface do usuário da caixa de diálogo.
     */
    void setupUi();
};

#endif // SETTINGSDIALOG_H
