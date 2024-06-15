#include "game.h"
#include <QImage>

GameField::GameField()
{
    setFixedSize(600, 600);
    setFocusPolicy(Qt::StrongFocus);
    StartNewGame();
}

void GameField::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QBrush GameFieldBrush(QColor(141, 184, 97), Qt::SolidPattern);
    QBrush snakeBodyBrush(QColor(234, 185, 31), Qt::SolidPattern);
    QBrush snakeHeadBrush(QColor(182, 39, 220), Qt::SolidPattern);
    QBrush foodBrush(QColor(247, 103, 123), Qt::SolidPattern);
    QPainter painter;
    painter.begin(this);
    if(m_isGameOver)
    {
        painter.setPen(QColor(250, 250, 250));
        painter.setFont(QFont("Arial", 15, 700));
        painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, "You lose :(\nScore: " + QString::number(m_score));
        return;
    }
    painter.setBrush(GameFieldBrush);
    painter.setPen(QColor(50, 50, 50));
    painter.drawRect(0, 0, width()-1, height()-1);

    painter.setPen(QColor(0, 0, 0));
    painter.setBrush(snakeHeadBrush);
    painter.drawEllipse(m_snake->m_snakeBody[0]->m_x * m_snakeItemSize, m_snake->m_snakeBody[0]->m_y * m_snakeItemSize, m_snakeItemSize, m_snakeItemSize);
    painter.setBrush(snakeBodyBrush);

    for(int i = 1; i < m_snake -> m_snakeBody.size(); i++)
    {
        painter.drawEllipse(m_snake -> m_snakeBody[i] -> m_x * m_snakeItemSize, m_snake -> m_snakeBody[i] -> m_y * m_snakeItemSize, m_snakeItemSize, m_snakeItemSize);
    }
    // Малюємо їжу
    painter.setBrush(foodBrush);
    painter.drawEllipse(m_food -> m_x * m_snakeItemSize, m_food -> m_y * m_snakeItemSize, m_snakeItemSize, m_snakeItemSize);

    painter.end();
    m_isMoveBlocked = false;
}


void GameField::keyPressEvent(QKeyEvent *e)
{
    if(e -> key() == Qt::Key_Space)
    {
        if(m_isGameOver)
        {
            StartNewGame();
            return;
        }
        m_isPause = !m_isPause;
        SetGameStatus();
    }
    if(m_isMoveBlocked)
    {
        return;
    }
    if ((e->key() == Qt::Key_Left || e->key() == Qt::Key_A) && m_snake->m_snakeDirection != Snake::SnakeDirection::right)
    {
        m_snake->m_snakeDirection = Snake::SnakeDirection::left;
    }
    if ((e->key() == Qt::Key_Right || e->key() == Qt::Key_D) && m_snake->m_snakeDirection != Snake::SnakeDirection::left)
    {
        m_snake->m_snakeDirection = Snake::SnakeDirection::right;
    }
    if ((e->key() == Qt::Key_Down || e->key() == Qt::Key_S) && m_snake->m_snakeDirection != Snake::SnakeDirection::up)
    {
        m_snake->m_snakeDirection = Snake::SnakeDirection::down;
    }
    if ((e->key() == Qt::Key_Up || e->key() == Qt::Key_W) && m_snake->m_snakeDirection != Snake::SnakeDirection::down)
    {
        m_snake->m_snakeDirection = Snake::SnakeDirection::up;
    }
    m_isMoveBlocked = true;
}

void GameField::SetGameStatus()
{
    if(m_isPause)
    {
        m_moveSnakeTimer -> stop();
    }
    else
    {
        m_moveSnakeTimer -> start(m_timerInterval);
    }
}



void GameField::GameOver()
{
    m_isGameOver = true;
    QString text = "Restart - SPACE";
    emit ChangeTextSignal(text);
    m_moveSnakeTimer -> stop();
    delete m_snake;
    delete m_moveSnakeTimer;
}

