#include <QtTest>
#include "DatabaseManager.h"
#include "AcademicManager.h"
#include <memory>

class TestLogic : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testStudentCRUD();
    void testCourseAndClass();
    void testGradesApproval();
    void testGradesFailure();
    void testFinalMention();

private:
    std::shared_ptr<DatabaseManager> db;
    std::shared_ptr<AcademicManager> manager;
};

void TestLogic::initTestCase() {
    // Use an in-memory database for testing or a temp file
    db = std::make_shared<DatabaseManager>(":memory:");
    QVERIFY(db->init());
    manager = std::make_shared<AcademicManager>(db);
}

void TestLogic::cleanupTestCase() {
    // Shared ptr will clean up
}

void TestLogic::testStudentCRUD() {
    QString res = manager->addStudent("John Doe", "12345");
    QVERIFY(res.contains("successfully"));

    auto s = db->getStudentByRegistration("12345");
    QVERIFY(s.has_value());
    QCOMPARE(s->name, "John Doe");
}

void TestLogic::testCourseAndClass() {
    QString res = manager->createCourse("Math", "Basic Math");
    QVERIFY(res.contains("created"));

    QString resClass = manager->openClass("Math", "2023.1");
    QVERIFY(resClass.contains("Class opened"));
}

void TestLogic::testGradesApproval() {
    // Prerequisite: Student and Class exist from previous tests
    // John Doe (12345), Math, 2023.1

    // Register grades: 7.5, 7.5, 7.5, 7.5 -> Avg 7.5 -> Approved
    // (Updated to >= 7.0 rule)
    QString res = manager->registerGrades("12345", "Math", 7.5, 7.5, 7.5, 7.5);

    QVERIFY(res.contains("Approved"));

    // Verify stored
    auto s = db->getStudentByRegistration("12345");
    auto classes = db->getAllClasses(); // Assuming only one class for Math so far
    int classId = classes.last().id;

    auto grade = db->getStudentGrade(s->id, classId);
    QVERIFY(grade.has_value());
    QCOMPARE(grade->b1, 7.5);
}

void TestLogic::testGradesFailure() {
    manager->addStudent("Jane Doe", "54321");
    // Reuse Math course

    // 2, 2, 2, 2 -> Avg 2 -> Failed
    QString res = manager->registerGrades("54321", "Math", 2.0, 2.0, 2.0, 2.0);
    QVERIFY(res.contains("Failed"));
}

void TestLogic::testFinalMention() {
    // Test logic where student goes to final
    manager->addStudent("Bob Smith", "67890");

    // 4, 4, 4, 4 -> Avg 4 -> Needs Final
    QString res = manager->registerGrades("67890", "Math", 4.0, 4.0, 4.0, 4.0);
    QVERIFY(res.contains("Needs Final Exam"));

    // Now we need a way to add final grade.
    // The current facade 'registerGrades' only takes 4 bimesters.
    // I need to update AcademicManager to support final grade or update DatabaseManager directly for test.
    // Or update registerGrades to optional 5th arg.

    // Let's manually update DB for this test to check calculation logic
    auto s = db->getStudentByRegistration("67890");
    auto classes = db->getAllClasses();
    int classId = classes.last().id;

    // Add final grade of 8. (Avg 4 + 8) / 2 = 6 -> Approved
    db->updateGrades(s->id, classId, 4.0, 4.0, 4.0, 4.0, 8.0);

    StudentStatus status = manager->calculateStatus(s->id, classId);
    QVERIFY(status.approved);
    QVERIFY(status.statusMessage.contains("Approved with Final Exam"));
}

QTEST_MAIN(TestLogic)
#include "test_logic.moc"
