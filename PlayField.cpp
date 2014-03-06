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

#include "PlayField.h"

#include <QApplication>
#include <QResizeEvent>
#include <QFont>
#include <QFontMetrics>

PlayField::PlayField(QWidget *parent) :
    QWidget(parent/*, name, f*/)
{
    /* Setup the default values for some member variables */
    m_timer_mode = TimerUp;
    m_map_mode = MapBuild;
    m_size = 9;
    m_xpos = -1;
    m_ypos = -1;
    m_dir = North;
    m_state = Intro;
    m_pixmap = 0;
    m_counter = 0;

    m_step = 0;
    m_bool_step = false;

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timerTick()));

    /* Initialize the xpos/ypos increment counter */
    xpos_inc[North] = 0;
    xpos_inc[East] = 1;
    xpos_inc[South] = 0;
    xpos_inc[West] = -1;
    ypos_inc[North] = -1;
    ypos_inc[East] = 0;
    ypos_inc[South] = 1;
    ypos_inc[West] = 0;

    m_pixmap = new QPixmap(160, 177);

    /* Load XMP and PNG images */
    m_bkg_east.load("://images/bkg_east.xpm");
    m_bkg_north.load("://images/bkg_north.xpm");
    m_bkg_south.load("://images/bkg_south.xpm");
    m_bkg_west.load("://images/bkg_west.xpm");

    m_close_center.load("://images/close_center.xpm");
    m_close_center_bright.load("://images/close_center_bright.xpm");
    m_close_left.load("://images/close_left.xpm");
    m_close_left_bright.load("://images/close_left_bright.xpm");
    m_close_right.load("://images/close_right.xpm");
    m_close_right_bright.load("://images/close_right_bright.xpm");

    m_cmp_east.load("://images/cmp_east.png");
    m_cmp_north.load("://images/cmp_north.png");
    m_cmp_south.load("://images/cmp_south.png");
    m_cmp_west.load("://images/cmp_west.png");

    m_far_center.load("://images/far_center.xpm");
    m_far_center_bright.load("://images/far_center_bright.xpm");
    m_far_left.load("://images/far_left.xpm");
    m_far_left_bright.load("://images/far_left_bright.xpm");
    m_far_right.load("://images/far_right.xpm");
    m_far_right_bright.load("://images/far_right_bright.xpm");

    m_ground.load("://images/ground.xpm");

    m_middle_center.load("://images/middle_center.xpm");
    m_middle_center_bright.load("://images/middle_center_bright.xpm");
    m_middle_left.load("://images/middle_left.xpm");
    m_middle_left_bright.load("://images/middle_left_bright.xpm");
    m_middle_right.load("://images/middle_right.xpm");
    m_middle_right_bright.load("://images/middle_right_bright.xpm");

    m_numbers.load("://images/numbers.xpm");
    m_timeup.load("://images/timeup.xpm");
    m_title.load("://images/title.png");
    m_youwin.load("://images/youwin.xpm");

    retranslateUi();
}

void PlayField::retranslateUi()
{
    m_rt_str_step = tr("Step: %1");
    m_rt_str_copyright = tr("Original game by Bill Kendrick\n"
                            "Zaurus port by Robert Ernst\n"
                            "Qt and MotoMAGX port by EXL\n"
                            "(c) 2001-2014");
    m_rt_str_gamename = tr("PDA Maze");
    m_rt_str_kktitle = tr("Control\nKeys");
    m_rt_str_kkbody = tr("Movement: D-pad or W, A, S, D\n"
                         "Show Map: Space or Enter\n"
                         "New Game: F5\n"
                         "Exit: Esc and F10");
}

void PlayField::paintEvent(QPaintEvent */*event*/)
{
    switch (m_state) {
    case Intro:
        drawIntro();
        break;
    case Playing:
        drawPlaying();
        break;
    case ViewMap:
        drawViewMap();
        break;
    case GameOverWin:
        drawGameOverWin();
        break;
    case GameOverLoose:
        drawGameOverLoose();
        break;
    case Help:
        drawHelp();
        break;
    }
}

