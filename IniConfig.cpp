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
        return false;
    } else {
        m_cfg_PDA_Maze->beginGroup("PDA_Maze");
        v_cfg_timer_mode = m_cfg_PDA_Maze->value("TimerMode").toInt();
        v_cfg_map_mode = m_cfg_PDA_Maze->value("MapMode").toInt();
        v_cfg_map_size = m_cfg_PDA_Maze->value("MapSize").toInt();
        m_cfg_PDA_Maze->endGroup();

#ifdef _DEBUG
        qDebug() << "[IniConfig.cpp::IniConfig::readIniConfig()]"
                 << "\n=== DEBUG ==="
                 << "\nv_cfg_timer_mode =" << v_cfg_timer_mode
                 << "\nv_cfg_map_mode =" << v_cfg_map_mode
                 << "\nv_cfg_map_size =" << v_cfg_map_size
                 << "\n=== END DEBUG ===";
#endif


        return true;
    }
}

void IniConfig::loadDefaultSettings()
{
    v_cfg_timer_mode = 301;
    v_cfg_map_mode = 302;
    v_cfg_map_size = 303;
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
        m_cfg_PDA_Maze->endGroup();
        return true;
    }
}

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

IniConfig::~IniConfig()
{
    /* Empty Destructor */
}
