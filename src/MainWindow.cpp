#include "MainWindow.h"
#include "ManagementWidgets.h"
#include "GradebookView.h"
#include <QMessageBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QMenuBar>
#include <QMenu>
#include <QClipboard>
#include <QApplication>

MainWindow::MainWindow(std::shared_ptr<AcademicManager> manager, std::shared_ptr<AIClient> aiClient, QWidget *parent)
    : QMainWindow(parent), m_manager(manager), m_aiClient(aiClient) {

    setupUi();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    setWindowTitle("Academic Management System - AI Powered");
    resize(1024, 768);

    // Menu Bar
    QMenu *fileMenu = menuBar()->addMenu("&File");
    QAction *settingsAction = new QAction("&Settings", this);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onSettings);
    fileMenu->addAction(settingsAction);


    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    m_mainTabs = new QTabWidget(this);
    mainLayout->addWidget(m_mainTabs);

    setupManagementTab();
    setupChatTab();

    connect(m_mainTabs, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

void MainWindow::setupManagementTab() {
    // Instead of a single widget, we use nested tabs for different management entities
    QTabWidget *mgmtTabs = new QTabWidget();

    // 1. Students
    m_studentView = new StudentView(m_manager);
    mgmtTabs->addTab(m_studentView, "Students");

    // 2. Courses
    m_courseView = new CourseView(m_manager);
    mgmtTabs->addTab(m_courseView, "Courses");

    // 3. Classes
    m_classView = new ClassView(m_manager);
    mgmtTabs->addTab(m_classView, "Classes");

    // 4. Gradebook
    m_gradebookView = new GradebookView(m_manager);
    mgmtTabs->addTab(m_gradebookView, "Gradebook");

    // Add logic to refresh specific tab when selected?
    // For now, views refresh themselves on actions, but might need refresh if data changed elsewhere.
    connect(mgmtTabs, &QTabWidget::currentChanged, [this](int index) {
        if (index == 0) m_studentView->refresh();
        if (index == 1) m_courseView->refresh();
        if (index == 2) m_classView->refresh();
        if (index == 3) m_gradebookView->refreshClasses();
    });

    m_mainTabs->addTab(mgmtTabs, "Management");
}

void MainWindow::setupChatTab() {
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);

    m_chatDisplay = new QTextEdit();
    m_chatDisplay->setReadOnly(true);

    m_copyButton = new QPushButton("Copy Last Response");
    m_copyButton->setEnabled(false); // Disabled until first AI response

    QHBoxLayout *displayLayout = new QHBoxLayout();
    displayLayout->addWidget(m_chatDisplay);
    QVBoxLayout* displayButtons = new QVBoxLayout();
    displayButtons->addWidget(m_copyButton);
    displayButtons->addStretch();
    displayLayout->addLayout(displayButtons);
    layout->addLayout(displayLayout);

    m_typingIndicator = new QLabel("AI is typing...");
    m_typingIndicator->setVisible(false);
    layout->addWidget(m_typingIndicator);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    m_chatInput = new QLineEdit();
    m_sendBtn = new QPushButton("Send");

    inputLayout->addWidget(m_chatInput);
    inputLayout->addWidget(m_sendBtn);

    layout->addLayout(inputLayout);

    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSendChat);
    connect(m_chatInput, &QLineEdit::returnPressed, this, &MainWindow::onSendChat);
    connect(m_copyButton, &QPushButton::clicked, this, &MainWindow::onCopyResponse);


    m_mainTabs->addTab(tab, "AI Assistant");
}

void MainWindow::onTabChanged(int index) {
    // If we switch back to Management, maybe refresh the current sub-tab?
    // Not strictly necessary if the sub-tab handler handles it.
}

void MainWindow::onSendChat() {
    QString msg = m_chatInput->text().trimmed();
    if (msg.isEmpty()) return;

    appendChatMessage("User", msg);
    m_chatInput->clear();
    m_chatInput->setEnabled(false);
    m_sendBtn->setEnabled(false);
    m_typingIndicator->setVisible(true);
    m_copyButton->setEnabled(false);


    m_aiClient->sendMessage(msg, [this](QString response) {
        appendChatMessage("AI", response);
        m_chatInput->setEnabled(true);
        m_sendBtn->setEnabled(true);
        m_typingIndicator->setVisible(false);
        m_copyButton->setEnabled(true);
        m_chatInput->setFocus();

        // After AI action, refresh views just in case
        // We could emit a signal, or just brute force refresh visible ones
        if (m_studentView) m_studentView->refresh();
        if (m_courseView) m_courseView->refresh();
        if (m_classView) m_classView->refresh();
        if (m_gradebookView) m_gradebookView->refreshClasses(); // Updates combo box if courses added
    });
}

void MainWindow::appendChatMessage(const QString& sender, const QString& message) {
    m_chatDisplay->append(QString("<b>%1:</b> %2").arg(sender, message));
    if (sender == "AI") {
        m_lastResponse = message;
    }
}

void MainWindow::onCopyResponse() {
    QApplication::clipboard()->setText(m_lastResponse);
    QMessageBox::information(this, "Copied", "AI response copied to clipboard.");
}

void MainWindow::onSettings() {
    SettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        // Reload settings in AIClient
        m_aiClient->loadSettings();
        QMessageBox::information(this, "Settings Updated", "AI client settings have been reloaded.");
    }
}