void PlayField::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    setFocusPolicy(Qt::StrongFocus);
}

void PlayField::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
    if (m_state == Playing) {
        moveForward();
    }
    break;
    case Qt::Key_Down:
    case Qt::Key_S:
    if (m_state == Playing) {
        moveBackward();
    }
    break;
    case Qt::Key_Left:
    case Qt::Key_A:
    if (m_state == Playing) {
        turnLeft();
    }
    break;
    case Qt::Key_Right:
    case Qt::Key_D:
    if (m_state == Playing) {
        turnRight();
    }
    break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
    case Qt::Key_Return:
    if (m_state == Playing) {
        m_state = ViewMap;
        repaint();
    } else if (m_state == ViewMap) {
        m_state = Playing;
        repaint();
    } else {
        start();
    }
    break;
    case Qt::Key_Escape:
    if (m_state == Playing || m_state == ViewMap) {
        stop();
    }
    break;
    }
}

void PlayField::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    if (m_state == Playing) {
    int max_x = m_pixmap->width();
    if (event->y() > max_x) {
        /* click on the lower area of the screen: compass, maze, timer */
        m_state = ViewMap;
        repaint();
    } else if (abs(event->x() - event->y()) > 10
        && abs(max_x - event->x() - event->y()) > 10) {
        /* click in the upper/lower/left/right quarter of the screen */
        int x = event->x();
        int y = event->y();
        if (x > y) {
        if ((max_x - x) > y) {
            moveForward();
        } else {
            turnRight();
        }
        } else {
        if ((max_x - y) < x) {
            moveBackward();
        } else {
            turnLeft();
        }
        }

    }
    } else if (m_state == ViewMap) {
    m_state = Playing;
    repaint();
    } else {
    start();
    }
}

void PlayField::timerTick(void)
{
    if (m_timer_mode == TimerDown) {
    if (m_counter > 0) {
        m_counter--;
        if (m_counter == 0) {
        m_state = GameOverLoose;
        m_timer->stop();
        }
    }
    } else {
    m_counter++;
    }
    repaint();
}

void PlayField::start(void)
{
    m_state = Playing;
    m_timer->start(1000/*, false*/);
    m_counter = (m_timer_mode == TimerDown) ? (m_size * m_size ) - 1 : 0;
    m_step = 0;
    makeMaze();
    repaint();
}

void PlayField::stop(void)
{
    m_step = 0;
    if (m_state != Intro) {
    m_state = Intro;
    m_timer->stop();
    repaint();
    }
}

void PlayField::help()
{
    m_step = 0;
    if (m_state != Help) {
        m_state = Help;
        m_timer->stop();
        repaint();
    }
}

void PlayField::updateTimerMode(int timer_mode)
{
    if (timer_mode != m_timer_mode) {
    switch (timer_mode) {
    case TimerDown:
        m_timer_mode = TimerDown;
        break;
    default:
        m_timer_mode = TimerUp;
        break;
    }
    if (m_state == Playing || m_state == ViewMap) {
        start();
    }
    }
}

void PlayField::updateMapMode(int map_mode)
{
    if (map_mode != m_map_mode) {
    switch (map_mode) {
    case MapAll:
        m_map_mode = MapAll;
        break;
    case MapNone:
        m_map_mode = MapNone;
        break;
    default:
        m_map_mode = MapBuild;
        break;
    }
    if (m_state == Playing || m_state == ViewMap) {
        start();
    }
    }
}

void PlayField::updateSize(int size)
{
    if (size != m_size) {
    m_size = size;
    if (m_state == Playing || m_state == ViewMap) {
        start();
    }
    }
}

void PlayField::updateStepStatus(bool qStep)
{
    m_bool_step = qStep;
    repaint();
}

