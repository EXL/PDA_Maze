#include "MainWindow.h"

#include <QCloseEvent>
#include <QMenuBar>
#include <QApplication>

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

    m_playField = new PlayField(this);
    m_playField->updateTimerMode(m_ini_PDA_Maze->getV_cfg_timer_mode());
    m_playField->updateMapMode(m_ini_PDA_Maze->getV_cfg_map_mode());
    m_playField->updateSize(m_ini_PDA_Maze->getV_cfg_map_size());
    m_playField->updateStepStatus(m_ini_PDA_Maze->getV_cfg_step_show());
    m_playField->updateScreenScale(m_ini_PDA_Maze->getV_cfg_scale_screen());
    m_playField->updateSmoothStatus(m_ini_PDA_Maze->getV_cfg_screen_smoothing());

    createActions();
    m_actionStep->setChecked(m_ini_PDA_Maze->getV_cfg_step_show());
    if (m_ini_PDA_Maze->getV_cfg_scale_screen()) {
        m_actionSmoothScreen->setChecked(m_ini_PDA_Maze->getV_cfg_screen_smoothing());
    } else {
        disableSmoothAction();
    }

    createMenus();

    setCentralWidget(m_playField);

    detFixedSize(m_ini_PDA_Maze->getV_cfg_scale_screen());
    setWindowIcon(QIcon("://icons/PDA_maze_64x64.png"));
}

