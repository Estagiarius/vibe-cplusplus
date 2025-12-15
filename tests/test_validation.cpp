#include <QtTest>
#include "DatabaseManager.h"
#include "AcademicManager.h"
#include <memory>

class TestLogicValidation : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testInvalidGrades();
    void testRecoveryLogic();
    void testInputSanitization();

private:
    std::shared_ptr<DatabaseManager> db;
    std::shared_ptr<AcademicManager> manager;
};

void TestLogicValidation::initTestCase() {
    db = std::make_shared<DatabaseManager>(":memory:");
    QVERIFY(db->init());
    manager = std::make_shared<AcademicManager>(db);

    // Setup basic data
    manager->createCourse("Math", "Mathematics");
    manager->openClass("Math", "2024.1");
    manager->addStudent("Val Test", "VAL001");
}

void TestLogicValidation::cleanupTestCase() {}

void TestLogicValidation::testInvalidGrades() {
    // Attempt to register 11.0
    QString res = manager->registerGrades("VAL001", "Math", 11.0, 5.0, 5.0, 5.0);
    QVERIFY(res.contains("Error"));
    QVERIFY(res.contains("between 0.0 and 10.0"));

    // Attempt negative
    res = manager->registerGrades("VAL001", "Math", -1.0, 5.0, 5.0, 5.0);
    QVERIFY(res.contains("Error"));
}

void TestLogicValidation::testRecoveryLogic() {
    // Add student specifically for this test
    manager->addStudent("Rec Test", "REC001");

    // Register grades avg = 4.0 (Needs Recovery)
    manager->registerGrades("REC001", "Math", 4.0, 4.0, 4.0, 4.0);

    QString report = manager->getStudentReport("REC001");
    QVERIFY(report.contains("Recuperação") || report.contains("Needs Final Exam"));

    // Now apply Final Exam grade of 6.0
    // Avg 4.0 + Final 6.0 = 10.0 / 2 = 5.0 -> Pass

    // We need to use updateGradesById or registerGrades doesn't support final param yet exposed to string facade?
    // Facade `registerGrades` is 4 params. We need `updateGradesById` or update facade.
    // Let's use internal ID method or assume `update_grades` tool usage pattern (which is strict 4 args currently in JSON?).
    // Actually `AcademicManager` has `updateGradesById` which takes final.
    // We need to resolve ID first.

    // For test simplicity, let's use the DB/Internal method exposed to test logic (manager friend or public).
    // `updateGradesById` is public.

    int sid = -1;
    auto students = manager->getAllStudents();
    for(auto s : students) if(s.registration == "REC001") sid = s.id;

    auto classes = manager->getAllClasses(); // Math 2024.1 is likely only one
    int cid = classes[0].id;

    // Apply Final 6.0
    manager->updateGradesById(sid, cid, 4.0, 4.0, 4.0, 4.0, 6.0);

    // Check status
    StudentStatus st = manager->calculateStatus(sid, cid);
    QVERIFY(st.approved);
    QVERIFY(st.statusMessage.contains("Approved with Final Exam"));
    QCOMPARE(st.average, 5.0);
}

void TestLogicValidation::testInputSanitization() {
    // Empty names
    QString res = manager->addStudent("", "");
    QVERIFY(res.contains("Error"));

    res = manager->createCourse("   ", "Desc");
    QVERIFY(res.contains("Error"));
}

QTEST_MAIN(TestLogicValidation)
#include "test_validation.moc"
