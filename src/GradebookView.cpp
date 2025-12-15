#include "GradebookView.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QDate>
#include <QKeyEvent>
#include <QColor>
#include <QFileDialog>

GradebookView::GradebookView(std::shared_ptr<AcademicManager> manager, QWidget *parent)
    : QWidget(parent), m_manager(manager), m_loading(false) {
    setupUi();
    refreshClasses();
}

void GradebookView::setupUi() {
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Top Controls: Class Selector
    QHBoxLayout *topLayout = new QHBoxLayout();
    m_classCombo = new QComboBox();
    topLayout->addWidget(new QLabel("Select Class:"));
    topLayout->addWidget(m_classCombo);
    topLayout->addStretch();
    layout->addLayout(topLayout);

    // Tab Widget
    m_tabWidget = new QTabWidget();
    layout->addWidget(m_tabWidget);

    setupGradesTab();
    setupAttendanceTab();

    m_tabWidget->addTab(m_gradesTab, "Grades");
    m_tabWidget->addTab(m_attendanceTab, "Attendance");

    connect(m_classCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &GradebookView::onClassChanged);
}

void GradebookView::setupGradesTab() {
    m_gradesTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_gradesTab);

    // Table
    m_gradesTable = new QTableWidget();
    m_gradesTable->setColumnCount(7); // Student, B1, B2, B3, B4, Final, Average/Status
    m_gradesTable->setHorizontalHeaderLabels({"Student", "B1", "B2", "B3", "B4", "Final", "Status"});
    m_gradesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(m_gradesTable);

    // Bottom Controls: Enroll Student
    QHBoxLayout *botLayout = new QHBoxLayout();
    m_studentSelector = new QComboBox();
    QPushButton *enrollBtn = new QPushButton("Enroll Student");
    botLayout->addWidget(new QLabel("Enroll Student:"));
    botLayout->addWidget(m_studentSelector);
    botLayout->addWidget(enrollBtn);
    botLayout->addStretch();

    QPushButton *exportBtn = new QPushButton("Export Grades");
    botLayout->addWidget(exportBtn);

    layout->addLayout(botLayout);

    connect(m_gradesTable, &QTableWidget::cellChanged, this, &GradebookView::onCellChanged);
    connect(enrollBtn, &QPushButton::clicked, this, &GradebookView::onEnrollStudent);
    connect(exportBtn, &QPushButton::clicked, this, &GradebookView::onExportGrades);
}

void GradebookView::setupAttendanceTab() {
    m_attendanceTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_attendanceTab);

    // Top controls for date selection
    QHBoxLayout *topLayout = new QHBoxLayout();
    m_dateEdit = new QDateEdit(QDate::currentDate());
    m_dateEdit->setCalendarPopup(true);
    topLayout->addWidget(new QLabel("Date:"));
    topLayout->addWidget(m_dateEdit);
    topLayout->addStretch();
    layout->addLayout(topLayout);

    // Table
    m_attendanceTable = new QTableWidget();
    m_attendanceTable->setColumnCount(2); // Student Name, Present (Checkbox)
    m_attendanceTable->setHorizontalHeaderLabels({"Student", "Present"});
    m_attendanceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(m_attendanceTable);

    // Bottom controls
    QPushButton *saveBtn = new QPushButton("Save Attendance");
    layout->addWidget(saveBtn, 0, Qt::AlignRight);

    connect(m_dateEdit, &QDateEdit::dateChanged, this, &GradebookView::onDateChanged);
    connect(saveBtn, &QPushButton::clicked, this, &GradebookView::onSaveAttendance);
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
    m_gradesTable->setRowCount(0);

    int classId = m_classCombo->currentData().toInt();
    if (classId <= 0) {
        m_loading = false;
        return;
    }

    auto enrollments = m_manager->getClassEnrollments(classId);

    m_gradesTable->setRowCount(enrollments.size());

    for (int i = 0; i < enrollments.size(); ++i) {
        const auto& e = enrollments[i];

        QTableWidgetItem *nameItem = new QTableWidgetItem(e.studentName);
        nameItem->setData(Qt::UserRole, e.studentId); // Store ID
        nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable); // Read only

        m_gradesTable->setItem(i, 0, nameItem);
        m_gradesTable->setItem(i, 1, new QTableWidgetItem(QString::number(e.grade.b1)));
        m_gradesTable->setItem(i, 2, new QTableWidgetItem(QString::number(e.grade.b2)));
        m_gradesTable->setItem(i, 3, new QTableWidgetItem(QString::number(e.grade.b3)));
        m_gradesTable->setItem(i, 4, new QTableWidgetItem(QString::number(e.grade.b4)));

        QString finalStr = e.grade.hasFinal ? QString::number(e.grade.finalGrade) : "";
        m_gradesTable->setItem(i, 5, new QTableWidgetItem(finalStr));

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
        m_gradesTable->setItem(i, 6, statItem);
    }

    m_loading = false;
}