void PlayField::drawIntro(void)
{
    QPainter painter_pixmap(m_pixmap);
    QPainter painter_widget(this);
    QColor black(0, 0, 0);
    QColor white(255, 255, 255);
    QColor dirtywhite(242, 243, 244);

    int max_x = width();
    int max_y = height();

    painter_pixmap.fillRect(0, 0, max_x, max_y, white);
    painter_pixmap.setPen(white);

    QFont f1 = painter_pixmap.font();
    f1.setPixelSize(30);
    f1.setBold(true);
    QFont f2 = painter_pixmap.font();
    f2.setPixelSize(10);
    painter_pixmap.drawPixmap(0, 0, m_title);
    painter_pixmap.setFont(f1);
    painter_pixmap.setPen(black);
    painter_pixmap.drawText(5, 5, max_x, max_x / 2, Qt::AlignHCenter | Qt::AlignVCenter, m_rt_str_gamename);
    painter_pixmap.setPen(white);
    painter_pixmap.drawText(0, 0, max_x, max_x / 2, Qt::AlignHCenter | Qt::AlignVCenter, m_rt_str_gamename);

    painter_pixmap.fillRect(3, (max_y / 2) + 25, max_x - (3 * 2), 50, dirtywhite);

    painter_pixmap.setFont(f2);
    painter_pixmap.setPen(black);
    painter_pixmap.drawText(4, (max_y / 2) + 25, max_x - (4 * 2), 50,
                            Qt::AlignHCenter | Qt::AlignVCenter,
                            m_rt_str_copyright);

    //painter_pixmap.flush();
    painter_widget.drawPixmap(0, 0, *m_pixmap);
}

void PlayField::drawHelp(void)
{
    QPainter painter_pixmap(m_pixmap);
    QPainter painter_widget(this);
    QColor black(0, 0, 0);
    QColor white(255, 255, 255);
    QColor dirtywhite(250, 240, 190);

    int max_x = width();
    int max_y = height();

    painter_pixmap.fillRect(0, 0, max_x, max_y, white);
    painter_pixmap.setPen(white);

    QFont f1 = painter_pixmap.font();
    f1.setPixelSize(30);
    f1.setBold(true);
    QFont f2 = painter_pixmap.font();
    f2.setPixelSize(10);
    painter_pixmap.drawPixmap(0, 0, m_title);
    painter_pixmap.setFont(f1);
    painter_pixmap.setPen(black);
    painter_pixmap.drawText(5, 5, max_x, max_x / 2, Qt::AlignHCenter | Qt::AlignVCenter, m_rt_str_kktitle);
    painter_pixmap.setPen(white);
    painter_pixmap.drawText(0, 0, max_x, max_x / 2, Qt::AlignHCenter | Qt::AlignVCenter, m_rt_str_kktitle);

    painter_pixmap.fillRect(3, (max_y / 2) + 25, max_x - (3 * 2), 50, dirtywhite);

    painter_pixmap.setFont(f2);
    painter_pixmap.setPen(black);
    painter_pixmap.drawText(5, (max_y / 2) + 25, max_x - (4 * 2), 50,
                            Qt::AlignLeft,
                            m_rt_str_kkbody);

    //painter_pixmap.flush();
    painter_widget.drawPixmap(0, 0, *m_pixmap);
}

