#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include "CIniConfig.h"
#include "CPlayField.h"

#include <QAction>
#include <QActionGroup>
#include <QMainWindow>
#include <QMenu>
#include <QTranslator>

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    CMainWindow(QWidget *parent = 0);
    ~CMainWindow();

private:
    CIniConfig *m_IniConfig;
    CPlayField *m_PlayField;

    QTranslator appTranslator;

    QAction *m_actionNewGame;
    QAction *m_actionQuit;
    QAction *m_actionShowStep;
    QAction *m_actionSmoothScreen;
    QAction *m_actionHelp;
    QAction *m_actionAbout;
    QAction *m_actionAboutQt;

    QAction *m_actionTimer;
    QActionGroup *m_actionGroupTimer;
    QAction *m_actionMapMode;
    QActionGroup *m_actionGroupMapMode;
    QAction *m_actionMapSize;
    QActionGroup *m_actionGroupMapSize;
    QAction *m_actionScaleScreen;
    QActionGroup *m_actionGroupScaleScreen;
    QAction *m_actionAppLang;
    QActionGroup *m_actionGroupAppLang;

    QMenu *m_menuFile;
    QMenu *m_menuSettings;
    QMenu *m_menuHelp;

    QMenu *m_menupTimer;
    QMenu *m_menupMapMode;
    QMenu *m_menupMapSize;
    QMenu *m_menupAppLang;
    QMenu *m_menupScaleScreen;

private:
    void createActions();
    void createMenus();
    QMenu *createTimerMenu();
    QMenu *createMapModeMenu();
    QMenu *createMapSizeMenu();
    QMenu *createScaleScreenMenu();
    QMenu *createLanguageMenu();
    void disableSmoothScreen();
    void loadTranslations();
    void retranslateUi();
    void detFixedSize(int aScreenScale);

private slots:
    void slotTimerModeChange(QAction *);
    void slotMapModeChange(QAction *);
    void slotMapSizeChange(QAction *);
    void slotScaleScreenChange(QAction *);
    void slotShowStepChange(bool aShowStep);
    void slotSmoothScreenChange(bool aSmoothScreen);
    void slotSwitchLanguage(QAction *);

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // CMAINWINDOW_H
