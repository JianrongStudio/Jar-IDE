#include "welcomewindow.h"
#include "appstyle.h"
#include "pythonchecker.h"
#include "jroproject.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QDir>
#include <QIcon>

WelcomeWindow::WelcomeWindow(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("welcomeRoot");
    setMinimumSize(860, 560);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(60, 50, 60, 40);
    root->setSpacing(18);

    auto *title = new QLabel("Jar-IDE", this);
    title->setObjectName("brandTitle");
    auto *sub = new QLabel(tr("A lightweight animated Python editor  ·  Win7 ~ Win11"), this);
    sub->setObjectName("brandSub");
    root->addWidget(title);
    root->addWidget(sub);
    root->addSpacing(30);

    auto *cards = new QHBoxLayout;
    cards->setSpacing(22);
    cards->addWidget(makeCard("open-folder.svg", tr("Open Folder"),
                              tr("Open an existing folder as a project"), SLOT(chooseFolder())));
    cards->addWidget(makeCard("open-file.svg", tr("Open File"),
                              tr("Open a single Python file"), SLOT(chooseFile())));
    cards->addWidget(makeCard("new-project.svg", tr("New Jianrong Project"),
                              tr("Create a project.jro + src/ scaffold"), SLOT(newProject())));
    root->addLayout(cards);
    root->addStretch();

    m_status = new QLabel(this);
    m_status->setObjectName("brandSub");
    if (PythonChecker::isAvailable())
        m_status->setText(tr("● Python environment detected: %1").arg(PythonChecker::findPython()));
    else
        m_status->setText(tr("● Python not detected — it will be downloaded on demand"));
    root->addWidget(m_status);
}

QWidget *WelcomeWindow::makeCard(const QString &icon, const QString &title,
                                 const QString &desc, const char *slot)
{
    auto *card = new QFrame;
    card->setObjectName("card");
    card->setMinimumSize(220, 200);
    card->setCursor(Qt::PointingHandCursor);

    auto *v = new QVBoxLayout(card);
    v->setContentsMargins(22, 24, 22, 20);
    v->setSpacing(10);

    auto *ic = new QLabel(card);
    ic->setPixmap(QIcon(AppStyle::iconPath(icon)).pixmap(46, 46));
    v->addWidget(ic);

    auto *t = new QLabel(title, card);
    t->setObjectName("cardTitle");
    v->addWidget(t);

    auto *d = new QLabel(desc, card);
    d->setObjectName("cardDesc");
    d->setWordWrap(true);
    v->addWidget(d);
    v->addStretch();

    card->installEventFilter(this);
    card->setProperty("_slot", QString(slot));

    auto *btn = new QPushButton(tr("Open"), card);
    v->addWidget(btn);
    connect(btn, SIGNAL(clicked()), this, slot);
    return card;
}

bool WelcomeWindow::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::MouseButtonRelease) {
        QString slot = o->property("_slot").toString();
        if (slot == "chooseFolder") { chooseFolder(); return true; }
        if (slot == "chooseFile")   { chooseFile();   return true; }
        if (slot == "newProject")   { newProject();   return true; }
    }
    return QWidget::eventFilter(o, e);
}

void WelcomeWindow::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    auto *eff = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(eff);
    auto *anim = new QPropertyAnimation(eff, "opacity", this);
    anim->setDuration(550);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void WelcomeWindow::chooseFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Project Folder"));
    if (!dir.isEmpty())
        emit openFolder(dir);
}

void WelcomeWindow::chooseFile()
{
    QString f = QFileDialog::getOpenFileName(this, tr("Open Python File"),
                                             QString(), tr("Python (*.py *.jro);;All (*.*)"));
    if (!f.isEmpty())
        emit openFile(f);
}

void WelcomeWindow::newProject()
{
    QString parentDir = QFileDialog::getExistingDirectory(
        this, tr("Choose parent directory for the new project"));
    if (parentDir.isEmpty())
        return;

    bool ok = false;
    QString name = QInputDialog::getText(
        this, tr("New Jianrong Project"),
        tr("Project name:"), QLineEdit::Normal, "MyProject", &ok).trimmed();
    if (!ok || name.isEmpty())
        return;

    QString root = JroProject::createOnDisk(parentDir, name);
    if (root.isEmpty()) {
        QMessageBox::warning(this, tr("Jar-IDE"), tr("Could not create the project folder."));
        return;
    }
    QMessageBox::information(this, tr("Jar-IDE"),
        tr("Project created at:\n%1").arg(root));
    emit openProject(root);
}
