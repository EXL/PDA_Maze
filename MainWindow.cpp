#include "MainWindow.h"

#include <QCloseEvent>
#include <QMenuBar>
#include <QApplication>
#include <QDir>

#ifdef _DEBUG
#include <QDebug>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_ini_PDA_Maze = new IniConfig();
    if (!(m_ini_PDA_Maze->readIniConfig())) {
#ifdef _DEBUG
        qDebug() << "Error reading config!";
#endif
    }

    m_scale = m_ini_PDA_Maze->getV_cfg_scale_screen();

    /* Install translations for App */
    qApp->installTranslator(&appTranslator);

    m_playField = new PlayField(this);
    m_playField->updateTimerMode(m_ini_PDA_Maze->getV_cfg_timer_mode());
    m_playField->updateMapMode(m_ini_PDA_Maze->getV_cfg_map_mode());
    m_playField->updateSize(m_ini_PDA_Maze->getV_cfg_map_size());
    m_playField->updateStepStatus(m_ini_PDA_Maze->getV_cfg_step_show());
    m_playField->updateScreenScale(m_scale);
    m_playField->updateSmoothStatus(m_ini_PDA_Maze->getV_cfg_screen_smoothing());
    m_playField->updateLang(m_ini_PDA_Maze->getV_cfg_app_language());

    createActions();
    m_actionStep->setChecked(m_ini_PDA_Maze->getV_cfg_step_show());
    if (m_scale) {
        m_actionSmoothScreen->setChecked(m_ini_PDA_Maze->getV_cfg_screen_smoothing());
    } else {
        disableSmoothAction();
    }

    createMenus();

    loadTranslations();

    retranslateUi();

    setCentralWidget(m_playField);
    detFixedSize(m_scale);
    setWindowIcon(QIcon("://icons/PDA_maze_64x64.png"));
}

