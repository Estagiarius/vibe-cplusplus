#include "ManagementWidgets.h"
#include <QMessageBox>
#include "EditStudentDialog.h"
#include "EditCourseDialog.h"
#include "EditClassDialog.h"

// Helper to access DB directly or via manager. Manager has Facade, but for models we might need raw lists.
// Since AcademicManager wraps DatabaseManager but doesn't expose list getters yet, I should add them to AcademicManager or access DB via manager->db?
// AcademicManager.h doesn't expose m_db publically.
// I will add getters to AcademicManager or friend class.
// Better practice: Add getters to AcademicManager. I'll modify AcademicManager.h in next step or use what I have.
// Wait, `AcademicManager` currently only has facade for Strings.
// I should update `AcademicManager` to expose lists OR use `DatabaseManager` shared pointer if I pass it or expose it.
// MainWindow has `std::shared_ptr<AcademicManager>`. `AcademicManager` has `m_db`.
// Let's modify AcademicManager to expose getters for Lists.

// ======================= STUDENT VIEW =======================
#include <QMenu>

StudentView::StudentView(std::shared_ptr<AcademicManager> manager, QWidget *parent)
    : QWidget(parent), m_manager(manager) {

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Search
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Buscar por nome ou matrícula...");
    layout->addWidget(m_searchEdit);

    // Table
    m_table = new QTableView();
    m_model = new QStandardItemModel(this);
    m_model->setHorizontalHeaderLabels({"ID", "Name", "Registration"});
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_table->setModel(m_proxyModel);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_table, &QTableView::customContextMenuRequested, this, &StudentView::onShowContextMenu);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &StudentView::onSearch);
    layout->addWidget(m_table);

    // Form
    QGroupBox *groupBox = new QGroupBox("New Student");
    QHBoxLayout *formLayout = new QHBoxLayout(groupBox);

    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("Name");
    m_regEdit = new QLineEdit();
    m_regEdit->setPlaceholderText("Registration");
    QPushButton *addBtn = new QPushButton("Add");

    formLayout->addWidget(m_nameEdit);
    formLayout->addWidget(m_regEdit);
    formLayout->addWidget(addBtn);

    layout->addWidget(groupBox);

    connect(addBtn, &QPushButton::clicked, this, &StudentView::onAdd);

    refresh();
}

void StudentView::refresh() {
    m_model->removeRows(0, m_model->rowCount());
    auto students = m_manager->getAllStudents();
    for (const auto& s : students) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(s.id));
        row << new QStandardItem(s.name);
        row << new QStandardItem(s.registration);
        m_model->appendRow(row);
    }
}

void StudentView::onAdd() {
    if (m_nameEdit->text().isEmpty() || m_regEdit->text().isEmpty()) return;

    m_manager->addStudent(m_nameEdit->text(), m_regEdit->text());
    m_nameEdit->clear();
    m_regEdit->clear();
    refresh();
}

void StudentView::onShowContextMenu(const QPoint &pos) {
    QModelIndex index = m_table->indexAt(pos);
    if (!index.isValid()) {
        return;
    }

    QMenu contextMenu(this);
    QAction *editAction = contextMenu.addAction("Editar");
    QAction *deleteAction = contextMenu.addAction("Excluir");

    connect(editAction, &QAction::triggered, this, &StudentView::onEdit);
    connect(deleteAction, &QAction::triggered, this, &StudentView::onDelete);

    contextMenu.exec(m_table->viewport()->mapToGlobal(pos));
}

void StudentView::onEdit() {
    QModelIndexList selected = m_table->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) {
        return;
    }

    int row = selected.first().row();
    int studentId = m_model->item(row, 0)->text().toInt();
    QString currentName = m_model->item(row, 1)->text();
    QString currentReg = m_model->item(row, 2)->text();

    EditStudentDialog dialog(currentName, currentReg, this);
    if (dialog.exec() == QDialog::Accepted) {
        QString newName = dialog.getName();
        QString newReg = dialog.getRegistration();
        m_manager->updateStudent(studentId, newName, newReg);
        refresh();
    }
}

