#include "GameStateGame.hpp"
#include "Input.hpp"
#include "Ncurses.hpp"
#include "Config.hpp"

GameStateGame::GameStateGame():
    player(NULL),
    board(NULL),
    gameTimer(NULL),
    foods(NULL)
{ }
GameStateGame::~GameStateGame()
{ }
void GameStateGame::load(int stack)
{
    UNUSED(stack);

    this->boardX = 0;
    this->boardY = 1;
    this->board = new Board(80, 23);

    bool result = this->board->loadFile("levels/00.nsnake");
    if (!result)
        throw "GameStateGame: Couldn't load the level.";

    if (this->board->getSupportedPlayers() > 1)
        throw "GameStateGame: Not a single-player level.";

    if (Config::centerGameScreenHorizontally)
        this->boardX = Ncurses::currentWidth/2 - this->board->getWidth()/2;

    if (Config::centerGameScreenVertically)
        this->boardY = Ncurses::currentHeight/2 - this->board->getHeight()/2;

    this->foods = new FoodManager(this->board);
    this->foods->addAtRandom();

    this->player = new Snake(this->board);
    this->player->setKeys('w', 's', 'a', 'd');

    // Again, this is the game speed.
    // This is the timer that says when the snake will be
    // updated.
    this->gameTimer = new TimerCounter(this->board->getTimeout());
    this->gameTimer->startCounting();
}
int GameStateGame::unload()
{
// Time to delete!
// This macro deletes a thing only if it's non-NULL,
// making sure it won't double-delete things.
#define safe_delete(x) \
{                      \
    if (x)             \
    {                  \
        delete (x);    \
        x = NULL;      \
    }                  \
}

    safe_delete(this->player);
    safe_delete(this->board);
    return 0;
}
GameState::StateCode GameStateGame::update(float dt)
{
    UNUSED(dt);

    Input* input = Input::getInstance();
    input->update();

    if (input->isKeyDown('q')) // quit the game
        return GameState::QUIT;

    if (input->isKeyDown('r')) // restart the game!
        return GameState::GAME_START;

    if (input->isKeyDown('i')) // increase the player
        this->player->eatFood();

    if (input->isKeyDown('u'))
    {
        this->board->increaseSpeed();
        this->gameTimer->setDelay(this->board->getTimeout());
        this->gameTimer->startCounting();
    }

//    this->foods->update();
    this->player->update();

    // If the game speed time has passed, we'll force
    // the snake to move.
    //
    // Note that we always check for input on Snake::update(),
    // so don't worry about that.
    if (this->gameTimer->isDone())
    {
        this->player->move();
        this->player->checkCollision();

        this->foods->update();
        this->gameTimer->startCounting();
    }

    return GameState::CONTINUE;
}
void GameStateGame::render()
{
    std::string logo("nSnake v2.0");

    Ncurses::setStyle(Color::pair("cyan"));
    Ncurses::print(logo, 0, 0);

    std::string speed("speed");
    int speedX = (Ncurses::currentWidth/8);

    Ncurses::print(speed, speedX, 0);
    Ncurses::print(Ncurses::intToString(this->board->getSpeed()),
                   speedX + 3 + speed.length(), 0);

    std::string score("score");
    int scoreX = (Ncurses::currentWidth/8 * 2);

    Ncurses::print(score, scoreX, 0);
    Ncurses::print(Ncurses::intToString(this->player->getScore()),
                   scoreX + 3 + score.length(), 0);

    std::string level("level");
    int levelX = (Ncurses::currentWidth/8 * 3);

    Ncurses::print(level, levelX, 0);
    Ncurses::print(this->board->getLevelName(),
                   levelX + 3 + level.length(), 0);

    std::string info("| <q> quit | <r> restart |");
    int infoX = Ncurses::currentWidth - info.length();

    Ncurses::setStyle(Color::pair("magenta"));
    Ncurses::print(info, infoX, 0);

    this->board->render(this->boardX, this->boardY);
}

