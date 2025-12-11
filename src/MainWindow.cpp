#include "MainWindow.h"
#include <QMessageBox>
#include <QFormLayout>
#include <QGroupBox>

MainWindow::MainWindow(std::shared_ptr<AcademicManager> manager, std::shared_ptr<AIClient> aiClient, QWidget *parent)
    : QMainWindow(parent), m_manager(manager), m_aiClient(aiClient) {

    setupUi();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    setWindowTitle("Academic Management System - AI Powered");
    resize(1024, 768);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    m_tabs = new QTabWidget(this);
    mainLayout->addWidget(m_tabs);

    setupManagementTab();
    setupChatTab();
}

void MainWindow::setupManagementTab() {
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);

    // Student Section
    QGroupBox *studentGroup = new QGroupBox("Add Student");
    QFormLayout *studentLayout = new QFormLayout(studentGroup);

    m_studentNameEdit = new QLineEdit();
    m_studentRegEdit = new QLineEdit();
    QPushButton *addStudentBtn = new QPushButton("Add Student");

    studentLayout->addRow("Name:", m_studentNameEdit);
    studentLayout->addRow("Registration:", m_studentRegEdit);
    studentLayout->addRow(addStudentBtn);

    connect(addStudentBtn, &QPushButton::clicked, this, &MainWindow::onAddStudent);

    layout->addWidget(studentGroup);

    // Placeholder for more management tools
    layout->addStretch();

    m_tabs->addTab(tab, "Management");
}

void MainWindow::setupChatTab() {
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);

    m_chatDisplay = new QTextEdit();
    m_chatDisplay->setReadOnly(true);
    layout->addWidget(m_chatDisplay);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    m_chatInput = new QLineEdit();
    m_sendBtn = new QPushButton("Send");

    inputLayout->addWidget(m_chatInput);
    inputLayout->addWidget(m_sendBtn);

    layout->addLayout(inputLayout);

    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSendChat);
    connect(m_chatInput, &QLineEdit::returnPressed, this, &MainWindow::onSendChat);

    m_tabs->addTab(tab, "AI Assistant");
}

void MainWindow::onAddStudent() {
    QString name = m_studentNameEdit->text();
    QString reg = m_studentRegEdit->text();

    if (name.isEmpty() || reg.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all fields");
        return;
    }

    QString result = m_manager->addStudent(name, reg);
    QMessageBox::information(this, "Result", result);

    m_studentNameEdit->clear();
    m_studentRegEdit->clear();
}

void MainWindow::onAddCourse() {
    // Implementation pending for manual add
}

void MainWindow::onSendChat() {
    QString msg = m_chatInput->text().trimmed();
    if (msg.isEmpty()) return;

    appendChatMessage("User", msg);
    m_chatInput->clear();
    m_chatInput->setEnabled(false);
    m_sendBtn->setEnabled(false);

    m_aiClient->sendMessage(msg, [this](QString response) {
        appendChatMessage("AI", response);
        m_chatInput->setEnabled(true);
        m_sendBtn->setEnabled(true);
        m_chatInput->setFocus();
    });
}

void MainWindow::appendChatMessage(const QString& sender, const QString& message) {
    m_chatDisplay->append(QString("<b>%1:</b> %2").arg(sender, message));
}