void PlayField::drawPlaying(void)
{
    QPainter painter_pixmap(m_pixmap);
    QPainter painter_widget(this);
    QColor white(255, 255, 255);

    int max_x = width();
    int max_y = height();

    painter_pixmap.fillRect(0, 0, max_x, max_y, white);
    painter_pixmap.setPen(white);

    drawMazeView(painter_pixmap);
    drawCompass(painter_pixmap);
    drawTime(painter_pixmap);
    drawSteps(painter_pixmap);

    //painter_pixmap.flush()
    painter_widget.drawPixmap(0, 0, *m_pixmap);
    //painter_widget.drawPixmap(0, 0, m_pixmap->scaled(240, 320, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void PlayField::drawViewMap(void)
{
    QPainter painter_pixmap(m_pixmap);
    QPainter painter_widget(this);
    QColor black(0, 0, 0);
    QColor grey(192, 192, 192);
    QColor white(255, 255, 255);
    QColor red(255, 0, 0);
    int max_x = width();
    int max_y = height();
    int xsize = max_x / m_size;
    int ysize = max_x / m_size;
    int xoffs = (max_x - xsize * m_size) / 2;
    int yoffs = (max_x - ysize * m_size) / 2;
    int x;
    int y;

    painter_pixmap.fillRect(0, 0, max_x, max_y, white);
    painter_pixmap.setPen(white);

    for (y = 0; y < m_size; y++) {
    for (x = 0; x < m_size; x++) {
        if (x == m_xpos && y == m_ypos) {
        painter_pixmap.fillRect(xoffs + x * xsize, yoffs + y * ysize, xsize, ysize, red);
        } else if (m_map_mode == MapNone
        || (m_map_mode == MapBuild && m_seen[y][x] == false)) {
        painter_pixmap.fillRect(xoffs + x * xsize, yoffs + y * ysize, xsize, ysize, grey);
        } else if (m_maze[y][x] == 255) {
        painter_pixmap.fillRect(xoffs + x * xsize, yoffs + y * ysize, xsize, ysize, black);
        } else {
        painter_pixmap.fillRect(xoffs + x * xsize, yoffs + y * ysize, xsize, ysize, white);
        }
    }
    }
    drawCompass(painter_pixmap);
    drawTime(painter_pixmap);

    //painter_pixmap.flush();
    painter_widget.drawPixmap(0, 0, *m_pixmap);
}

void PlayField::drawGameOverWin(void)
{
    QPainter painter_pixmap(m_pixmap);
    QPainter painter_widget(this);
    QColor white(255, 255, 255);
    int max_x = width();
    int max_y = height();

    painter_pixmap.fillRect(0, 0, max_x, max_y, white);
    painter_pixmap.setPen(white);

    int xoffs = (max_x - m_youwin.width()) / 2;
    int yoffs = (max_y - m_youwin.height()) / 2;
    drawMazeView(painter_pixmap);
    drawCompass(painter_pixmap);
    drawTime(painter_pixmap);
    drawSteps(painter_pixmap);
    painter_pixmap.drawPixmap(xoffs, yoffs, m_youwin);

    //painter_pixmap.flush();
    painter_widget.drawPixmap(0, 0, *m_pixmap);
}

void PlayField::drawGameOverLoose(void)
{
    QPainter painter_pixmap(m_pixmap);
    QPainter painter_widget(this);
    QColor white(255, 255, 255);
    int max_x = width();
    int max_y = height();

    painter_pixmap.fillRect(0, 0, max_x, max_y, white);
    painter_pixmap.setPen(white);

    int xoffs = (max_x - m_timeup.width()) / 2;
    int yoffs = (max_y - m_timeup.height()) / 2;
    drawMazeView(painter_pixmap);
    drawCompass(painter_pixmap);
    drawTime(painter_pixmap);
    drawSteps(painter_pixmap);
    painter_pixmap.drawPixmap(xoffs, yoffs, m_timeup);

    //painter_pixmap.flush();
    painter_widget.drawPixmap(0, 0, *m_pixmap);
}

void PlayField::moveForward(void)
{
    int xpos = m_xpos + xpos_inc[m_dir];
    int ypos = m_ypos + ypos_inc[m_dir];

    if (m_maze[ypos][xpos] != 255) {
    m_step++;
    m_xpos = xpos;
    m_ypos = ypos;
    m_seen[m_ypos - 1][m_xpos - 1] = true;
    m_seen[m_ypos - 1][m_xpos + 0] = true;
    m_seen[m_ypos - 1][m_xpos + 1] = true;
    m_seen[m_ypos + 0][m_xpos - 1] = true;
    m_seen[m_ypos + 0][m_xpos + 0] = true;
    m_seen[m_ypos + 0][m_xpos + 1] = true;
    m_seen[m_ypos + 1][m_xpos - 1] = true;
    m_seen[m_ypos + 1][m_xpos + 0] = true;
    m_seen[m_ypos + 1][m_xpos + 1] = true;
    if (m_xpos == 1 && m_ypos == 2) {
        m_state = GameOverWin;
        m_timer->stop();
    }
    repaint();
    }
}

void PlayField::moveBackward(void)
{
    int xpos = m_xpos - xpos_inc[m_dir];
    int ypos = m_ypos - ypos_inc[m_dir];

    if (m_maze[ypos][xpos] != 255) {
    m_step++;
    m_xpos = xpos;
    m_ypos = ypos;
    m_seen[m_ypos - 1][m_xpos - 1] = true;
    m_seen[m_ypos - 1][m_xpos + 0] = true;
    m_seen[m_ypos - 1][m_xpos + 1] = true;
    m_seen[m_ypos + 0][m_xpos - 1] = true;
    m_seen[m_ypos + 0][m_xpos + 0] = true;
    m_seen[m_ypos + 0][m_xpos + 1] = true;
    m_seen[m_ypos + 1][m_xpos - 1] = true;
    m_seen[m_ypos + 1][m_xpos + 0] = true;
    m_seen[m_ypos + 1][m_xpos + 1] = true;
    if (m_xpos == 1 && m_ypos == 2) {
        m_state = GameOverWin;
    }
    repaint();
    }
}

void PlayField::turnLeft(void)
{
    switch (m_dir) {
    case North:
    m_dir = West;
    break;
    case East:
    m_dir = North;
    break;
    case South:
    m_dir = East;
    break;
    case West:
    m_dir = South;
    break;
    }
    repaint();
}

void PlayField::turnRight(void)
{
    switch (m_dir) {
    case North:
    m_dir = East;
    break;
    case East:
    m_dir = South;
    break;
    case South:
    m_dir = West;
    break;
    case West:
    m_dir = North;
    break;
    }
    repaint();
}

int PlayField::mazeChunk(int xpos, int ypos)
{
    /* This is the function mazechunk() from Bill Kendricks pdamaze.c */
    return (xpos > 0 && ypos > 0 && xpos < (m_size - 1) && ypos < (m_size - 1))
    ? m_maze[ypos][xpos] : -1;
}

void PlayField::makeMaze(void)
{
    /* This is the function create_maze() from Bill Kendricks pdamaze.c */
    int x;
    int y;

    for (y = 1; y < (m_size - 1); y++) {
    for (x = 1; x < (m_size - 1); x++) {
        m_maze[y][x] = 255;
    }
    m_maze[y][0] = -1;
    m_maze[y][m_size - 1] = -1;
    }
    for (x = 0; x < m_size; x++) {
    m_maze[0][x] = -1;
    m_maze[m_size - 1][x] = -1;
    }

    y = 2;
    x = 2;
    int xx = 0;
    int j = 0;
    int ok = 2;
    m_maze[y][x] = 5;

    do {
    if (ok == 2) {
        j = (rand() % 4);
        xx = j;
    }
    ok = 0;
    int nx = x + xpos_inc[j] * 2;
    int ny = y + ypos_inc[j] * 2;
    if (mazeChunk(nx, ny) == 255) {
        m_maze[ny][nx] = j + 1;
        m_maze[y + ypos_inc[j]][x + xpos_inc[j]] = 0;
        x = nx;
        y = ny;
        ok = 2;
    }
    if (ok == 0) {
        if (j < 3) {
        j = j + 1;
        while (j > 3) {
            j = j - 4;
        }
        } else {
        j = 0;
        }
        if (j != xx) {
        ok = 1;
        }
    }
    if (ok == 0) {
        j = mazeChunk(x, y);
        m_maze[y][x] = 0;
        if (j < 5) {
        x = x - (xpos_inc[j - 1] * 2);
        y = y - (ypos_inc[j - 1] * 2);
        ok = 2;
        }
    }
    } while (ok != 0);

    for (x = 0; x < m_size; x++) {
    m_seen[0][x] = 1;
    m_seen[1][x] = 1;
    m_seen[m_size - 2][x] = 1;
    m_seen[m_size - 1][x] = 1;
    }
    for (y = 2; y < m_size - 2; y++) {
    m_seen[y][0] = 1;
    m_seen[y][1] = 1;
    m_seen[y][m_size - 2] = 1;
    m_seen[y][m_size - 1] = 1;
    for (x = 2; x < m_size - 2; x++) {
        m_seen[y][x] = 0;
    }
    }

    m_maze[2][1] = 0;

    m_xpos = m_size / 2 + 1;
    m_ypos = m_size / 2 + 1;
    m_xpos = m_xpos + (m_xpos % 2);
    m_ypos = m_ypos + (m_ypos % 2);
    do {
    switch(rand() % 4) {
    case 0:
        m_dir = North;
        break;
    case 1:
        m_dir = South;
        break;
    case 2:
        m_dir = West;
        break;
    case 3:
        m_dir = East;
        break;
    }
    } while (m_maze[m_ypos + ypos_inc[m_dir]][m_xpos + xpos_inc[m_dir]] == 255);
}

void PlayField::drawMazeView(QPainter &painter)
{
    switch (m_dir) {
    case North:
    painter.drawPixmap(0, 0, m_bkg_north);
    break;
    case East:
    painter.drawPixmap(0, 0, m_bkg_east);
    break;
    case South:
    painter.drawPixmap(0, 0, m_bkg_south);
    break;
    case West:
    painter.drawPixmap(0, 0, m_bkg_west);
    break;
    }
    painter.drawPixmap(0, 80, m_ground);

    switch (m_dir) {
    case North:
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos - 3), DistFar, -1);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos - 3), DistFar, 1);
    drawWall(painter, mazeChunk(m_xpos + 0, m_ypos - 3), DistFar, 0);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos - 2), DistMid, -1);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos - 2), DistMid, 1);
    drawWall(painter, mazeChunk(m_xpos + 0, m_ypos - 2), DistMid, 0);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos - 1), DistClose, -1);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos - 1), DistClose, 1);
    drawWall(painter, mazeChunk(m_xpos + 0, m_ypos - 1), DistClose, 0);
    break;
    case East:
    drawWall(painter, mazeChunk(m_xpos + 3, m_ypos - 1), DistFar, -1);
    drawWall(painter, mazeChunk(m_xpos + 3, m_ypos + 1), DistFar, 1);
    drawWall(painter, mazeChunk(m_xpos + 3, m_ypos + 0), DistFar, 0);
    drawWall(painter, mazeChunk(m_xpos + 2, m_ypos - 1), DistMid, -1);
    drawWall(painter, mazeChunk(m_xpos + 2, m_ypos + 1), DistMid, 1);
    drawWall(painter, mazeChunk(m_xpos + 2, m_ypos + 0), DistMid, 0);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos - 1), DistClose, -1);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos + 1), DistClose, 1);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos + 0), DistClose, 0);
    break;
    case South:
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos + 3), DistFar, -1);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos + 3), DistFar, 1);
    drawWall(painter, mazeChunk(m_xpos + 0, m_ypos + 3), DistFar, 0);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos + 2), DistMid, -1);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos + 2), DistMid, 1);
    drawWall(painter, mazeChunk(m_xpos + 0, m_ypos + 2), DistMid, 0);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos + 1), DistClose, -1);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos + 1), DistClose, 1);
    drawWall(painter, mazeChunk(m_xpos + 0, m_ypos + 1), DistClose, 0);
    break;
    case West:
    drawWall(painter, mazeChunk(m_xpos - 3, m_ypos + 1), DistFar, -1);
    drawWall(painter, mazeChunk(m_xpos - 3, m_ypos - 1), DistFar, 1);
    drawWall(painter, mazeChunk(m_xpos - 3, m_ypos + 0), DistFar, 0);
    drawWall(painter, mazeChunk(m_xpos - 2, m_ypos + 1), DistMid, -1);
    drawWall(painter, mazeChunk(m_xpos - 2, m_ypos - 1), DistMid, 1);
    drawWall(painter, mazeChunk(m_xpos - 2, m_ypos + 0), DistMid, 0);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos + 1), DistClose, -1);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos - 1), DistClose, 1);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos + 0), DistClose, 0);
    break;
    }
}

