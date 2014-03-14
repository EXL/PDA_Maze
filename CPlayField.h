/*
 * Copyright (C) 2002 Robert Ernst <robert.ernst@maxxio.com>
 * Copyright (C) 2014 EXL <exlmotodev@gmail.com>
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See COPYING for GPL licensing information.
 *
 */

#ifndef CPLAYFIELD_H
#define CPLAYFIELD_H

#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QWidget>

#ifdef _DEBUG
#include <QDebug>
#endif

class CPlayField : public QWidget
{
    Q_OBJECT

public:
    CPlayField(QWidget *parent = 0);
    void retranslateUi();
    ~CPlayField();

public:
    enum TimerModes {
    TimerUp,
    TimerDown,
    };
    enum MapModes {
    MapAll,
    MapBuild,
    MapNone,
    };
    enum Dist {
    DistFar,
    DistMid,
    DistClose,
    };

private:
    static const int max_size = 49;

private:
    QString m_rt_str_step;
    QString m_rt_str_copyright;
    QString m_rt_str_gamename;
    QString m_rt_str_kktitle;
    QString m_rt_str_kkbody;
    QString m_lang;

    size_t m_step;
    bool m_bool_step;

    int m_scr_scale;
    bool m_scr_smooth;

    enum TimerModes m_timer_mode;
    enum MapModes m_map_mode;
    int m_size;
    enum {
    Intro,
    Playing,
    ViewMap,
    GameOverWin,
    GameOverLoose,
        Help
    } m_state;
    int m_xpos;
    int m_ypos;
    enum Dirs {
    North = 0,
    East = 1,
    South = 2,
    West = 3,
    } m_dir;
    int m_maze[max_size][max_size];
    int m_seen[max_size][max_size];
    int xpos_inc[4];
    int ypos_inc[4];
    int m_counter;
    QTimer *m_timer;
    QPixmap *m_pixmap;
    QPixmap m_bkg_east;
    QPixmap m_bkg_north;
    QPixmap m_bkg_south;
    QPixmap m_bkg_west;
    QPixmap m_close_center;
    QPixmap m_close_center_bright;
    QPixmap m_close_left;
    QPixmap m_close_left_bright;
    QPixmap m_close_right;
    QPixmap m_close_right_bright;
    QPixmap m_cmp_east;
    QPixmap m_cmp_north;
    QPixmap m_cmp_south;
    QPixmap m_cmp_west;
    QPixmap m_far_center;
    QPixmap m_far_center_bright;
    QPixmap m_far_left;
    QPixmap m_far_left_bright;
    QPixmap m_far_right;
    QPixmap m_far_right_bright;
    QPixmap m_ground;
    QPixmap m_middle_center;
    QPixmap m_middle_center_bright;
    QPixmap m_middle_left;
    QPixmap m_middle_left_bright;
    QPixmap m_middle_right;
    QPixmap m_middle_right_bright;
    QPixmap m_numbers;
    QPixmap m_timeup;
    QPixmap m_title;
    QPixmap m_youwin;

public slots:
    void updateTimerMode(int timer_mode);
    void updateMapMode(int map_mode);
    void updateSize(int size);
    void updateStepStatus(bool qStep);
    void updateScreenScale(int scale);
    void updateSmoothStatus(bool smooth);
    void updateLang(QString lang);
    void timerTick(void);
    void start(void);
    void stop(void);
    void help(void);

protected:
    void paintEvent(QPaintEvent */*event*/);
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    void drawIntro(void);
    void drawHelp(void);
    void drawPlaying(void);
    void drawViewMap(void);
    void drawGameOverWin(void);
    void drawGameOverLoose(void);
    void drawAllOnWidget(QPainter &painter);
    void moveForward(void);
    void moveBackward(void);
    void turnLeft(void);
    void turnRight(void);
    void makeMaze(void);
    int mazeChunk(int xpos, int ypos);
    void drawMazeView(QPainter &painter);
    void drawCompass(QPainter &painter);
    void drawTime(QPainter &painter);
    void drawSteps(QPainter &painter);
    void drawWall(QPainter &painter, int block, enum Dist dist, int xoffset);
    void drawFarCenter(QPainter &painter, int xx);
    void drawFarLeft(QPainter &painter, int xx);
    void drawFarRight(QPainter &painter, int xx);
    void drawMidCenter(QPainter &painter, int xx);
    void drawMidLeft(QPainter &painter, int xx);
    void drawMidRight(QPainter &painter, int xx);
    void drawCloseCenter(QPainter &painter);
    void drawCloseLeft(QPainter &painter);
    void drawCloseRight(QPainter &painter);
};

#endif // CPLAYFIELD_H