void MainWindow::createActions()
{
    m_actionNewGame = new QAction(this);
    m_actionNewGame->setShortcut(Qt::Key_F5);
    connect(m_actionNewGame, SIGNAL(triggered()),
            m_playField, SLOT(start()));

    m_actionQuit = new QAction(this);
    m_actionQuit->setShortcut(Qt::Key_F10);
    connect(m_actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    m_actionStep = new QAction(this);
    m_actionStep->setCheckable(true);
    connect(m_actionStep, SIGNAL(triggered(bool)),
            this, SLOT(slotShowStepChange(bool)));

    m_actionSmoothScreen = new QAction(this);
    m_actionSmoothScreen->setCheckable(true);
    connect(m_actionSmoothScreen, SIGNAL(triggered(bool)),
            this, SLOT(slotSmoothScreenChange(bool)));

    m_actionHelp = new QAction(this);
    m_actionHelp->setShortcut(Qt::Key_F1);
    connect(m_actionHelp, SIGNAL(triggered()), m_playField, SLOT(help()));

    m_actionAbout = new QAction(this);
    connect(m_actionAbout, SIGNAL(triggered()), m_playField, SLOT(stop()));

    m_actionAboutQt = new QAction(this);
    connect(m_actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

QMenu *MainWindow::createTimerMenu()
{
    m_menupTimer = new QMenu();

    m_actionGroupTimer = new QActionGroup(this);
    connect(m_actionGroupTimer, SIGNAL(triggered(QAction *)),
            this, SLOT(slotTimerModeChange(QAction *)));

    for(size_t i = 0; i < 2; ++i) {
        m_actionTimer = new QAction(this);
        m_actionTimer->setCheckable(true);
        m_actionTimer->setData((int)i);

        m_menupTimer->addAction(m_actionTimer);
        m_actionGroupTimer->addAction(m_actionTimer);

        if((int)i == m_ini_PDA_Maze->getV_cfg_timer_mode()) {
            m_actionTimer->setChecked(true);
        }
    }

    return m_menupTimer;
}

QMenu *MainWindow::createMapModeMenu()
{
    m_menupMode = new QMenu();

    m_actionGroupMode = new QActionGroup(this);
    connect(m_actionGroupMode, SIGNAL(triggered(QAction *)),
            this, SLOT(slotMapModeChange(QAction *)));

    for(size_t i = 0; i < 3; ++i) {
        m_actionMode = new QAction(this);
        m_actionMode->setCheckable(true);
        m_actionMode->setData((int)i);

        m_menupMode->addAction(m_actionMode);
        m_actionGroupMode->addAction(m_actionMode);

        if((int)i == m_ini_PDA_Maze->getV_cfg_map_mode()) {
            m_actionMode->setChecked(true);
        }
    }

    return m_menupMode;
}

QMenu *MainWindow::createMapSizeMenu()
{
    m_menupSize = new QMenu();

    m_actionGroupSize = new QActionGroup(this);
    connect(m_actionGroupSize, SIGNAL(triggered(QAction *)),
            this, SLOT(slotMapSizeChange(QAction *)));

    for(size_t i = 9; i < 50; i+=10) {
        m_actionSize = new QAction(this);
        m_actionSize->setCheckable(true);
        m_actionSize->setData((int)i);

        m_menupSize->addAction(m_actionSize);
        m_actionGroupSize->addAction(m_actionSize);

        if((int)i == m_ini_PDA_Maze->getV_cfg_map_size()) {
            m_actionSize->setChecked(true);
        }
    }

    return m_menupSize;
}

QMenu *MainWindow::createScreenSizeMenu()
{
    m_menupScreenSize = new QMenu();

    m_actionGroupScreenSize = new QActionGroup(this);
    connect(m_actionGroupScreenSize, SIGNAL(triggered(QAction*)),
            this, SLOT(slotScreenSizeChange(QAction *)));

    for (size_t i = 0; i < 3; ++i) {
        m_actionScreenSize = new QAction(this);
        m_actionScreenSize->setCheckable(true);
        m_actionScreenSize->setData((int)i);

        m_menupScreenSize->addAction(m_actionScreenSize);
        m_actionGroupScreenSize->addAction(m_actionScreenSize);

        if ((int)i == m_ini_PDA_Maze->getV_cfg_scale_screen()) {
            m_actionScreenSize->setChecked(true);
        }
    }

    m_menupScreenSize->addSeparator();
    m_menupScreenSize->addAction(m_actionSmoothScreen);

    return m_menupScreenSize;
}

QMenu *MainWindow::createLanguageMenu()
{
    m_menupLang = new QMenu(this);

    m_actionGroupLang = new QActionGroup(this);
    connect(m_actionGroupLang, SIGNAL(triggered(QAction *)),
            this, SLOT(slotSwitchLanguage(QAction*)));

    QDir qmDir("://i18n/");
    QStringList fileNames = qmDir.entryList(QStringList("PDA_Maze_*.qm"));

    for (int i = 0; i < fileNames.size(); ++i) {
        QString locale = fileNames[i];
        locale.remove(0, locale.indexOf('_') + 1);
        locale.remove("Maze_");
        locale.chop(3);

        QTranslator translator;
        translator.load(fileNames[i], qmDir.absolutePath());

        QString language;
        if (locale == "en") {
            language = "English";
        } else if (locale == "ru") {
            language = "Russian";
        } else if (locale == "es") {
            language = "Spanish";
        }

        m_actionLang = new QAction(QString("&%1").arg(language), this);
        m_actionLang->setCheckable(true);
        m_actionLang->setIcon(QIcon(QString("://icons/") + locale + QString(".png")));
        m_actionLang->setData(locale);

        m_menupLang->addAction(m_actionLang);
        m_actionGroupLang->addAction(m_actionLang);

        if (locale == m_ini_PDA_Maze->getV_cfg_app_language()) {
            m_actionLang->setChecked(true);
        } else if (m_ini_PDA_Maze->getV_cfg_app_language() == ""
                   && locale == "en") {
            m_actionLang->setChecked(true);
        }
    }
    return m_menupLang;
}

void MainWindow::createMenus()
{
    m_menuFile = new QMenu();
    m_menuFile->addAction(m_actionNewGame);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_actionQuit);

    m_menuSettings = new QMenu();
    m_menuSettings->addMenu(createTimerMenu());
    m_menuSettings->addMenu(createMapModeMenu());
    m_menuSettings->addMenu(createMapSizeMenu());
    m_menuSettings->addSeparator();
    m_menuSettings->addAction(m_actionStep);
    m_menuSettings->addSeparator();
    m_menuSettings->addMenu(createScreenSizeMenu());
    m_menuSettings->addSeparator();
    m_menuSettings->addMenu(createLanguageMenu());

    m_menuHelp = new QMenu();
    m_menuHelp->addAction(m_actionHelp);
    m_menuHelp->addSeparator();
    m_menuHelp->addAction(m_actionAbout);
    m_menuHelp->addAction(m_actionAboutQt);

    menuBar()->addMenu(m_menuFile);
    menuBar()->addMenu(m_menuSettings);
    menuBar()->addMenu(m_menuHelp);
}

void MainWindow::disableSmoothAction()
{
    m_actionSmoothScreen->setChecked(false);
    m_actionSmoothScreen->setDisabled(true);
    m_ini_PDA_Maze->setV_cfg_screen_smoothing(false);
#ifdef _DEBUG
    qDebug() << "Smooth menu are disabled. "
                "The cfg_screen_smoothing is: "
             << m_ini_PDA_Maze->getV_cfg_screen_smoothing();
#endif
}

void MainWindow::detFixedSize(int scale)
{

    int m = menuBar()->sizeHint().height();

    switch (scale) {
        case 1:
        {
            m_playField->setFixedSize(240, 265);
            setFixedSize(240, 265 + m);
            break;
        }
        case 2:
        {
            m_playField->setFixedSize(480, 531);
            setFixedSize(480, 531 + m);
            break;
        }
        case 0:
        default:
        {
            m_playField->setFixedSize(160, 177);
            setFixedSize(160, 177 + m); // adjustSize() isn't working correctly
            break;
        }
    }
}

void MainWindow::loadTranslations()
{
    QDir qmDir("://i18n");
    QString qmPath = qmDir.absolutePath();

    appTranslator.load("PDA_Maze_" + m_ini_PDA_Maze->getV_cfg_app_language(), qmPath);
}

void MainWindow::retranslateUi()
{
    m_actionGroupTimer->actions().at(0)->setText(tr("Timer Up"));
    m_actionGroupTimer->actions().at(1)->setText(tr("Timer Down"));

    m_actionGroupMode->actions().at(0)->setText(tr("All"));
    m_actionGroupMode->actions().at(1)->setText(tr("Build"));
    m_actionGroupMode->actions().at(2)->setText(tr("None"));

    for (size_t i = 0; i < 5; ++i) {
        m_actionGroupSize->actions().at(i)->setText(tr("%1x%1").arg(i * 10 + 9));
    }

    m_actionGroupScreenSize->actions().at(0)->setText(tr("160x177"));
    m_actionGroupScreenSize->actions().at(1)->setText(tr("240x265"));
    m_actionGroupScreenSize->actions().at(2)->setText(tr("480x531"));

    m_actionNewGame->setText(tr("&New Game"));
    m_actionQuit->setText(tr("&Quit"));
    m_actionStep->setText(tr("&Step Counter"));
    m_actionSmoothScreen->setText(tr("Smooth"));
    m_actionHelp->setText(tr("&Control Keys"));
    m_actionAbout->setText(tr("&About"));
    m_actionAboutQt->setText(tr("About &Qt..."));
    m_menupTimer->setTitle(tr("&Timer"));
    m_menupMode->setTitle(tr("&Map"));
    m_menupSize->setTitle(tr("Si&ze"));
    m_menupScreenSize->setTitle(tr("S&creen Size"));
    m_menuFile->setTitle(tr("&Game"));

    m_menupLang->setTitle(tr("&Language"));
    m_menuHelp->setTitle(tr("&Help"));

    switch (m_scale) {
        case 1:
        case 2:
        {
            m_menuSettings->setTitle(tr("&Settings"));
            setWindowTitle(tr("PDA Maze"));
            break;
        }
        case 0:
        default:
        {
            m_menuSettings->setTitle(tr("&Settings-short.")); // Russian menu length fix on 160x200
            setWindowTitle(tr("Maze"));
        }
    }

#ifdef _DEBUG
    qDebug() << "Retranslate MainWindow, Lang: " << m_ini_PDA_Maze->getV_cfg_app_language();
#endif

    m_playField->retranslateUi();
}

void MainWindow::slotTimerModeChange(QAction *action)
{
#ifdef _DEBUG
    qDebug() << "Timer: " << action->data().toInt();
#endif
    m_ini_PDA_Maze->setV_cfg_timer_mode(action->data().toInt());
    m_playField->updateTimerMode(action->data().toInt());
}

void MainWindow::slotMapModeChange(QAction *action)
{
#ifdef _DEBUG
    qDebug() << "Mode: " << action->data().toInt();
#endif
    m_ini_PDA_Maze->setV_cfg_map_mode(action->data().toInt());
    m_playField->updateMapMode(action->data().toInt());
}

void MainWindow::slotMapSizeChange(QAction *action)
{
#ifdef _DEBUG
    qDebug() << "Size: " << action->data().toInt();
#endif
    m_ini_PDA_Maze->setV_cfg_map_size(action->data().toInt());
    m_playField->updateSize(action->data().toInt());
}

void MainWindow::slotScreenSizeChange(QAction *action)
{
#ifdef _DEBUG
    qDebug() << "Screen Size: " << action->data().toInt();
#endif

    m_scale = action->data().toInt();

    switch (m_scale) {
        case 0:
        {
            disableSmoothAction();
            break;
        }
        default:
        {
            m_actionSmoothScreen->setEnabled(true);
            break;
        }
    }

    m_ini_PDA_Maze->setV_cfg_scale_screen(m_scale);
    m_playField->updateScreenScale(m_scale);
    detFixedSize(m_scale);

    retranslateUi(); // Change Window Title
}

void MainWindow::slotShowStepChange(bool step)
{
#ifdef _DEBUG
    qDebug() << "StepShow: " << step;
#endif
    m_ini_PDA_Maze->setV_cfg_step_show(step);
    m_playField->updateStepStatus(step);
}

void MainWindow::slotSmoothScreenChange(bool smooth)
{
#ifdef _DEBUG
    qDebug() << "ScreenSmooth: " << smooth;
#endif
    m_ini_PDA_Maze->setV_cfg_screen_smoothing(smooth);
    m_playField->updateSmoothStatus(smooth);
}

void MainWindow::slotSwitchLanguage(QAction *action)
{
    QString locale = action->data().toString();

    m_ini_PDA_Maze->setV_cfg_app_language(locale);

    QDir qmDir("://i18n");
    QString qmPath = qmDir.absolutePath();

    appTranslator.load("PDA_Maze_" + locale, qmPath);

    m_playField->updateLang(locale);

    retranslateUi();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
#ifdef _DEBUG
    qDebug() << "Writing config...";
#endif
    m_ini_PDA_Maze->writeIniConfig();
#ifdef _DEBUG
    qDebug() << "Quiting...";
#endif
    event->accept();
}

MainWindow::~MainWindow()
{
    /* Empty Destructor */
}