void GradebookView::onClassChanged(int index) {
    if (index >= 0) {
        refreshGrades();
        refreshAttendance();
    }
}

void GradebookView::onCellChanged(int row, int column) {
    if (m_loading) return;

    // Columns 1-5 are editable grades
    if (column < 1 || column > 5) return;

    int classId = m_classCombo->currentData().toInt();
    int studentId = m_gradesTable->item(row, 0)->data(Qt::UserRole).toInt();

    QTableWidgetItem *item = m_gradesTable->item(row, column);
    if (item) {
        double grade = item->text().toDouble();
        if (grade < 6.0) {
            item->setForeground(QBrush(Qt::red));
        } else {
            item->setForeground(QBrush(Qt::black));
        }
    }

    double b1 = m_gradesTable->item(row, 1)->text().toDouble();
    double b2 = m_gradesTable->item(row, 2)->text().toDouble();
    double b3 = m_gradesTable->item(row, 3)->text().toDouble();
    double b4 = m_gradesTable->item(row, 4)->text().toDouble();

    QString finalStr = m_gradesTable->item(row, 5)->text();
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

        QTableWidgetItem *statItem = m_gradesTable->item(row, 6);
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

void GradebookView::refreshAttendance() {
    m_loading = true;
    m_attendanceTable->setRowCount(0);

    int classId = m_classCombo->currentData().toInt();
    if (classId <= 0) {
        m_loading = false;
        return;
    }

    auto enrollments = m_manager->getClassEnrollments(classId);
    QString date = m_dateEdit->date().toString("yyyy-MM-dd");
    auto attendanceRecords = m_manager->getAttendanceForClass(classId, date);

    m_attendanceTable->setRowCount(enrollments.size());

    for (int i = 0; i < enrollments.size(); ++i) {
        const auto& enrollment = enrollments[i];

        QTableWidgetItem *nameItem = new QTableWidgetItem(enrollment.studentName);
        nameItem->setData(Qt::UserRole, enrollment.studentId);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        m_attendanceTable->setItem(i, 0, nameItem);

        QTableWidgetItem *checkItem = new QTableWidgetItem();
        checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        checkItem->setCheckState(Qt::Unchecked);

        // Check if there is an attendance record for this student
        for (const auto& record : attendanceRecords) {
            if (record.studentId == enrollment.studentId) {
                checkItem->setCheckState(record.present ? Qt::Checked : Qt::Unchecked);
                break;
            }
        }

        m_attendanceTable->setItem(i, 1, checkItem);
    }

    m_loading = false;
}

void GradebookView::onDateChanged(const QDate &date) {
    refreshAttendance();
}

void GradebookView::onSaveAttendance() {
    int classId = m_classCombo->currentData().toInt();
    if (classId <= 0) return;

    QString date = m_dateEdit->date().toString("yyyy-MM-dd");

    for (int i = 0; i < m_attendanceTable->rowCount(); ++i) {
        int studentId = m_attendanceTable->item(i, 0)->data(Qt::UserRole).toInt();
        bool isPresent = m_attendanceTable->item(i, 1)->checkState() == Qt::Checked;
        m_manager->updateAttendance(studentId, classId, date, isPresent);
    }

    QMessageBox::information(this, "Success", "Attendance saved successfully!");
    refreshAttendance();
}

void GradebookView::keyPressEvent(QKeyEvent *event) {
    if (m_gradesTable->hasFocus()) {
        int row = m_gradesTable->currentRow();
        int col = m_gradesTable->currentColumn();

        switch (event->key()) {
            case Qt::Key_Up:
                if (row > 0) {
                    m_gradesTable->setCurrentCell(row - 1, col);
                }
                break;
            case Qt::Key_Down:
                if (row < m_gradesTable->rowCount() - 1) {
                    m_gradesTable->setCurrentCell(row + 1, col);
                }
                break;
            case Qt::Key_Left:
                if (col > 1) { // Can't edit student name
                    m_gradesTable->setCurrentCell(row, col - 1);
                }
                break;
            case Qt::Key_Right:
                if (col < m_gradesTable->columnCount() - 2) { // Can't edit status
                    m_gradesTable->setCurrentCell(row, col + 1);
                }
                break;
            default:
                QWidget::keyPressEvent(event);
        }
    } else {
        QWidget::keyPressEvent(event);
    }
}

void GradebookView::onExportGrades() {
    int classId = m_classCombo->currentData().toInt();
    if (classId <= 0) {
        QMessageBox::warning(this, "No Class Selected", "Please select a class to export.");
        return;
    }

    QString csvData = m_manager->exportClassGradesAsCsv(classId);
    if (csvData.isEmpty()) {
        QMessageBox::information(this, "No Data", "There is no data to export for this class.");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Save Grades", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << csvData;
        file.close();
        QMessageBox::information(this, "Success", "Grades exported successfully!");
    } else {
        QMessageBox::warning(this, "Error", "Could not save file.");
    }
}