void StudentView::onDelete() {
    QModelIndexList selected = m_table->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) {
        return;
    }

    int row = selected.first().row();
    int studentId = m_model->item(row, 0)->text().toInt();
    QString studentName = m_model->item(row, 1)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Excluir Aluno", "Tem certeza que deseja excluir " + studentName + "?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QString result = m_manager->deleteStudent(studentId);
        QMessageBox::information(this, "Resultado", result);
        refresh();
    }
}

void StudentView::onSearch(const QString &text) {
    m_proxyModel->setFilterRegularExpression(QRegularExpression(text, QRegularExpression::CaseInsensitiveOption));
    m_proxyModel->setFilterKeyColumn(-1); // Search in all columns
}

// ======================= COURSE VIEW =======================
CourseView::CourseView(std::shared_ptr<AcademicManager> manager, QWidget *parent)
    : QWidget(parent), m_manager(manager) {

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Search
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Buscar por nome ou descrição...");
    layout->addWidget(m_searchEdit);

    // Table
    m_table = new QTableView();
    m_model = new QStandardItemModel(this);
    m_model->setHorizontalHeaderLabels({"ID", "Name", "Description"});
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_table->setModel(m_proxyModel);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_table, &QTableView::customContextMenuRequested, this, &CourseView::onShowContextMenu);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &CourseView::onSearch);
    layout->addWidget(m_table);

    // Form
    QGroupBox *groupBox = new QGroupBox("New Course");
    QHBoxLayout *formLayout = new QHBoxLayout(groupBox);

    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("Course Name");
    m_descEdit = new QLineEdit();
    m_descEdit->setPlaceholderText("Description");
    QPushButton *addBtn = new QPushButton("Add");

    formLayout->addWidget(m_nameEdit);
    formLayout->addWidget(m_descEdit);
    formLayout->addWidget(addBtn);

    layout->addWidget(groupBox);

    connect(addBtn, &QPushButton::clicked, this, &CourseView::onAdd);

    refresh();
}

void CourseView::refresh() {
    m_model->removeRows(0, m_model->rowCount());
    auto courses = m_manager->getAllCourses();
    for (const auto& c : courses) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(c.id));
        row << new QStandardItem(c.name);
        row << new QStandardItem(c.description);
        m_model->appendRow(row);
    }
}

void CourseView::onAdd() {
    if (m_nameEdit->text().isEmpty()) return;

    m_manager->createCourse(m_nameEdit->text(), m_descEdit->text());
    m_nameEdit->clear();
    m_descEdit->clear();
    refresh();
}

void CourseView::onShowContextMenu(const QPoint &pos) {
    QModelIndex index = m_table->indexAt(pos);
    if (!index.isValid()) {
        return;
    }

    QMenu contextMenu(this);
    QAction *editAction = contextMenu.addAction("Editar");
    QAction *deleteAction = contextMenu.addAction("Excluir");

    connect(editAction, &QAction::triggered, this, &CourseView::onEdit);
    connect(deleteAction, &QAction::triggered, this, &CourseView::onDelete);

    contextMenu.exec(m_table->viewport()->mapToGlobal(pos));
}

void CourseView::onEdit() {
    QModelIndexList selected = m_table->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) {
        return;
    }

    int row = selected.first().row();
    int courseId = m_model->item(row, 0)->text().toInt();
    QString currentName = m_model->item(row, 1)->text();
    QString currentDesc = m_model->item(row, 2)->text();

    EditCourseDialog dialog(currentName, currentDesc, this);
    if (dialog.exec() == QDialog::Accepted) {
        QString newName = dialog.getName();
        QString newDesc = dialog.getDescription();
        m_manager->updateCourse(courseId, newName, newDesc);
        refresh();
    }
}

void CourseView::onDelete() {
    QModelIndexList selected = m_table->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) {
        return;
    }

    int row = selected.first().row();
    int courseId = m_model->item(row, 0)->text().toInt();
    QString courseName = m_model->item(row, 1)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Excluir Curso", "Tem certeza que deseja excluir " + courseName + "?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QString result = m_manager->deleteCourse(courseId);
        QMessageBox::information(this, "Resultado", result);
        refresh();
    }
}

void CourseView::onSearch(const QString &text) {
    m_proxyModel->setFilterRegularExpression(QRegularExpression(text, QRegularExpression::CaseInsensitiveOption));
    m_proxyModel->setFilterKeyColumn(-1); // Search in all columns
}