void GameField::StartNewGame()
{
    bool ok;
    QStringList difficulties;
    difficulties << "Easy" << "Medium" << "Hard";
    QString difficulty = QInputDialog::getItem(this, "Select Difficulty", "Difficulty:", difficulties, 0, false, &ok);
    if (ok && !difficulty.isEmpty()) {
        if (difficulty == "Easy") {
            m_timerInterval = 200;
            m_snakeItemSize = 30;
        } else if (difficulty == "Medium") {
            m_timerInterval = 100;
            m_snakeItemSize = 20;
        } else if (difficulty == "Hard") {
            m_timerInterval = 50;
            m_snakeItemSize = 10;
        }
    } else
    {
        m_timerInterval = 100;
        m_snakeItemSize = 20;
    }

    m_fieldSize = width()/m_snakeItemSize;

    m_isPause = false;
    m_isMoveBlocked = false;
    m_isGameOver = false;
    m_snake = new Snake();
    m_food = new SnakeItem(m_fieldSize/2, m_fieldSize/2);
    m_moveSnakeTimer = new QTimer();
    connect(m_moveSnakeTimer, &QTimer::timeout, this, &GameField::MoveSnakeSlot);
    m_moveSnakeTimer -> start(m_timerInterval);
    m_score = 0;
    QString text = "Score: " + QString::number(m_score) + "\nPause - SPACE";
    emit ChangeTextSignal(text);
}

void GameField::CreateFood()
{
    m_food -> m_x = QRandomGenerator::global() -> bounded(0, m_fieldSize - 1);
    m_food -> m_y = QRandomGenerator::global() -> bounded(0, m_fieldSize - 1);
    for(int i = 0; i < m_snake -> m_snakeBody.size(); i++)
    {
        if(m_food -> m_x == m_snake -> m_snakeBody[i] -> m_x && m_food -> m_y == m_snake -> m_snakeBody[i] -> m_y)
        {
            return CreateFood();
        }
    }
}

void GameField::MoveSnakeSlot()
{
    SnakeItem *newSnakeItem;
    if(m_snake -> m_snakeDirection == Snake::SnakeDirection::right)
    {
        newSnakeItem = new SnakeItem(m_snake -> m_snakeBody[0] -> m_x + 1, m_snake -> m_snakeBody[0] -> m_y);
    }
    else if(m_snake -> m_snakeDirection == Snake::SnakeDirection::left)
    {
        newSnakeItem = new SnakeItem(m_snake -> m_snakeBody[0] -> m_x - 1, m_snake -> m_snakeBody[0] -> m_y);
    }
    else if(m_snake -> m_snakeDirection == Snake::SnakeDirection::up)
    {
        newSnakeItem = new SnakeItem(m_snake -> m_snakeBody[0] -> m_x, m_snake -> m_snakeBody[0] -> m_y - 1);
    }
    else
    {
        newSnakeItem = new SnakeItem(m_snake -> m_snakeBody[0] -> m_x, m_snake -> m_snakeBody[0] -> m_y + 1);
    }
    if (newSnakeItem->m_x >= m_fieldSize || newSnakeItem->m_x < 0 || newSnakeItem->m_y < 0 || newSnakeItem->m_y >= m_fieldSize)
    {
        GameOver();
    }
    for(int i = 0; i < m_snake -> m_snakeBody.size(); i++)
    {
        if(newSnakeItem -> m_x == m_snake -> m_snakeBody[i] -> m_x && newSnakeItem -> m_y == m_snake -> m_snakeBody[i] -> m_y)
        {
            GameOver();
        }
    }
    if (newSnakeItem -> m_x == m_food -> m_x && newSnakeItem -> m_y == m_food -> m_y)
    {
        m_score++;
        CreateFood();
        QString text = "Score: " + QString::number(m_score) + "\nPause - SPACE";
        emit ChangeTextSignal(text);

        int newInterval = m_timerInterval - (m_snake->m_snakeBody.size() * 2);
        m_moveSnakeTimer->start(qMax(newInterval, 10));
    }
    else
    {
        m_snake -> m_snakeBody.removeLast();
    }
    m_snake -> m_snakeBody.insert(0, newSnakeItem);

    repaint();
}

SnakeItem::SnakeItem(int x, int y)
{
    m_x = x;
    m_y = y;
}

Snake::Snake()
{
    m_snakeBeginSize = 4;
    for(int i = 0; i < m_snakeBeginSize; i++)
    {
        m_snakeBody.insert(0, new SnakeItem(i, 0));
    }
    m_snakeDirection = SnakeDirection::right;

}