void MainWindow::createActions()
{
    m_actionNewGame = new QAction(this);
    m_actionNewGame->setText(tr("&New Game"));
    m_actionNewGame->setShortcut(Qt::Key_F5);
    connect(m_actionNewGame, SIGNAL(triggered()),
            m_playField, SLOT(start()));

    m_actionQuit = new QAction(this);
    m_actionQuit->setText(tr("&Quit"));
    m_actionQuit->setShortcut(Qt::Key_F10);
    connect(m_actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    m_actionStep = new QAction(this);
    m_actionStep->setText(tr("&Step Counter"));
    m_actionStep->setCheckable(true);
    connect(m_actionStep, SIGNAL(triggered(bool)),
            this, SLOT(slotShowStepChange(bool)));

    m_actionSmoothScreen = new QAction(this);
    m_actionSmoothScreen->setText(tr("Smooth"));
    m_actionSmoothScreen->setCheckable(true);
    connect(m_actionSmoothScreen, SIGNAL(triggered(bool)),
            this, SLOT(slotSmoothScreenChange(bool)));

    m_actionHelp = new QAction(this);
    m_actionHelp->setText(tr("&Help"));
    m_actionHelp->setShortcut(Qt::Key_F1);
    connect(m_actionHelp, SIGNAL(triggered()), m_playField, SLOT(help()));

    m_actionAbout = new QAction(this);
    m_actionAbout->setText(tr("&About"));
    connect(m_actionAbout, SIGNAL(triggered()), m_playField, SLOT(stop()));

    m_actionAboutQt = new QAction(this);
    m_actionAboutQt->setText(tr("About &Qt..."));
    connect(m_actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

QMenu *MainWindow::createTimerMenu()
{
    m_menupTimer = new QMenu();
    m_menupTimer->setTitle(tr("&Timer"));

    m_actionGroupTimer = new QActionGroup(this);
    connect(m_actionGroupTimer, SIGNAL(triggered(QAction *)),
            this, SLOT(slotTimerModeChange(QAction *)));

    for(size_t i = 0; i < 2; ++i) {
        m_actionTimer = new QAction(this);
        m_actionTimer->setCheckable(true);
        m_actionTimer->setData((int)i);

        m_menupTimer->addAction(m_actionTimer);
        m_actionGroupTimer->addAction(m_actionTimer);

        switch (i) {
        case PlayField::TimerUp:
        {
            m_actionTimer->setText(tr("Timer Up"));
            break;
        }
        case PlayField::TimerDown:
        {
            m_actionTimer->setText(tr("Timer Down"));
            break;
        }
        default:
            break;
        }

        if((int)i == m_ini_PDA_Maze->getV_cfg_timer_mode()) {
            m_actionTimer->setChecked(true);
        }
    }

    return m_menupTimer;
}

QMenu *MainWindow::createMapModeMenu()
{
    m_menupMode = new QMenu();
    m_menupMode->setTitle(tr("&Map"));

    m_actionGroupMode = new QActionGroup(this);
    connect(m_actionGroupMode, SIGNAL(triggered(QAction *)),
            this, SLOT(slotMapModeChange(QAction *)));

    for(size_t i = 0; i < 3; ++i) {
        m_actionMode = new QAction(this);
        m_actionMode->setCheckable(true);
        m_actionMode->setData((int)i);

        m_menupMode->addAction(m_actionMode);
        m_actionGroupMode->addAction(m_actionMode);

        switch (i) {
        case PlayField::MapAll:
        {
            m_actionMode->setText(tr("All"));
            break;
        }
        case PlayField::MapBuild:
        {
            m_actionMode->setText(tr("Build"));
            break;
        }
        case PlayField::MapNone:
        {
            m_actionMode->setText(tr("None"));
            break;
        }
        default:
            break;
        }

        if((int)i == m_ini_PDA_Maze->getV_cfg_map_mode()) {
            m_actionMode->setChecked(true);
        }
    }

    return m_menupMode;
}

QMenu *MainWindow::createMapSizeMenu()
{
    m_menupSize = new QMenu();
    m_menupSize->setTitle(tr("Si&ze"));

    m_actionGroupSize = new QActionGroup(this);
    connect(m_actionGroupSize, SIGNAL(triggered(QAction *)),
            this, SLOT(slotMapSizeChange(QAction *)));

    for(size_t i = 9; i < 50; i+=10) {
        m_actionSize = new QAction(this);
        m_actionSize->setCheckable(true);
        m_actionSize->setData((int)i);

        m_menupSize->addAction(m_actionSize);
        m_actionGroupSize->addAction(m_actionSize);

        switch (i) {
        case 9:
        {
            m_actionSize->setText(tr("9x9"));
            break;
        }
        case 19:
        {
            m_actionSize->setText(tr("19x19"));
            break;
        }
        case 29:
        {
            m_actionSize->setText(tr("29x29"));
            break;
        }
        case 39:
        {
            m_actionSize->setText(tr("39x39"));
            break;
        }
        case 49:
        {
            m_actionSize->setText(tr("49x49"));
            break;
        }
        default:
            break;
        }

        if((int)i == m_ini_PDA_Maze->getV_cfg_map_size()) {
            m_actionSize->setChecked(true);
        }
    }

    return m_menupSize;
}

QMenu *MainWindow::createScreenSizeMenu()
{
    m_menupScreenSize = new QMenu();
    m_menupScreenSize->setTitle(tr("S&creen Size"));

    m_actionGroupScreenSize = new QActionGroup(this);
    connect(m_actionGroupScreenSize, SIGNAL(triggered(QAction*)),
            this, SLOT(slotScreenSizeChange(QAction *)));

    for (size_t i = 0; i < 3; ++i) {
        m_actionScreenSize = new QAction(this);
        m_actionScreenSize->setCheckable(true);
        m_actionScreenSize->setData((int)i);

        m_menupScreenSize->addAction(m_actionScreenSize);
        m_actionGroupScreenSize->addAction(m_actionScreenSize);

        switch (i) {
        case 0:
        {
            m_actionScreenSize->setText(tr("160x177"));
            break;
        }
        case 1:
        {
            m_actionScreenSize->setText(tr("240x265"));
            break;
        }
        case 2:
        {
            m_actionScreenSize->setText(tr("480x531"));
            break;
        }
        default:
        {
#ifdef _DEBUG
            qDebug() << "Error creating screen size menu!";
#endif
            break;
        }
        }

        if ((int)i == m_ini_PDA_Maze->getV_cfg_scale_screen()) {
            m_actionScreenSize->setChecked(true);
        }
    }

    m_menupScreenSize->addSeparator();
    m_menupScreenSize->addAction(m_actionSmoothScreen);

    return m_menupScreenSize;
}

void MainWindow::createMenus()
{
    m_menuFile = new QMenu();
    m_menuFile->setTitle(tr("&Game"));
    m_menuFile->addAction(m_actionNewGame);
    m_menuFile->addSeparator();
    m_menuFile->addAction(m_actionQuit);

    m_menuSettings = new QMenu();
    m_menuSettings->setTitle(tr("&Settings"));
    m_menuSettings->addMenu(createTimerMenu());
    m_menuSettings->addMenu(createMapModeMenu());
    m_menuSettings->addMenu(createMapSizeMenu());
    m_menuSettings->addSeparator();
    m_menuSettings->addAction(m_actionStep);
    m_menuSettings->addSeparator();
    m_menuSettings->addMenu(createScreenSizeMenu());

    m_menuHelp = new QMenu();
    m_menuHelp->setTitle(tr("&Help"));
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
        setWindowTitle(tr("PDA Maze"));
        m_playField->setFixedSize(240, 265);
        setFixedSize(240, 265 + m);
        break;
    }
    case 2:
    {
        setWindowTitle(tr("PDA Maze"));
        m_playField->setFixedSize(480, 531);
        setFixedSize(480, 531 + m);
        break;
    }
    case 0:
    default:
    {
        setWindowTitle(tr("Maze"));
        m_playField->setFixedSize(160, 177);
        setFixedSize(160, 177 + m); // adjustSize() isn't working correctly
        break;
    }
    }
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

    switch (action->data().toInt()) {
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

    m_ini_PDA_Maze->setV_cfg_scale_screen(action->data().toInt());
    m_playField->updateScreenScale(action->data().toInt());
    detFixedSize(action->data().toInt());
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
