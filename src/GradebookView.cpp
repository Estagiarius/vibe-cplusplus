#include "GradebookView.h"
#include <QHeaderView>
#include <QMessageBox>

GradebookView::GradebookView(std::shared_ptr<AcademicManager> manager, QWidget *parent)
    : QWidget(parent), m_manager(manager), m_loading(false) {

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Top Controls: Class Selector
    QHBoxLayout *topLayout = new QHBoxLayout();
    m_classCombo = new QComboBox();
    topLayout->addWidget(new QLabel("Select Class:"));
    topLayout->addWidget(m_classCombo);
    topLayout->addStretch();

    layout->addLayout(topLayout);

    // Table
    m_table = new QTableWidget();
    m_table->setColumnCount(7); // Student, B1, B2, B3, B4, Final, Average/Status
    m_table->setHorizontalHeaderLabels({"Student", "B1", "B2", "B3", "B4", "Final", "Status"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(m_table);

    // Bottom Controls: Enroll Student
    QHBoxLayout *botLayout = new QHBoxLayout();
    m_studentSelector = new QComboBox();
    QPushButton *enrollBtn = new QPushButton("Enroll Student");
    botLayout->addWidget(new QLabel("Enroll Student:"));
    botLayout->addWidget(m_studentSelector);
    botLayout->addWidget(enrollBtn);
    botLayout->addStretch();

    layout->addLayout(botLayout);

    connect(m_classCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &GradebookView::onClassChanged);
    connect(m_table, &QTableWidget::cellChanged, this, &GradebookView::onCellChanged);
    connect(enrollBtn, &QPushButton::clicked, this, &GradebookView::onEnrollStudent);

    refreshClasses();
}

void GradebookView::refreshClasses() {
    m_loading = true;
    m_classCombo->clear();
    auto classes = m_manager->getAllClasses();
    auto courses = m_manager->getAllCourses(); // To resolve names

    for (const auto& c : classes) {
        QString name = "Unknown";
        for (const auto& co : courses) { if(co.id == c.courseId) { name = co.name; break; } }
        QString label = QString("%1 - %2").arg(name, c.semester);
        m_classCombo->addItem(label, c.id);
    }

    // Also refresh student selector
    m_studentSelector->clear();
    auto students = m_manager->getAllStudents();
    for (const auto& s : students) {
        m_studentSelector->addItem(s.name, s.id);
    }

    m_loading = false;
    if (m_classCombo->count() > 0) refreshGrades();
}

void GradebookView::refreshGrades() {
    m_loading = true;
    m_table->setRowCount(0);

    int classId = m_classCombo->currentData().toInt();
    if (classId <= 0) {
        m_loading = false;
        return;
    }

    auto enrollments = m_manager->getClassEnrollments(classId);

    m_table->setRowCount(enrollments.size());

    for (int i = 0; i < enrollments.size(); ++i) {
        const auto& e = enrollments[i];

        QTableWidgetItem *nameItem = new QTableWidgetItem(e.studentName);
        nameItem->setData(Qt::UserRole, e.studentId); // Store ID
        nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable); // Read only

        m_table->setItem(i, 0, nameItem);
        m_table->setItem(i, 1, new QTableWidgetItem(QString::number(e.grade.b1)));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::number(e.grade.b2)));
        m_table->setItem(i, 3, new QTableWidgetItem(QString::number(e.grade.b3)));
        m_table->setItem(i, 4, new QTableWidgetItem(QString::number(e.grade.b4)));

        QString finalStr = e.grade.hasFinal ? QString::number(e.grade.finalGrade) : "";
        m_table->setItem(i, 5, new QTableWidgetItem(finalStr));

        // Calculate status display
        StudentStatus status = m_manager->calculateStatus(e.studentId, classId);
        QString statusStr = QString("%1 (%2)").arg(QString::number(status.average, 'f', 2), status.statusMessage);

        QTableWidgetItem *statItem = new QTableWidgetItem(statusStr);
        if (!status.approved && status.statusMessage.contains("Recuperação")) {
            statItem->setForeground(QBrush(Qt::darkYellow));
            statItem->setToolTip("Student needs to take the Final Exam");
        } else if (!status.approved) {
            statItem->setForeground(QBrush(Qt::red));
        } else {
            statItem->setForeground(QBrush(Qt::darkGreen));
        }
        statItem->setFlags(statItem->flags() ^ Qt::ItemIsEditable);
        m_table->setItem(i, 6, statItem);
    }

    m_loading = false;
}

void GradebookView::onClassChanged(int index) {
    if (index >= 0) refreshGrades();
}

void GradebookView::onCellChanged(int row, int column) {
    if (m_loading) return;

    // Columns 1-5 are editable grades
    if (column < 1 || column > 5) return;

    int classId = m_classCombo->currentData().toInt();
    int studentId = m_table->item(row, 0)->data(Qt::UserRole).toInt();

    double b1 = m_table->item(row, 1)->text().toDouble();
    double b2 = m_table->item(row, 2)->text().toDouble();
    double b3 = m_table->item(row, 3)->text().toDouble();
    double b4 = m_table->item(row, 4)->text().toDouble();

    QString finalStr = m_table->item(row, 5)->text();
    double finalGrade = -1;
    if (!finalStr.isEmpty()) {
        finalGrade = finalStr.toDouble();
    }

    bool ok = m_manager->updateGradesById(studentId, classId, b1, b2, b3, b4, finalGrade);
    if (ok) {
        // Refresh status cell
        m_loading = true; // Prevent loop
        StudentStatus status = m_manager->calculateStatus(studentId, classId);
        QString statusStr = QString("%1 (%2)").arg(QString::number(status.average, 'f', 2), status.statusMessage);

        QTableWidgetItem *statItem = m_table->item(row, 6);
        statItem->setText(statusStr);

        if (!status.approved && status.statusMessage.contains("Recuperação")) {
            statItem->setForeground(QBrush(Qt::darkYellow));
        } else if (!status.approved) {
            statItem->setForeground(QBrush(Qt::red));
        } else {
            statItem->setForeground(QBrush(Qt::darkGreen));
        }

        m_loading = false;
    } else {
        // Revert change if invalid
        m_loading = true;
        QMessageBox::warning(this, "Invalid Grade", "Grades must be between 0.0 and 10.0");
        refreshGrades(); // Re-fetch correct data to revert UI
        m_loading = false;
    }
}

void GradebookView::onEnrollStudent() {
    int classId = m_classCombo->currentData().toInt();
    int studentId = m_studentSelector->currentData().toInt();

    if (classId <= 0 || studentId <= 0) return;

    if (m_manager->enrollStudentById(studentId, classId)) {
        refreshGrades();
    } else {
        QMessageBox::warning(this, "Error", "Failed to enroll (maybe already enrolled?)");
    }
}
