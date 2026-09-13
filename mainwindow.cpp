#include "mainwindow.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPrinter>
#include <QTextDocument>
#include <QFileDialog>
#include <QListWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), nextId(1)
{
    setWindowTitle("Complaint Routing System");
    resize(880, 640);

    loadData();

    // Main choice page: Student or Admin
    QWidget *c = new QWidget(this);
    setCentralWidget(c);
    QVBoxLayout *mainLayout = new QVBoxLayout(c);

    QLabel *title = new QLabel("📋 Complaint Routing System");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:28px; font-weight:bold; color:#1d66c1;");
    mainLayout->addWidget(title);

    // Buttons
    QPushButton *btnStudent = new QPushButton("👨‍🎓 Student Login");
    QPushButton *btnAdmin = new QPushButton("🏛 Admin Login");

    QString btnStyle = "background:#1d66c1; color:white; padding:12px; border-radius:10px; font-size:16px;";
    btnStudent->setStyleSheet(btnStyle);
    btnAdmin->setStyleSheet(btnStyle);

    mainLayout->addSpacing(20);
    mainLayout->addWidget(btnStudent);
    mainLayout->addWidget(btnAdmin);
    mainLayout->addStretch();

    connect(btnStudent, &QPushButton::clicked, this, &MainWindow::studentLogin);
    connect(btnAdmin, &QPushButton::clicked, this, &MainWindow::adminLogin);
}

MainWindow::~MainWindow()
{
    saveData();
}

// ---------- persistence ----------
void MainWindow::loadData()
{
    QFile f("complaints.json");
    if (!f.exists()) return;
    if (!f.open(QIODevice::ReadOnly)) return;

    QByteArray data = f.readAll();
    f.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) return;
    QJsonArray arr = doc.array();

    complaints.clear();
    for (auto v : arr) {
        QJsonObject o = v.toObject();
        Complaint c;
        c.id = o["id"].toInt();
        c.studentId = o["sid"].toString();
        c.name = o["name"].toString();
        c.department = o["dept"].toString();
        c.message = o["msg"].toString();
        c.anonymous = o["anon"].toBool();
        c.resolved = o["res"].toBool();
        complaints.append(c);
        if (c.id >= nextId) nextId = c.id + 1;
    }
}

void MainWindow::saveData()
{
    QJsonArray arr;
    for (const Complaint &c : complaints) {
        QJsonObject o;
        o["id"] = c.id;
        o["sid"] = c.studentId;
        o["name"] = c.name;
        o["dept"] = c.department;
        o["msg"] = c.message;
        o["anon"] = c.anonymous;
        o["res"] = c.resolved;
        arr.append(o);
    }
    QJsonDocument doc(arr);
    QFile f("complaints.json");
    if (!f.open(QIODevice::WriteOnly)) return;
    f.write(doc.toJson());
    f.close();
}

// ---------- helpers ----------
void MainWindow::showInfo(const QString &t)
{
    QMessageBox::information(this, "Info", t);
}

QStringList MainWindow::departments() const
{
    return {"CSE", "EEE", "CCE", "BBA", "Others"};
}

// ---------- login & panels ----------
void MainWindow::studentLogin()
{
    bool ok;
    QString sid = QInputDialog::getText(this, "Student Login",
                                        "Enter Student ID:", QLineEdit::Normal,
                                        QString(), &ok);
    if (!ok || sid.trimmed().isEmpty()) {
        showInfo("Login cancelled or empty ID.");
        return;
    }
    currentUserId = sid.trimmed();
    openStudentPanel();
}

void MainWindow::adminLogin()
{
    bool ok;
    QString pass = QInputDialog::getText(this, "Admin Login",
                                         "Enter Admin Password:", QLineEdit::Password,
                                         QString(), &ok);
    if (!ok) {
        showInfo("Login cancelled.");
        return;
    }
    if (pass != "ugrad.iiuc.bd") {
        QMessageBox::warning(this, "Denied", "Wrong password!");
        return;
    }
    openAdminPanel();
}

void MainWindow::openStudentPanel()
{
    QWidget *w = new QWidget;
    w->setWindowTitle("Student Panel - " + currentUserId);
    QVBoxLayout *v = new QVBoxLayout(w);

    QLabel *lbl = new QLabel("👨‍🎓 Student Panel");
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setStyleSheet("font-size:20px; font-weight:bold;");
    v->addWidget(lbl);

    QPushButton *btnSubmit = new QPushButton("➕ Submit Complaint");
    QPushButton *btnMy = new QPushButton("📌 My Complaints");

    QString style = "background:#1d66c1; color:white; padding:10px; border-radius:8px; font-size:14px;";
    btnSubmit->setStyleSheet(style);
    btnMy->setStyleSheet(style);

    v->addWidget(btnSubmit);
    v->addWidget(btnMy);
    v->addStretch();

    connect(btnSubmit, &QPushButton::clicked, this, &MainWindow::submitComplaint);
    connect(btnMy, &QPushButton::clicked, this, &MainWindow::viewMyComplaints);

    w->resize(600, 480);
    w->show();
}