// ======================= CLASS VIEW =======================
ClassView::ClassView(std::shared_ptr<AcademicManager> manager, QWidget *parent)
    : QWidget(parent), m_manager(manager) {

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Table
    m_table = new QTableView();
    m_model = new QStandardItemModel(this);
    m_model->setHorizontalHeaderLabels({"ID", "Course", "Semester"});
    m_table->setModel(m_model);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_table, &QTableView::customContextMenuRequested, this, &ClassView::onShowContextMenu);
    layout->addWidget(m_table);

    // Form
    QGroupBox *groupBox = new QGroupBox("Open Class");
    QHBoxLayout *formLayout = new QHBoxLayout(groupBox);

    m_courseCombo = new QComboBox();
    m_semesterEdit = new QLineEdit();
    m_semesterEdit->setPlaceholderText("Semester (e.g. 2023.1)");
    QPushButton *addBtn = new QPushButton("Open");

    formLayout->addWidget(new QLabel("Course:"));
    formLayout->addWidget(m_courseCombo);
    formLayout->addWidget(m_semesterEdit);
    formLayout->addWidget(addBtn);

    layout->addWidget(groupBox);

    connect(addBtn, &QPushButton::clicked, this, &ClassView::onAdd);

    refresh();
}

void ClassView::refresh() {
    m_model->removeRows(0, m_model->rowCount());
    m_courseCombo->clear();

    auto courses = m_manager->getAllCourses();
    for(const auto& c : courses) {
        m_courseCombo->addItem(c.name, c.id);
    }

    auto classes = m_manager->getAllClasses();
    for (const auto& c : classes) {
        // Find course name efficiently?
        // For now, iterate or use a map if I had it.
        QString courseName = "Unknown";
        for(const auto& co : courses) { if(co.id == c.courseId) { courseName = co.name; break; } }

        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(c.id));
        row << new QStandardItem(courseName);
        row << new QStandardItem(c.semester);
        m_model->appendRow(row);
    }
}

void ClassView::onAdd() {
    if (m_semesterEdit->text().isEmpty() || m_courseCombo->currentIndex() == -1) return;

    QString courseName = m_courseCombo->currentText();
    m_manager->openClass(courseName, m_semesterEdit->text());
    m_semesterEdit->clear();
    refresh();
}

void ClassView::onShowContextMenu(const QPoint &pos) {
    QModelIndex index = m_table->indexAt(pos);
    if (!index.isValid()) {
        return;
    }

    QMenu contextMenu(this);
    QAction *editAction = contextMenu.addAction("Editar");
    QAction *deleteAction = contextMenu.addAction("Excluir");

    connect(editAction, &QAction::triggered, this, &ClassView::onEdit);
    connect(deleteAction, &QAction::triggered, this, &ClassView::onDelete);

    contextMenu.exec(m_table->viewport()->mapToGlobal(pos));
}

void ClassView::onEdit() {
    QModelIndexList selected = m_table->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) {
        return;
    }

    int row = selected.first().row();
    int classId = m_model->item(row, 0)->text().toInt();
    QString currentSemester = m_model->item(row, 2)->text();

    // The model only has the course name, not the ID. We need to find the ID.
    auto classes = m_manager->getAllClasses();
    int courseId = -1;
    for (const auto& c : classes) {
        if (c.id == classId) {
            courseId = c.courseId;
            break;
        }
    }

    if (courseId == -1) {
        QMessageBox::critical(this, "Erro", "Não foi possível encontrar a turma.");
        return;
    }

    auto courses = m_manager->getAllCourses();
    EditClassDialog dialog(courseId, currentSemester, courses, this);
    if (dialog.exec() == QDialog::Accepted) {
        int newCourseId = dialog.getCourseId();
        QString newSemester = dialog.getSemester();
        m_manager->updateClass(classId, newCourseId, newSemester);
        refresh();
    }
}

void ClassView::onDelete() {
    QModelIndexList selected = m_table->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) {
        return;
    }

    int row = selected.first().row();
    int classId = m_model->item(row, 0)->text().toInt();
    QString className = m_model->item(row, 1)->text() + " " + m_model->item(row, 2)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Excluir Turma", "Tem certeza que deseja excluir " + className + "?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QString result = m_manager->deleteClass(classId);
        QMessageBox::information(this, "Resultado", result);
        refresh();
    }
}
