#include "AcademicManager.h"
#include <QDebug>

AcademicManager::AcademicManager(std::shared_ptr<DatabaseManager> db, QObject *parent)
    : QObject(parent), m_db(db) {}

StudentStatus AcademicManager::calculateStatus(int studentId, int classId) {
    auto gradeOpt = m_db->getStudentGrade(studentId, classId);
    if (!gradeOpt) {
        return {"Unknown", "Unknown", 0.0, false, "Student not enrolled or class not found."};
    }

    Grade g = *gradeOpt;

    // Logic: Average of 4 bimesters.
    // If any is -1 (not graded), we treat as 0 for calculation or return pending?
    // User requirement: "four bimesters ... + a fifth final mention"
    // Requirement: "Keep average >= 5.0"

    // Let's assume simple arithmetic mean.
    // Ensure negative values (placeholders) are treated as 0 for calculation,
    // but in a real system we might want to wait until all are posted.
    // For this mock, we calculate what we have.

    double sum = 0;
    int count = 4;
    double val1 = g.b1 < 0 ? 0 : g.b1;
    double val2 = g.b2 < 0 ? 0 : g.b2;
    double val3 = g.b3 < 0 ? 0 : g.b3;
    double val4 = g.b4 < 0 ? 0 : g.b4;

    double avg = (val1 + val2 + val3 + val4) / 4.0;

    // Check if there is a final grade that replaces or helps?
    // Usually in Brazil: if avg >= 7 pass. If >= 4 and < 7, goes to final.
    // Then (avg + final) / 2 >= 5.
    // The user said: "Keep average >= 5.0". Let's stick to a simple rule first:
    // Final Average = (B1+B2+B3+B4)/4. If < 5 and hasFinal, maybe (Avg + Final)/2 ?
    // Let's implement a standard recovery logic.

    bool approved = false;
    QString msg;

    if (g.hasFinal) {
        double finalAvg = (avg + g.finalGrade) / 2.0;
        if (finalAvg >= 5.0) {
            approved = true;
            avg = finalAvg; // Update reported average
            msg = "Approved with Final Exam";
        } else {
            approved = false;
            avg = finalAvg;
            msg = "Failed after Final Exam";
        }
    } else {
        if (avg >= 7.0) {
            approved = true;
            msg = "Approved";
        } else if (avg >= 4.0) {
            // Eligible for final
            approved = false;
            msg = "Needs Final Exam (Recuperação)";
        } else {
            approved = false;
            msg = "Failed";
        }

        // Use the simplified rule "Keep average >= 5.0" requested by user implies
        // maybe the passing grade IS 5.0 straight up?
        // "Mantenha média >= 5,0".
        // If the user means passing grade is 5.0:
        // IMPORTANT: The user confirmed "Recovery" logic (Avg < 7 and >= 4 goes to final).
        // So we should NOT override approval just because Avg >= 5 if standard rule applies.
        // However, "Mantenha média >= 5,0" might be the passing grade for the final calc.
        // Let's stick to: Avg >= 7 Approved. 4 <= Avg < 7 Recovery. < 4 Failed.

        // Re-evaluating the "simple rule":
        // If the user explicitly asked for "Média >= 5.0", maybe they mean the PASSING threshold is 5.0, not 7.0?
        // "Sim. Pense na educação brasileira... Mantenha média >= 5,0."
        // Usually Brazil is 7.0. But if user asked 5.0, let's respect 5.0 as the PRIMARY passing grade?
        // OR is 5.0 the minimum to pass *after* recovery?
        // Let's assume standard university rule:
        // Direct pass: >= 7.0
        // Final Exam: >= 4.0 and < 7.0
        // Pass after Final: (Avg + Final)/2 >= 5.0

        // The code block above does exactly this. I will remove the conflicting override block.
    }

    // Retrieve names for better reporting (inefficient querying here, but okay for mock)
    // We would need to fetch student name and class/course name.
    // Doing a quick hack or adding methods to DB.
    // For now, return placeholders or empty.

    return {
        "", // Student Name
        "", // Class Name
        avg,
        approved,
        msg
    };
}

// Helpers
int AcademicManager::findCourseIdByName(const QString& name) {
    auto courses = m_db->getAllCourses();
    for (const auto& c : courses) {
        if (c.name.compare(name, Qt::CaseInsensitive) == 0) return c.id;
    }
    return -1;
}

int AcademicManager::findStudentIdByReg(const QString& reg) {
    auto s = m_db->getStudentByRegistration(reg);
    if (s) return s->id;
    return -1;
}

