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
};

#endif // INICONFIG_H
