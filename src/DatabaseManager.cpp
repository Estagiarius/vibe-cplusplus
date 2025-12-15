#include "DatabaseManager.h"
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

DatabaseManager::DatabaseManager(const QString& dbName, QObject *parent)
    : QObject(parent) {

    // For testing/mocking purposes, we might want to check if we are running in a specific environment
    // But for now, let's just use the local path or a memory db for tests if specified.

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbName);
}

DatabaseManager::~DatabaseManager() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::init() {
    if (!m_db.open()) {
        qCritical() << "Error opening database:" << m_db.lastError().text();
        return false;
    }
    return createTables();
}

bool DatabaseManager::createTables() {
    QSqlQuery query;

    // Students
    if (!query.exec("CREATE TABLE IF NOT EXISTS students ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT NOT NULL, "
                    "registration TEXT UNIQUE NOT NULL)")) {
        qCritical() << "Error creating students table:" << query.lastError().text();
        return false;
    }

    // Courses
    if (!query.exec("CREATE TABLE IF NOT EXISTS courses ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT NOT NULL, "
                    "description TEXT)")) {
        qCritical() << "Error creating courses table:" << query.lastError().text();
        return false;
    }

    // Classes (Turmas)
    if (!query.exec("CREATE TABLE IF NOT EXISTS classes ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "course_id INTEGER, "
                    "semester TEXT NOT NULL, "
                    "FOREIGN KEY(course_id) REFERENCES courses(id))")) {
        qCritical() << "Error creating classes table:" << query.lastError().text();
        return false;
    }

    // Grades/Enrollments
    // We treat enrollment + grades in one table for simplicity, or we can separate them.
    // Let's keep it simple: grades table implies enrollment.
    // b1-b4 default to 0 or null. Let's use 0.0 for simplicity, or handle nulls if we want to distinguish "not graded yet".
    // For this mock, -1 will indicate "not graded".
    if (!query.exec("CREATE TABLE IF NOT EXISTS grades ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "student_id INTEGER, "
                    "class_id INTEGER, "
                    "b1 REAL DEFAULT -1, "
                    "b2 REAL DEFAULT -1, "
                    "b3 REAL DEFAULT -1, "
                    "b4 REAL DEFAULT -1, "
                    "final_grade REAL DEFAULT -1, "
                    "FOREIGN KEY(student_id) REFERENCES students(id), "
                    "FOREIGN KEY(class_id) REFERENCES classes(id), "
                    "UNIQUE(student_id, class_id))")) {
        qCritical() << "Error creating grades table:" << query.lastError().text();
        return false;
    }

    // Attendance Table
    if (!query.exec("CREATE TABLE IF NOT EXISTS attendance ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "student_id INTEGER, "
                    "class_id INTEGER, "
                    "date TEXT NOT NULL, " // YYYY-MM-DD
                    "present INTEGER NOT NULL, " // 1 for present, 0 for absent
                    "FOREIGN KEY(student_id) REFERENCES students(id), "
                    "FOREIGN KEY(class_id) REFERENCES classes(id), "
                    "UNIQUE(student_id, class_id, date))")) {
        qCritical() << "Error creating attendance table:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::addStudent(const QString& name, const QString& registration) {
    QSqlQuery query;
    query.prepare("INSERT INTO students (name, registration) VALUES (:name, :reg)");
    query.bindValue(":name", name);
    query.bindValue(":reg", registration);
    if (!query.exec()) {
        qWarning() << "Add student failed:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<Student> DatabaseManager::getAllStudents() {
    QList<Student> list;
    QSqlQuery query("SELECT id, name, registration FROM students");
    while (query.next()) {
        list.append({
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString()
        });
    }
    return list;
}

std::optional<Student> DatabaseManager::getStudentByRegistration(const QString& reg) {
    QSqlQuery query;
    query.prepare("SELECT id, name, registration FROM students WHERE registration = :reg");
    query.bindValue(":reg", reg);
    if (query.exec() && query.next()) {
        return Student{
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString()
        };
    }
    return std::nullopt;
}

bool DatabaseManager::addOrUpdateAttendance(int studentId, int classId, const QString& date, bool present) {
    QSqlQuery query;
    // Use INSERT OR REPLACE (UPSERT) to simplify logic
    query.prepare("INSERT OR REPLACE INTO attendance (student_id, class_id, date, present) "
                  "VALUES (:sid, :cid, :date, :present)");
    query.bindValue(":sid", studentId);
    query.bindValue(":cid", classId);
    query.bindValue(":date", date);
    query.bindValue(":present", present ? 1 : 0);

    if (!query.exec()) {
        qWarning() << "Error updating attendance:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<AttendanceRecord> DatabaseManager::getAttendance(int classId, const QString& date) {
    QList<AttendanceRecord> list;
    QSqlQuery query;
    query.prepare("SELECT student_id, present FROM attendance WHERE class_id = :cid AND date = :date");
    query.bindValue(":cid", classId);
    query.bindValue(":date", date);

    if (query.exec()) {
        while (query.next()) {
            list.append({
                query.value(0).toInt(),
                date,
                query.value(1).toBool()
            });
        }
    } else {
        qWarning() << "Error fetching attendance:" << query.lastError().text();
    }
    return list;
}

QList<ClassEnrollment> DatabaseManager::getClassEnrollments(int classId) {
    QList<ClassEnrollment> list;
    QSqlQuery query;
    query.prepare("SELECT g.id, g.student_id, s.name, s.registration, g.class_id, "
                  "g.b1, g.b2, g.b3, g.b4, g.final_grade "
                  "FROM grades g "
                  "JOIN students s ON g.student_id = s.id "
                  "WHERE g.class_id = :cid");
    query.bindValue(":cid", classId);

    if (query.exec()) {
        while (query.next()) {
            ClassEnrollment ce;
            ce.grade.id = query.value(0).toInt();
            ce.studentId = query.value(1).toInt(); // g.student_id
            ce.studentName = query.value(2).toString();
            ce.studentReg = query.value(3).toString();
            ce.grade.studentId = ce.studentId;
            ce.grade.classId = query.value(4).toInt();
            ce.grade.b1 = query.value(5).toDouble();
            ce.grade.b2 = query.value(6).toDouble();
            ce.grade.b3 = query.value(7).toDouble();
            ce.grade.b4 = query.value(8).toDouble();
            ce.grade.finalGrade = query.value(9).toDouble();
            ce.grade.hasFinal = (ce.grade.finalGrade >= 0);

            list.append(ce);
        }
    } else {
        qWarning() << "Error fetching class enrollments:" << query.lastError().text();
    }
    return list;
}

bool DatabaseManager::addCourse(const QString& name, const QString& description) {
    QSqlQuery query;
    query.prepare("INSERT INTO courses (name, description) VALUES (:name, :desc)");
    query.bindValue(":name", name);
    query.bindValue(":desc", description);
    return query.exec();
}

QList<Course> DatabaseManager::getAllCourses() {
    QList<Course> list;
    QSqlQuery query("SELECT id, name, description FROM courses");
    while (query.next()) {
        list.append({
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString()
        });
    }
    return list;
}

bool DatabaseManager::addClass(int courseId, const QString& semester) {
    QSqlQuery query;
    query.prepare("INSERT INTO classes (course_id, semester) VALUES (:cid, :sem)");
    query.bindValue(":cid", courseId);
    query.bindValue(":sem", semester);
    return query.exec();
}

QList<Class> DatabaseManager::getAllClasses() {
    QList<Class> list;
    QSqlQuery query("SELECT id, course_id, semester FROM classes");
    while (query.next()) {
        list.append({
            query.value(0).toInt(),
            query.value(1).toInt(),
            query.value(2).toString()
        });
    }
    return list;
}

bool DatabaseManager::enrollStudent(int studentId, int classId) {
    QSqlQuery query;
    query.prepare("INSERT INTO grades (student_id, class_id) VALUES (:sid, :cid)");
    query.bindValue(":sid", studentId);
    query.bindValue(":cid", classId);
    return query.exec();
}

bool DatabaseManager::updateGrades(int studentId, int classId, double b1, double b2, double b3, double b4, double finalGrade) {
    QSqlQuery query;
    // We update only if the record exists (enrollment).
    query.prepare("UPDATE grades SET b1=:b1, b2=:b2, b3=:b3, b4=:b4, final_grade=:fg "
                  "WHERE student_id=:sid AND class_id=:cid");
    query.bindValue(":b1", b1);
    query.bindValue(":b2", b2);
    query.bindValue(":b3", b3);
    query.bindValue(":b4", b4);
    query.bindValue(":fg", finalGrade);
    query.bindValue(":sid", studentId);
    query.bindValue(":cid", classId);
    return query.exec();
}

std::optional<Grade> DatabaseManager::getStudentGrade(int studentId, int classId) {
    QSqlQuery query;
    query.prepare("SELECT id, student_id, class_id, b1, b2, b3, b4, final_grade FROM grades "
                  "WHERE student_id=:sid AND class_id=:cid");
    query.bindValue(":sid", studentId);
    query.bindValue(":cid", classId);

    if (query.exec() && query.next()) {
        Grade g;
        g.id = query.value(0).toInt();
        g.studentId = query.value(1).toInt();
        g.classId = query.value(2).toInt();
        g.b1 = query.value(3).toDouble();
        g.b2 = query.value(4).toDouble();
        g.b3 = query.value(5).toDouble();
        g.b4 = query.value(6).toDouble();
        g.finalGrade = query.value(7).toDouble();

        // If finalGrade is -1, it means no final exam taken/recorded
        g.hasFinal = (g.finalGrade >= 0);

        return g;
    }
    return std::nullopt;
}