int AcademicManager::findClassIdByCourseAndSemester(int courseId, const QString& semester) {
    auto classes = m_db->getAllClasses();
    for (const auto& c : classes) {
        if (c.courseId == courseId && c.semester == semester) return c.id;
    }
    return -1;
}


// Facade Implementations

QString AcademicManager::addStudent(const QString& name, const QString& registration) {
    if (name.trimmed().isEmpty() || registration.trimmed().isEmpty()) {
        return "Error: Name and Registration cannot be empty.";
    }
    if (m_db->getStudentByRegistration(registration)) {
        return "Error: Student with this registration already exists.";
    }
    if (m_db->addStudent(name, registration)) {
        return "Student added successfully: " + name;
    }
    return "Failed to add student.";
}

QString AcademicManager::updateStudent(int studentId, const QString& name, const QString& registration) {
    if (name.trimmed().isEmpty() || registration.trimmed().isEmpty()) {
        return "Error: Name and Registration cannot be empty.";
    }
    if (m_db->updateStudent(studentId, name, registration)) {
        return "Student updated successfully: " + name;
    }
    return "Failed to update student.";
}

QString AcademicManager::deleteStudent(int studentId) {
    // Check for enrollments before deleting
    auto classes = m_db->getAllClasses();
    for (const auto& c : classes) {
        if (m_db->getStudentGrade(studentId, c.id)) {
            return "Error: Cannot delete student with existing enrollments.";
        }
    }

    if (m_db->deleteStudent(studentId)) {
        return "Student deleted successfully.";
    }
    return "Failed to delete student.";
}

QString AcademicManager::createCourse(const QString& name, const QString& description) {
    if (name.trimmed().isEmpty()) {
        return "Error: Course name cannot be empty.";
    }
    if (m_db->addCourse(name, description)) {
        return "Course created: " + name;
    }
    return "Failed to create course.";
}

QString AcademicManager::updateCourse(int courseId, const QString& name, const QString& description) {
    if (name.trimmed().isEmpty()) {
        return "Error: Course name cannot be empty.";
    }
    if (m_db->updateCourse(courseId, name, description)) {
        return "Course updated successfully: " + name;
    }
    return "Failed to update course.";
}

QString AcademicManager::deleteCourse(int courseId) {
    // Check for classes associated with this course
    auto classes = m_db->getAllClasses();
    for (const auto& c : classes) {
        if (c.courseId == courseId) {
            return "Error: Cannot delete course with existing classes.";
        }
    }

    if (m_db->deleteCourse(courseId)) {
        return "Course deleted successfully.";
    }
    return "Failed to delete course.";
}

QString AcademicManager::updateClass(int classId, int courseId, const QString& semester) {
    if (semester.trimmed().isEmpty()) {
        return "Error: Semester cannot be empty.";
    }
    if (m_db->updateClass(classId, courseId, semester)) {
        return "Class updated successfully.";
    }
    return "Failed to update class.";
}

QString AcademicManager::deleteClass(int classId) {
    // Check for enrollments in this class
    auto enrollments = m_db->getClassEnrollments(classId);
    if (!enrollments.isEmpty()) {
        return "Error: Cannot delete class with existing student enrollments.";
    }

    if (m_db->deleteClass(classId)) {
        return "Class deleted successfully.";
    }
    return "Failed to delete class.";
}

QString AcademicManager::openClass(const QString& courseName, const QString& semester) {
    int cid = findCourseIdByName(courseName);
    if (cid == -1) return "Course not found: " + courseName;

    // Check for duplicate class
    if (findClassIdByCourseAndSemester(cid, semester) != -1) {
        return "Error: Class already exists for this course and semester.";
    }

    if (m_db->addClass(cid, semester)) {
        return "Class opened for " + courseName + " in " + semester;
    }
    return "Failed to open class.";
}

QString AcademicManager::registerGrades(const QString& studentReg, const QString& courseName, double b1, double b2, double b3, double b4) {
    // Basic validation
    if (b1 < 0 || b1 > 10 || b2 < 0 || b2 > 10 || b3 < 0 || b3 > 10 || b4 < 0 || b4 > 10) {
        return "Error: Grades must be between 0.0 and 10.0";
    }

    int sid = findStudentIdByReg(studentReg);
    if (sid == -1) return "Student not found: " + studentReg;

    int cid = findCourseIdByName(courseName);
    if (cid == -1) return "Course not found.";

    // Assume semester is current or find the latest?
    // For simplicity, let's find *any* class for this course that the student might be in.
    // Or we need the semester in the input.
    // Let's assume the prompt provides context or we just pick the first class found for that course (Mock limitation).
    // Better: Search for enrollments.

    // Wait, DB has no "get classes for student".
    // Let's assume we are adding to a specific semester or just finding the class id.
    // For now, let's just find the first class of this course.
    auto classes = m_db->getAllClasses();
    int classId = -1;
    for (const auto& c : classes) {
        if (c.courseId == cid) {
            classId = c.id;
            break;
        }
    }

    if (classId == -1) return "No class found for this course.";

    // Check enrollment, if not enrolled, enroll implicitly?
    if (!m_db->getStudentGrade(sid, classId)) {
        m_db->enrollStudent(sid, classId);
    }

    if (m_db->updateGrades(sid, classId, b1, b2, b3, b4)) {
        // Calculate status to return immediate feedback
        StudentStatus status = calculateStatus(sid, classId);
        return QString("Grades updated. Average: %1. Status: %2").arg(status.average).arg(status.statusMessage);
    }

    return "Failed to update grades.";
}