void PlayField::drawCompass(QPainter &painter)
{
    int max_x = 160;

    switch (m_dir) {
    case North:
    painter.drawPixmap(0, max_x, m_cmp_north);
    break;
    case East:
    painter.drawPixmap(0, max_x, m_cmp_east);
    break;
    case South:
    painter.drawPixmap(0, max_x, m_cmp_south);
    break;
    case West:
    painter.drawPixmap(0, max_x, m_cmp_west);
    break;
    }
}

void PlayField::drawTime(QPainter &painter)
{
    int max_x = 160;

    int min = (m_counter / 60) % 60;
    int sec = m_counter % 60;
    int i;

    char str[5] = {
        (char)((min / 10) + '0'),
        (char)((min % 10) + '0'),
        ':',
        (char)((sec / 10) + '0'),
        (char)((sec % 10) + '0')
    };

    for (i = 0; i < 5; i++) {
        int number = str[i] - '0';
        painter.drawPixmap(118 + i * 8, max_x + 2, m_numbers, number * 8, 0, 8, 12);
    }
}

void PlayField::drawSteps(QPainter &painter)
{
    if (m_bool_step) {
        int w = painter.fontMetrics().width(m_rt_str_step.arg(m_step));
        int h = painter.fontMetrics().height();

        painter.fillRect(100, 2, w, h, QColor(242, 243, 244));

        painter.setPen(Qt::black);
        painter.drawText(100, 2, w, h, Qt::AlignRight, m_rt_str_step.arg(m_step));
    }
}

