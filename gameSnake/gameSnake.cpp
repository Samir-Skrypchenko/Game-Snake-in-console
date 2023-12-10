#include <iostream>
#include <conio.h>
#include <thread>
#include <chrono>

const int W = 50;
const int H = W / 2;
char ch = 'w';

struct pos { int x; int y; };


class Apple;

void input(char& ch);


class Map
{
    private:
        char map[H][W];
    public:
        Map()
        {
            for (int y = 0; y < H; y++)
            {
                if (y == 0 || y == H - 1)
                {
                    for (int x = 0; x < W; x++)
                    {
                        map[y][x] = '#';
                    }
                }
                else
                {
                    map[y][0] = '#';
                    for (int x = 1; x < W - 1; x++)
                    {
                        map[y][x] = ' ';
                    }
                    map[y][W - 1] = '#';
                }
            }
        }

        void mapDraw() const
        {
            for (int y = 0; y < H; y++)
            {
                for (int x = 0; x < W; x++)
                {
                    std::cout << map[y][x];
                }
                std::cout << std::endl;
            }
        }

        char& getCharMap(const pos position) { return map[position.y][position.x]; } // Возвращаем ссылку элемента карты
                                                                                     // который находится на координатах (x ; y)
};


class Snake
{

    private:
        enum direction { RD, LU };
        direction dir;
        bool horizontalMove;
        pos tempPos; // переменная сохраняющая позицию последней части змейки перед тем как изменить её позицию
    private:
        static const int MAX_LENGTH = 50;
        int length;
        char sym;
        pos positions[MAX_LENGTH];
    public:
        Snake() : length(1), horizontalMove(false), dir(LU), sym('O') { positions[0] = { W / 2, H / 2 }; }

        void draw(Map& map) const
        {
            for (int i = 0; i < length; i++)
            {
                map.getCharMap(positions[i]) = sym;
            }
        }
        void move(Map& map)
        {
            map.getCharMap(positions[length - 1]) = ' '; // убираем с консоли последнюю лишнюю часть тела
            tempPos = positions[length - 1]; // сохраняем позицию последней части тела змейки
            for (int i = length - 1; i > 0; i--) //присваиваем i-й части тела змейки координаты (i-1)-й части змейки (0-я часть тела это голова)
            {
                positions[i] = positions[i - 1];
            }

            if (horizontalMove)
            {
                switch (ch)
                {
                    case 'd':
                        if (positions[0].x >= (W - 2))
                        {
                            positions[0].x = 1;
                            break;
                        }
                        positions[0].x++;
                        horizontalMove = false;
                        dir = RD;
                        break;
                    case 'a':
                        if (positions[0].x <= 1)
                        {
                            positions[0].x = W - 2;
                            break;
                        }
                        positions[0].x--;
                        horizontalMove = false;
                        dir = LU;
                        break;
                    default:
                        if (dir == LU)
                        {
                            if (positions[0].y <= 1)
                            {
                                positions[0].y = H - 2;
                                break;
                            }
                            positions[0].y--;
                            break;
                        }

                        if (positions[0].y >= (H - 2))
                        {
                            positions[0].y = 1;
                            break;
                        }
                        positions[0].y++;
                        break;
                }
            }
            else
            {
                switch (ch)
                {
                    case 'w':
                        if (positions[0].y <= 1)
                        {
                            positions[0].y = H - 2;
                            break;
                        }
                        positions[0].y--;
                        horizontalMove = true;
                        dir = LU;
                        break;
                    case 's':
                        if (positions[0].y >= (H - 2))
                        {
                            positions[0].y = 1;
                            break;
                        }
                        positions[0].y++;
                        horizontalMove = true;
                        dir = RD;
                        break;
                    default:
                        if (dir == LU)
                        {
                            if (positions[0].x <= 1)
                            {
                                positions[0].x = W - 2;
                                break;
                            }
                            positions[0].x--;
                            break;
                        }

                        if (positions[0].x >= (W - 2))
                        {
                            positions[0].x = 1;
                            break;
                        }
                        positions[0].x++;
                        horizontalMove = false;
                        dir = RD;
                        break;
                }
            }
        }
        void initNewPos() { positions[length - 1] = tempPos; }
        
        bool checkCollisionWithBody() const
        {
            for (int i = 0; i < length - 1; i++)
            {
                if (positions[0].x == positions[i + 1].x && positions[0].y == positions[i + 1].y)
                {
                    return true;
                }
            }

            return false;
        }

        // методы для работы со значениями полей из других классов
        int returnLength() const { return length; }
        pos getPosition(const int index) const { return positions[index]; }

        // объявления
        void checkEatingApple(Apple& apple);
};


class Apple
{
    private:
        pos position;
        char sym;
    public:
        Apple() : sym('Q') { position = { W / 2 - 3, H / 2 - 4 }; }

        void draw(Map& map) { map.getCharMap(position) = sym; }

        bool checkCollisionWithSnake(const Snake& snake) const
        {
            for (int i = 0; i < snake.returnLength(); i++)
            {
                if (position.x == snake.getPosition(i).x && position.y == snake.getPosition(i).y)
                {
                    return true;
                }
            }
            return false;
        }

        bool checkCollisionWithWall() const
        {
            if (position.y == 0 || position.y == (H - 1)) { return true; }
            else if (position.x == 0 || position.x == (W - 1)) { return true; }
            return false;
        }

        void spawn(const Snake& snake)
        {
            srand(time(NULL));
            do
            {
                position = { rand() % W, rand() % H };
            } while (checkCollisionWithSnake(snake) || checkCollisionWithWall());
        }

        pos getPosition() const { return position; }
};


class Game
{
    private:

    public:
        void gameLoop() const
        {
            Map map;
            Snake snake;
            Apple apple;

            while (!snake.checkCollisionWithBody())
            {
                snake.checkEatingApple(apple);
                snake.draw(map);
                apple.draw(map);
                map.mapDraw();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                snake.move(map);
                std::system("cls");
            }
        }
};


int main()
{
    std::thread thInput(input, std::ref(ch));
    thInput.detach();
    Game game;
    game.gameLoop();
}

void input(char& ch)
{
    while (1) { ch = _getch(); std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
}

void Snake::checkEatingApple(Apple& apple)
{
    if (apple.getPosition().x == positions[0].x && apple.getPosition().y == positions[0].y)
    {
        if (length < MAX_LENGTH)
        {
            length++;
            initNewPos();
        }
        apple.spawn(*this);
    }
}

