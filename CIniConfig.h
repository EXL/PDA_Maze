#ifndef CINICONFIG_H
#define CINICONFIG_H

#include <QObject>
#include <QSettings>

class CIniConfig : public QObject
{
    Q_OBJECT

public:
    explicit CIniConfig(QObject *parent = 0);
    ~CIniConfig();

public:
    enum TTimerModes {
        ETimerUp,
        ETimerDown
    };

    enum TMapModes {
        EMapAll,
        EMapBuild,
        EMapNone
    };

    enum TScaleScreen {
        ERes160x177,
        ERes240x265,
        ERes480x531
    };

    enum TMapSize {
        EMapSize9x9 = 9,
        EMapSize19x19 = 19,
        EMapSize29x29 = 29,
        EMapSize39x39 = 39,
        EMapSize49x49 = 49,
    };

private:
    QSettings *m_Settings;

    int iniTimerMode;
    int iniMapMode;
    int iniMapSize;
    bool iniStepShow;
    int iniScaleScreen;
    bool iniSmoothScreen;
    QString iniAppLang;

    void loadDefaultSettings();
    void showAllVariables();
    bool checkAllVariables();

public:
    bool readIniConfig();
    bool writeIniConfig();

    void setIniTimerMode(int aTimerMode);
    int getIniTimerMode() const;
    void setIniMapMode(int aMapMode);
    int getIniMapMode() const;
    void setIniMapSize(int aMapSize);
    int getIniMapSize() const;

    void setIniStepShow(bool aStepShow);
    bool getIniStepShow() const;
    void setIniScaleScreen(int aScaleScreen);
    int getIniScaleScreen() const;
    void setIniSmoothScreen(bool aSmoothScreen);
    bool getIniSmoothScreen() const;
    void setIniAppLang(const QString &aAppLang);
    QString getIniAppLang() const;
};

#endif // CINICONFIG_H