void PlayField::drawWall(QPainter &painter, int block, enum Dist dist, int xoffset)
{
    if (block == 255) {
    if (dist == DistFar) {
        if (xoffset < 0) {
        drawFarCenter(painter, 80 - 12 + (xoffset * 24));
        drawFarLeft(painter, 80 - 12 + (xoffset * 24) + 24);
        } else if (xoffset == 0) {
        drawFarCenter(painter, 80 - 12);
        } else if (xoffset > 0) {
        drawFarCenter(painter, 80 - 12 + (xoffset * 24));
        drawFarRight(painter, 80 - 12 + (xoffset * 24) - 10);
        }
    } else if (dist == DistMid) {
        if (xoffset < 0) {
        drawMidCenter(painter, 80 - 33 - (66 - 24) + (xoffset * 24)); // 49 -> 49.5
        drawMidLeft(painter, 80 - 33 - (66 - 24) + (xoffset * 24) + 66);
        } else if (xoffset == 0) {
        drawMidCenter(painter, 80 - 33);
        } else if (xoffset > 0) {
        drawMidCenter(painter, 80 - 33 + (66 - 24) + (xoffset * 24));
        drawMidRight(painter, 80 - 33 + (66 - 24) + (xoffset * 24) - 21); // 30 -> 30.5
        }
    } else if (dist == DistClose) {
        if (xoffset < 0) {
        drawCloseLeft(painter);
        } else if (xoffset == 0) {
        drawCloseCenter(painter);
        } else if (xoffset > 0) {
        drawCloseRight(painter);
        }
    }
    }
}

