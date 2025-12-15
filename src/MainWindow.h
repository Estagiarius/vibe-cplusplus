#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QStandardItemModel>
#include <memory>
#include "AcademicManager.h"
#include "AIClient.h"
#include "SettingsDialog.h"

// Forward declarations of our new widgets
class StudentView;
class CourseView;
class ClassView;
class GradebookView;

/**
 * @brief A classe MainWindow é a janela principal da aplicação.
 *
 * Contém o widget principal de separadores com as interfaces de gestão e de conversação com IA,
 * bem como a barra de menus para aceder às definições.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constrói uma nova MainWindow.
     * @param manager Um ponteiro partilhado para o AcademicManager para a lógica de negócio.
     * @param aiClient Um ponteiro partilhado para o AIClient para interacções de IA.
     * @param parent O widget pai.
     */
    MainWindow(std::shared_ptr<AcademicManager> manager, std::shared_ptr<AIClient> aiClient, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // AI Chat
    /**
     * @brief Chamado quando o usuário envia uma mensagem no chat.
     */
    void onSendChat();

    /**
     * @brief Anexa uma mensagem à exibição do chat.
     * @param sender O remetente da mensagem (por exemplo, "Usuário" ou "IA").
     * @param message O conteúdo da mensagem.
     */
    void appendChatMessage(const QString& sender, const QString& message);

    /**
     * @brief Abre a caixa de diálogo de configurações da IA.
     */
    void onSettings();

    /**
     * @brief Copia a última resposta da IA para a área de transferência.
     */
    void onCopyResponse();

    // Sync slots (refresh views when tabs change)
    void onTabChanged(int index);

private:
    std::shared_ptr<AcademicManager> m_manager;
    std::shared_ptr<AIClient> m_aiClient;

    QTabWidget *m_mainTabs;

    // Management Views
    StudentView *m_studentView;
    CourseView *m_courseView;
    ClassView *m_classView;
    GradebookView *m_gradebookView;

    // Chat Widgets
    QTextEdit *m_chatDisplay;
    QLineEdit *m_chatInput;
    QPushButton *m_sendBtn;
    QLabel *m_typingIndicator;
    QPushButton *m_copyButton;

    QString m_lastResponse;

    /**
     * @brief Configura a interface principal da janela.
     */
    void setupUi();

    /**
     * @brief Configura a aba de gerenciamento com todas as visualizações de gerenciamento.
     */
    void setupManagementTab();

    /**
     * @brief Configura a aba de chat do assistente de IA.
     */
    void setupChatTab();
};

#endif // MAINWINDOW_H
