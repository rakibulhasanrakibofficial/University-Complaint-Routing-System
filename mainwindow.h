#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

struct Complaint {
    int id;
    QString studentId;
    QString name;
    QString department;
    QString message;
    bool anonymous;
    bool resolved;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // login & panels
    void studentLogin();
    void adminLogin();
    void openStudentPanel();
    void openAdminPanel();

    // student actions
    void submitComplaint();
    void viewMyComplaints();

    // admin actions
    void viewAllComplaints();
    void viewByDepartment();
    void solveComplaint();
    void exportComplaintPDF();
    void showNoticeBoard();

private:
    QList<Complaint> complaints;
    QString currentUserId;      // holds student id when logged in student
    int nextId;

    // persistence
    void loadData();
    void saveData();

    // small helper
    void showInfo(const QString &t);
    QStringList departments() const;
};

#endif // MAINWINDOW_H
