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
            msg = "Needs Final Exam";
        } else {
            approved = false;
            msg = "Failed";
        }

        // Use the simplified rule "Keep average >= 5.0" requested by user implies
        // maybe the passing grade IS 5.0 straight up?
        // "Mantenha média >= 5,0".
        // If the user means passing grade is 5.0:
        if (avg >= 5.0) {
             approved = true;
             msg = "Approved";
        }
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
    if (m_db->addStudent(name, registration)) {
        return "Student added successfully: " + name;
    }
    return "Failed to add student (might already exist).";
}

QString AcademicManager::createCourse(const QString& name, const QString& description) {
    if (m_db->addCourse(name, description)) {
        return "Course created: " + name;
    }
    return "Failed to create course.";
}

QString AcademicManager::openClass(const QString& courseName, const QString& semester) {
    int cid = findCourseIdByName(courseName);
    if (cid == -1) return "Course not found: " + courseName;

    if (m_db->addClass(cid, semester)) {
        return "Class opened for " + courseName + " in " + semester;
    }
    return "Failed to open class.";
}

QString AcademicManager::registerGrades(const QString& studentReg, const QString& courseName, double b1, double b2, double b3, double b4) {
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

    // Mock report
    // In real app, we would join tables.
    return "Report for student " + studentReg + ": [Not fully implemented in mock view]";
}