void PlayField::drawFarCenter(QPainter &painter, int xx)
{
    painter.drawPixmap(xx, 80 - 12 + 1,
    (m_dir == North || m_dir == West) ? m_far_center_bright : m_far_center);
}

void PlayField::drawFarLeft(QPainter &painter, int xx)
{
    QPixmap pix;
    int y;

    pix = (m_dir == North || m_dir == East) ? m_far_left_bright : m_far_left;
    for (y = 0; y < 11; y++) { // 16.5 -> 16
        if (!y) {
            continue;
        }
    painter.drawPixmap(xx, 80 - 12 + y, pix, 0, y, y, 1);
    painter.drawPixmap(xx, 80 - 12 + 24 - y, pix, 0, 23 - y, y, 1); // 34.5 -> 34
    }
    painter.drawPixmap(xx, 80 - 12 + 11, pix, 0, 11, 12, 3); // 16.5 -> 16
}

void PlayField::drawFarRight(QPainter &painter, int xx)
{
    QPixmap pix;
    int y;

    pix = (m_dir == South || m_dir == West) ? m_far_right_bright : m_far_right;
    for (y = 0; y < 11; y++) { // 16.5 -> 16
    painter.drawPixmap(xx + 11 - y, 80 - 12 + y, pix, 11 - y, y, y, 1); // 16.5 -> 16
    painter.drawPixmap(xx + 11 - y, 80 - 12 + 24 - y + 1, pix, 11 - y, 24 - y, y, 1); // 16.5 -> 16
    }
    painter.drawPixmap(xx, 80 - 12 + 11, pix, 0, 11, 12, 4); // 16.5 -> 16
}

