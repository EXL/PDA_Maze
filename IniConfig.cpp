#include "IniConfig.h"

#include <QFileInfo>
#include <QApplication>

#ifdef _DEBUG
#include <QDebug>
#endif

IniConfig::IniConfig(QObject *parent) :
    QObject(parent)
{
    m_cfg_PDA_Maze = new QSettings(QString("PDA_Maze.ini"),
                                   QSettings::IniFormat);
}

bool IniConfig::readIniConfig()
{
    QFileInfo iniFile("PDA_Maze.ini");

    if(!(iniFile.exists())) {
#ifdef _DEBUG
        qDebug() << "[IniConfig.cpp::IniConfig::readIniConfig()]: "
                    "Warning: Configuration File isn't found! "
                    "The default configuration will be loaded!";
#endif
        loadDefaultSettings();
        return true;
    } else if(!(iniFile.isReadable())){
#ifdef _DEBUG
        qDebug() << "[IniConfig.cpp::IniConfig::readIniConfig()]: "
                    "Error: Configuration File isn't Readable!";
#endif
        loadDefaultSettings();
        return false;
    } else {
        m_cfg_PDA_Maze->beginGroup("PDA_Maze");
        v_cfg_timer_mode = m_cfg_PDA_Maze->value("TimerMode").toInt();
        v_cfg_map_mode = m_cfg_PDA_Maze->value("MapMode").toInt();
        v_cfg_map_size = m_cfg_PDA_Maze->value("MapSize").toInt();
        v_cfg_step_show = m_cfg_PDA_Maze->value("ShowStep").toBool();
        v_cfg_scale_screen = m_cfg_PDA_Maze->value("ScaleScreen").toInt();
        v_cfg_smoothing_screen = m_cfg_PDA_Maze->value("SmoothScreen").toBool();
        m_cfg_PDA_Maze->endGroup();

#ifdef _DEBUG
        qDebug() << "[IniConfig.cpp::IniConfig::readIniConfig()]"
                 << "\n=== DEBUG ==="
                 << "\nv_cfg_timer_mode =" << v_cfg_timer_mode
                 << "\nv_cfg_map_mode =" << v_cfg_map_mode
                 << "\nv_cfg_map_size =" << v_cfg_map_size
                 << "\nv_cfg_step_show =" << v_cfg_step_show
                 << "\nv_cfg_scale_screen =" << v_cfg_scale_screen
                 << "\nv_cfg_smoothing_screen =" << v_cfg_smoothing_screen
                 << "\n=== END DEBUG ===";
#endif


        return true;
    }
}

void IniConfig::loadDefaultSettings()
{
    v_cfg_timer_mode = 0;           // Timer Up
    v_cfg_map_mode = 1;             // Build Map Mode
    v_cfg_map_size = 29;            // 29x29 Size
    v_cfg_step_show = false;        // StepShow false
    v_cfg_scale_screen = 0;         // Don't scale screen
    v_cfg_smoothing_screen = false; // Don't smooth screen
}

bool IniConfig::writeIniConfig()
{
    QFileInfo iniFile("PDA_Maze.ini");
#ifdef _DEBUG
    if(!(iniFile.exists())) {
        qDebug() << "[IniConfig.cpp::IniConfig::writeIniConfig()]: "
                    "Error: Configuration File isn't found! "
                    "New configuration file will be created!";
    }
#endif
    if(iniFile.exists() && !(iniFile.isWritable())) {
#ifdef _DEBUG
        qDebug() << "[IniConfig.cpp::IniConfig::writeIniConfig()]: "
                    "Error: Configuration File isn't writable!";
#endif
        return false;
    } else {
        m_cfg_PDA_Maze->beginGroup("PDA_Maze");
        m_cfg_PDA_Maze->setValue("TimerMode", v_cfg_timer_mode);
        m_cfg_PDA_Maze->setValue("MapMode",  v_cfg_map_mode);
        m_cfg_PDA_Maze->setValue("MapSize", v_cfg_map_size);
        m_cfg_PDA_Maze->setValue("ShowStep", v_cfg_step_show);
        m_cfg_PDA_Maze->setValue("ScaleScreen", v_cfg_scale_screen);
        m_cfg_PDA_Maze->setValue("SmoothScreen", v_cfg_smoothing_screen);
        m_cfg_PDA_Maze->endGroup();
        return true;
    }
}

/* Getters and Setters */

int IniConfig::getV_cfg_timer_mode() const
{
    return v_cfg_timer_mode;
}

void IniConfig::setV_cfg_timer_mode(int value)
{
    v_cfg_timer_mode = value;
}

int IniConfig::getV_cfg_map_mode() const
{
    return v_cfg_map_mode;
}

void IniConfig::setV_cfg_map_mode(int value)
{
    v_cfg_map_mode = value;
}

int IniConfig::getV_cfg_map_size() const
{
    return v_cfg_map_size;
}

void IniConfig::setV_cfg_map_size(int value)
{
    v_cfg_map_size = value;
}

bool IniConfig::getV_cfg_step_show() const
{
    return v_cfg_step_show;
}

void IniConfig::setV_cfg_step_show(bool value)
{
    v_cfg_step_show = value;
}

int IniConfig::getV_cfg_scale_screen() const
{
    return v_cfg_scale_screen;
}

void IniConfig::setV_cfg_scale_screen(int value)
{
    v_cfg_scale_screen = value;
}

bool IniConfig::getV_cfg_screen_smoothing() const
{
    return v_cfg_smoothing_screen;
}

void IniConfig::setV_cfg_screen_smoothing(bool value)
{
    v_cfg_smoothing_screen = value;
}

IniConfig::~IniConfig()
{
    /* Empty Destructor */
}
