#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "IniConfig.h"
#include "PlayField.h"

#include <QMenu>
#include <QMainWindow>
#include <QAction>
#include <QActionGroup>
#include <QTranslator>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    int m_scale;

    IniConfig *m_ini_PDA_Maze;
    PlayField *m_playField;

    QTranslator appTranslator;

    QAction *m_actionNewGame;
    QAction *m_actionQuit;
    QAction *m_actionStep;
    QAction *m_actionSmoothScreen;
    QActionGroup *m_actionGroupTimer;
    QAction *m_actionTimer;
    QActionGroup *m_actionGroupMode;
    QAction *m_actionMode;
    QActionGroup *m_actionGroupSize;
    QAction *m_actionSize;
    QActionGroup *m_actionGroupScreenSize;
    QAction *m_actionScreenSize;
    QActionGroup *m_actionGroupLang;
    QAction *m_actionLang;
    QAction *m_actionHelp;
    QAction *m_actionAbout;
    QAction *m_actionAboutQt;

    QMenu *m_menuFile;
    QMenu *m_menuSettings;
    QMenu *m_menupTimer;
    QMenu *m_menupMode;
    QMenu *m_menupSize;
    QMenu *m_menupLang;
    QMenu *m_menupScreenSize;
    QMenu *m_menuHelp;

    void createActions();
    QMenu *createTimerMenu();
    QMenu *createMapModeMenu();
    QMenu *createMapSizeMenu();
    QMenu *createScreenSizeMenu();
    QMenu *createLanguageMenu();
    void createMenus();
    void disableSmoothAction();
    void detFixedSize(int scale);
    void loadTranslations();
    void retranslateUi();
private slots:
    void slotTimerModeChange(QAction *);
    void slotMapModeChange(QAction *);
    void slotMapSizeChange(QAction *);
    void slotScreenSizeChange(QAction *);
    void slotShowStepChange(bool step);
    void slotSmoothScreenChange(bool smooth);
    void slotSwitchLanguage(QAction *);
protected:
    void closeEvent(QCloseEvent *event);
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