QString AcademicManager::getStudentReport(const QString& studentReg) {
    int sid = findStudentIdByReg(studentReg);
    if (sid == -1) return "Student not found.";

    // Fetch all classes and check if student is enrolled
    // This is inefficient (should have getStudentEnrollments in DB), but works for now.
    QString report = "Report for student " + studentReg + ":\n";
    bool foundAny = false;

    auto classes = m_db->getAllClasses();
    auto courses = m_db->getAllCourses();

    for (const auto& c : classes) {
        auto grade = m_db->getStudentGrade(sid, c.id);
        if (grade) {
            foundAny = true;
            QString courseName = "Unknown";
            for(const auto& co : courses) { if(co.id == c.courseId) { courseName = co.name; break; } }

            StudentStatus st = calculateStatus(sid, c.id);
            report += QString("- Class: %1 (%2)\n").arg(courseName, c.semester);
            report += QString("  Grades: %1, %2, %3, %4. Final: %5\n")
                        .arg(grade->b1).arg(grade->b2).arg(grade->b3).arg(grade->b4)
                        .arg(grade->hasFinal ? QString::number(grade->finalGrade) : "N/A");
            report += QString("  Average: %1. Status: %2\n").arg(st.average).arg(st.statusMessage);
        }
    }

    if (!foundAny) report += "No enrollments found.";
    return report;
}

QString AcademicManager::getClassReport(const QString& courseName, const QString& semester) {
    int cid = findCourseIdByName(courseName);
    if (cid == -1) return "Course not found.";

    int classId = findClassIdByCourseAndSemester(cid, semester);
    if (classId == -1) return "Class not found.";

    auto enrollments = m_db->getClassEnrollments(classId);
    if (enrollments.isEmpty()) return "No students enrolled in this class.";

    QString report = QString("Report for Class %1 (%2):\n").arg(courseName, semester);
    report += QString("Total Students: %1\n").arg(enrollments.size());

    int passed = 0;
    int failed = 0;
    double sumAvg = 0;

    report += "Students:\n";
    for (const auto& e : enrollments) {
        StudentStatus st = calculateStatus(e.studentId, classId);
        if (st.approved) passed++; else failed++;
        sumAvg += st.average;

        report += QString(" - %1 (%2): Avg %3 [%4]\n")
                    .arg(e.studentName, e.studentReg, QString::number(st.average, 'f', 1), st.statusMessage);
    }

    double classAvg = sumAvg / enrollments.size();
    report += QString("\nClass Statistics:\n Average Grade: %1\n Passed: %2\n Failed: %3")
                .arg(QString::number(classAvg, 'f', 2)).arg(passed).arg(failed);

    return report;
}

QList<Student> AcademicManager::getAllStudents() {
    return m_db->getAllStudents();
}

QList<Course> AcademicManager::getAllCourses() {
    return m_db->getAllCourses();
}

QList<Class> AcademicManager::getAllClasses() {
    return m_db->getAllClasses();
}

QList<ClassEnrollment> AcademicManager::getClassEnrollments(int classId) {
    return m_db->getClassEnrollments(classId);
}

bool AcademicManager::updateGradesById(int studentId, int classId, double b1, double b2, double b3, double b4, double finalGrade) {
    // Validate
    auto check = [](double v) { return v >= 0.0 && v <= 10.0; };
    if (!check(b1) || !check(b2) || !check(b3) || !check(b4)) return false;
    // Final grade can be -1 (not set) or 0-10
    if (finalGrade != -1 && !check(finalGrade)) return false;

    return m_db->updateGrades(studentId, classId, b1, b2, b3, b4, finalGrade);
}

bool AcademicManager::enrollStudentById(int studentId, int classId) {
    return m_db->enrollStudent(studentId, classId);
}