void MainWindow::openAdminPanel()
{
    QWidget *w = new QWidget;
    w->setWindowTitle("Admin Panel");
    QVBoxLayout *v = new QVBoxLayout(w);

    QLabel *lbl = new QLabel("🏛 Admin Panel");
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setStyleSheet("font-size:20px; font-weight:bold;");
    v->addWidget(lbl);

    QPushButton *btnAll = new QPushButton("📄 View All Complaints");
    QPushButton *btnDept = new QPushButton("🏛 View by Department");
    QPushButton *btnSolve = new QPushButton("✅ Solve Complaint");
    QPushButton *btnPDF = new QPushButton("📄 Export Complaint PDF");
    QPushButton *btnNotice = new QPushButton("📢 Notice Board");

    QString style = "background:#1d66c1; color:white; padding:10px; border-radius:8px; font-size:14px;";
    btnAll->setStyleSheet(style);
    btnDept->setStyleSheet(style);
    btnSolve->setStyleSheet(style);
    btnPDF->setStyleSheet(style);
    btnNotice->setStyleSheet(style);

    v->addWidget(btnAll);
    v->addWidget(btnDept);
    v->addWidget(btnSolve);
    v->addWidget(btnPDF);
    v->addWidget(btnNotice);
    v->addStretch();

    connect(btnAll, &QPushButton::clicked, this, &MainWindow::viewAllComplaints);
    connect(btnDept, &QPushButton::clicked, this, &MainWindow::viewByDepartment);
    connect(btnSolve, &QPushButton::clicked, this, &MainWindow::solveComplaint);
    connect(btnPDF, &QPushButton::clicked, this, &MainWindow::exportComplaintPDF);
    connect(btnNotice, &QPushButton::clicked, this, &MainWindow::showNoticeBoard);

    w->resize(700, 520);
    w->show();
}

// ---------- student actions ----------
void MainWindow::submitComplaint()
{
    QWidget *f = new QWidget;
    f->setWindowTitle("Submit Complaint");
    QVBoxLayout *v = new QVBoxLayout(f);

    QLineEdit *leName = new QLineEdit;
    leName->setPlaceholderText("Your Name (optional if anonymous)");

    QComboBox *cbDept = new QComboBox;
    cbDept->addItems(departments());

    QComboBox *cbAnon = new QComboBox;
    cbAnon->addItems({"No", "Yes"});

    QTextEdit *teMsg = new QTextEdit;
    teMsg->setPlaceholderText("Write your complaint here...");

    v->addWidget(new QLabel("Name:"));
    v->addWidget(leName);
    v->addWidget(new QLabel("Department:"));
    v->addWidget(cbDept);
    v->addWidget(new QLabel("Anonymous?"));
    v->addWidget(cbAnon);
    v->addWidget(new QLabel("Message:"));
    v->addWidget(teMsg);

    QPushButton *btn = new QPushButton("Submit");
    btn->setStyleSheet("background:green; color:white; padding:8px; border-radius:6px;");
    v->addWidget(btn);

    connect(btn, &QPushButton::clicked, f, [=, this]() {
        QString msg = teMsg->toPlainText().trimmed();
        if (msg.isEmpty()) {
            QMessageBox::warning(f, "Error", "Message cannot be empty.");
            return;
        }
        Complaint c;
        c.id = nextId++;
        c.studentId = currentUserId;
        c.name = (cbAnon->currentText() == "Yes") ? QString("Anonymous") : leName->text().trimmed();
        c.department = cbDept->currentText();
        c.message = msg;
        c.anonymous = (cbAnon->currentText() == "Yes");
        c.resolved = false;

        complaints.append(c);
        saveData();
        QMessageBox::information(f, "Saved", "Complaint submitted successfully!");
        f->close();
    });

    f->resize(520, 520);
    f->show();
}

void MainWindow::viewMyComplaints()
{
    QWidget *w = new QWidget;
    w->setWindowTitle("My Complaints - " + currentUserId);
    QVBoxLayout *v = new QVBoxLayout(w);

    QListWidget *list = new QListWidget;
    for (const Complaint &c : complaints) {
        if (c.studentId == currentUserId) {
            QString s = QString("ID: %1 | Dept: %2 | %3\n%4")
            .arg(c.id)
                .arg(c.department)
                .arg(c.resolved ? "✅ Resolved" : "⏳ Pending")
                .arg(c.message);
            list->addItem(s);
        }
    }
    if (list->count() == 0) list->addItem("No complaints found.");

    v->addWidget(list);
    w->resize(600, 480);
    w->show();
}