void PlayField::drawMidCenter(QPainter &painter, int xx)
{
    painter.drawPixmap(xx, 80 - 33 + 1,
    (m_dir == North || m_dir == West) ? m_middle_center_bright : m_middle_center); // 49 -> 49.5
}

void PlayField::drawMidLeft(QPainter &painter, int xx)
{
    QPixmap pix;
    int y;

    pix = (m_dir == North || m_dir == East) ? m_middle_left_bright : m_middle_left;
    for (y = 0; y < 22; y++) {
        if (!y) {
            continue;
        }
    painter.drawPixmap(xx, 80 - 33 + y, pix, 0, y, y, 1); // 49 -> 49.5
    painter.drawPixmap(xx, 80 - 33 + 66 - y, pix, 0, 66 - y, y, 1); // 49 -> 49.5
    }
    painter.drawPixmap(xx, 80 - 33 + 22, pix, 0, 23, 24, 23); // 49 -> 49.5, 37 -> 37.5
}

void PlayField::drawMidRight(QPainter &painter, int xx)
{
    QPixmap pix;
    int y;

    pix = (m_dir == South || m_dir == West) ? m_middle_right_bright : m_middle_right;
    for (y = 0; y < 22; y++) {
    painter.drawPixmap(xx + 22 - y, 80 - 33 + y, pix, 22 - y, y, y, 1); // 33 -> 33.5
    painter.drawPixmap(xx + 22 - y, 80 - 33 + 66 - y + 1, pix, 22 - y, 66 - y, y, 1); // 33 -> 33.5
    }
    painter.drawPixmap(xx, 80 - 33 + 22, pix, 0, 22, 24, 24); // 33 -> 33.5, 37 -> 37.5
}

void PlayField::drawCloseCenter(QPainter &painter)
{
    painter.drawPixmap(0, 0,
    (m_dir == North || m_dir == West) ? m_close_center_bright : m_close_center);
}

void PlayField::drawCloseLeft(QPainter &painter)
{
    QPixmap pix;
    int y;

    pix = (m_dir == North || m_dir == East) ? m_close_left_bright : m_close_left;
    for (y = 0; y < 51; y++) { // 76 -> 76.5
        if (!y) {
            continue;
        }
    painter.drawPixmap(0, y, pix, 0, y, y, 1);
    painter.drawPixmap(0, 160 - y, pix, 0, 160 - y, y, 1);
    }
    painter.drawPixmap(0, 51, pix, 0, 51, 50, 60); // 76 -> 76.5
}

void PlayField::drawCloseRight(QPainter &painter)
{
    QPixmap pix;
    int y;

    pix = (m_dir == South || m_dir == West) ? m_close_right_bright : m_close_right;
    for (y = 0; y < 51; y++) { // 76 -> 76.5
    painter.drawPixmap(160 - y + 1, y, pix, 51 - y - 1, y, y, 1); // 76 -> 76.5
    painter.drawPixmap(160 - y, 160 - y, pix, 51 - y - 1, 160 - y, y, 1); // 76 -> 76.5
    }
    painter.drawPixmap(110, 51, pix, 0, 51, 50, 60); // 76 -> 76.5
}

PlayField::~PlayField()
{
    delete m_pixmap;
    delete m_timer;
}
