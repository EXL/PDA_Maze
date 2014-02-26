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
    isRunning = false;

    setWindowTitle("PDA Maze");

    m_ini_PDA_Maze = new IniConfig();
    m_ini_PDA_Maze->readIniConfig();

    m_playField = new PlayField(this);
    resetMazeVariables();

    createActions();

    createMenus();

    setCentralWidget(m_playField);
}

void MainWindow::resetMazeVariables()
{
    m_playField->setTimer_mode(
                (PlayField::TimerModes)
                m_ini_PDA_Maze->getV_cfg_timer_mode());
    m_playField->setMap_mode(
                (PlayField::MapModes)
                m_ini_PDA_Maze->getV_cfg_map_mode());
    m_playField->setMapSize(m_ini_PDA_Maze->getV_cfg_map_size());
}

void MainWindow::createActions()
{
    m_actionNewGame = new QAction(this);
    m_actionNewGame->setText(tr("&New Game"));
    m_actionNewGame->setShortcut(Qt::Key_F5);
    connect(m_actionNewGame, SIGNAL(triggered()),
            m_playField, SLOT(start()));
    connect(m_actionNewGame, SIGNAL(triggered()),
            this, SLOT(gameIsRunning()));

    m_actionQuit = new QAction(this);
    m_actionQuit->setText(tr("&Quit"));
    m_actionQuit->setShortcut(Qt::Key_F10);
    connect(m_actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    m_actionAbout = new QAction(this);
    m_actionAbout->setText(tr("&About..."));
    connect(m_actionAbout, SIGNAL(triggered()), m_playField, SLOT(stop()));
    connect(m_actionAbout, SIGNAL(triggered()), this, SLOT(gameIsntRunning()));

    m_actionAboutQt = new QAction(this);
    m_actionAboutQt->setText(tr("About &Qt"));
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

    m_menuAbout = new QMenu();
    m_menuAbout->setTitle(tr("&About"));
    m_menuAbout->addAction(m_actionAbout);
    m_menuAbout->addAction(m_actionAboutQt);

    menuBar()->addMenu(m_menuFile);
    menuBar()->addMenu(m_menuSettings);
    menuBar()->addMenu(m_menuAbout);
}

void MainWindow::slotTimerModeChange(QAction *action)
{
#ifdef _DEBUG
    qDebug() << "Timer: " << action->data().toInt();
#endif
    m_ini_PDA_Maze->setV_cfg_timer_mode(action->data().toInt());
    resetMazeVariables();
    if (isRunning) {
        m_playField->start();
    }
}

void MainWindow::slotMapModeChange(QAction *action)
{
#ifdef _DEBUG
    qDebug() << "Mode: " << action->data().toInt();
#endif
    m_ini_PDA_Maze->setV_cfg_map_mode(action->data().toInt());
    resetMazeVariables();
    if(isRunning) {
        m_playField->start();
    }
}

void MainWindow::slotMapSizeChange(QAction *action)
{
#ifdef _DEBUG
    qDebug() << "Size: " << action->data().toInt();
#endif
    m_ini_PDA_Maze->setV_cfg_map_size(action->data().toInt());
    resetMazeVariables();
    if(isRunning) {
        m_playField->start();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_ini_PDA_Maze->writeIniConfig();
    event->accept();
}

MainWindow::~MainWindow()
{
    /* Empty Destructor */
}
