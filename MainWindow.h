#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "IniConfig.h"
#include "PlayField.h"

#include <QMenu>
#include <QMainWindow>
#include <QAction>
#include <QActionGroup>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum TimerMode {
        TimerUp,
        TimerDown
    };

    enum MapModes {
        MapAll,
        MapBuild,
        MapNone,
    };

    IniConfig *m_ini_PDA_Maze;
    PlayField *m_playField;

    QAction *m_actionNewGame;
    QAction *m_actionQuit;
    QActionGroup *m_actionGroupTimer;
    QAction *m_actionTimer;
    QActionGroup *m_actionGroupMode;
    QAction *m_actionMode;
    QActionGroup *m_actionGroupSize;
    QAction *m_actionSize;
    QAction *m_actionAbout;
    QAction *m_actionAboutQt;

    QMenu *m_menuFile;
    QMenu *m_menuSettings;
    QMenu *m_menupTimer;
    QMenu *m_menupMode;
    QMenu *m_menupSize;
    QMenu *m_menuAbout;

    void createActions();
    QMenu *createTimerMenu();
    QMenu *createMapModeMenu();
    QMenu *createMapSizeMenu();
    void createMenus();
private slots:
    void slotTimerModeChange(QAction *);
    void slotMapModeChange(QAction *);
    void slotMapSizeChange(QAction *);
protected:
    void closeEvent(QCloseEvent *event);
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