// ---------- admin actions ----------
void MainWindow::viewAllComplaints()
{
    QWidget *w = new QWidget;
    w->setWindowTitle("All Complaints");
    QVBoxLayout *v = new QVBoxLayout(w);

    QListWidget *list = new QListWidget;
    for (const Complaint &c : complaints) {
        QString name_display = c.anonymous ? "Anonymous" : c.name;
        QString s = QString("ID: %1 | SID: %2 | Dept: %3 | %4\nName: %5\n%6")
                        .arg(c.id).arg(c.studentId).arg(c.department)
                        .arg(c.resolved ? "✅ Resolved" : "⏳ Pending")
                        .arg(name_display)
                        .arg(c.message);
        list->addItem(s);
    }
    if (list->count() == 0) list->addItem("No complaints yet.");

    v->addWidget(list);
    w->resize(800, 600);
    w->show();
}

void MainWindow::viewByDepartment()
{
    bool ok;
    QString dept = QInputDialog::getItem(this, "Department", "Choose department:", departments(), 0, false, &ok);
    if (!ok) return;

    QWidget *w = new QWidget;
    w->setWindowTitle(dept + " Complaints");
    QVBoxLayout *v = new QVBoxLayout(w);
    QListWidget *list = new QListWidget;

    for (const Complaint &c : complaints) {
        if (c.department == dept) {
            QString s = QString("ID: %1 | %2\n%3")
            .arg(c.id)
                .arg(c.resolved ? "Resolved" : "Pending")
                .arg(c.message);
            list->addItem(s);
        }
    }
    if (list->count() == 0) list->addItem("No complaints in this department.");

    v->addWidget(list);
    w->resize(700, 520);
    w->show();
}

void MainWindow::solveComplaint()
{
    bool ok;
    int id = QInputDialog::getInt(this, "Solve complaint", "Enter complaint ID:", 1, 1, 999999, 1, &ok);
    if (!ok) return;

    for (Complaint &c : complaints) {
        if (c.id == id) {
            c.resolved = true;
            saveData();
            showInfo("Complaint marked as resolved.");
            return;
        }
    }
    QMessageBox::warning(this, "Not found", "Complaint ID not found.");
}

void MainWindow::exportComplaintPDF()
{
    bool ok;
    int id = QInputDialog::getInt(this, "Export PDF", "Enter complaint ID:", 1, 1, 999999, 1, &ok);
    if (!ok) return;

    for (const Complaint &c : complaints) {
        if (c.id == id) {
            QString html;
            html += QString("<h2>Complaint ID: %1</h2>").arg(c.id);
            html += QString("<b>Student ID:</b> %1<br>").arg(c.studentId);
            html += QString("<b>Department:</b> %1<br>").arg(c.department);
            html += QString("<b>Name (as shown):</b> %1<br>").arg(c.anonymous ? "Anonymous" : c.name);
            html += QString("<b>Status:</b> %1<br><br>").arg(c.resolved ? "Resolved" : "Pending");
            html += QString("<b>Message:</b><br><pre>%1</pre>").arg(c.message.toHtmlEscaped());

            QString filePath = QFileDialog::getSaveFileName(this, "Save PDF", QString("complaint_%1.pdf").arg(c.id), "PDF files (*.pdf)");
            if (filePath.isEmpty()) return;

            QPrinter printer(QPrinter::HighResolution);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(filePath);

            QTextDocument doc;
            doc.setHtml(html);
            doc.print(&printer);

            showInfo("PDF exported successfully.");
            return;
        }
    }
    QMessageBox::warning(this, "Not found", "Complaint ID not found.");
}

void MainWindow::showNoticeBoard()
{
    QWidget *w = new QWidget;
    w->setWindowTitle("Notice Board - Summary");
    QVBoxLayout *v = new QVBoxLayout(w);

    QLabel *t = new QLabel("<h2>📢 Notice Board</h2>");
    t->setAlignment(Qt::AlignCenter);
    v->addWidget(t);

    QString html;
    for (const QString &dept : departments()) {
        int pending = 0, solved = 0;
        for (const Complaint &c : complaints) {
            if (c.department == dept) {
                if (c.resolved) solved++;
                else pending++;
            }
        }
        html += QString("<b>%1 Department</b><br>Pending: %2<br>Resolved: %3<br><br>")
                    .arg(dept).arg(pending).arg(solved);
    }

    QLabel *info = new QLabel(html);
    info->setTextFormat(Qt::RichText);
    v->addWidget(info);

    w->resize(400, 420);
    w->show();
}
