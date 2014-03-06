#ifndef INICONFIG_H
#define INICONFIG_H

#include <QObject>
#include <QSettings>

class IniConfig : public QObject
{
    Q_OBJECT

    QSettings *m_cfg_PDA_Maze;

    int v_cfg_timer_mode;
    int v_cfg_map_mode;
    int v_cfg_map_size;
    bool v_cfg_step_show;
    int v_cfg_scale_screen;
    bool v_cfg_smoothing_screen;

    void loadDefaultSettings();
public slots:
    void setV_cfg_timer_mode(int value);
    void setV_cfg_map_mode(int value);
    void setV_cfg_map_size(int value);
public:
    explicit IniConfig(QObject *parent = 0);
    bool readIniConfig();
    bool writeIniConfig();

    int getV_cfg_timer_mode() const;
    int getV_cfg_map_mode() const;
    int getV_cfg_map_size() const;

    ~IniConfig();
    bool getV_cfg_step_show() const;
    void setV_cfg_step_show(bool value);
    int getV_cfg_scale_screen() const;
    void setV_cfg_scale_screen(int value);
    bool getV_cfg_screen_smoothing() const;
    void setV_cfg_screen_smoothing(bool value);
};

#endif // INICONFIG_H
