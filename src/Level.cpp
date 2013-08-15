#include <fstream>
#include "Ncurses.hpp" // intToString()
#include "Level.hpp"

Level::Level()
{
    this->clear();
}
Level::~Level()
{ }
bool Level::load(std::string filename)
{
    this->clear();

    this->filename = filename;

    std::ifstream file(this->filename.c_str());
    if (!(file.is_open()))
        return false;

//    if (!(file.good())) what does it do?

    std::string line("");
    bool levelStarted(false);
    unsigned int levelWidth(0);

    // Parsing each line of the file
    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        if ((line[0] == '#') && (!levelStarted))
            continue; // ignoring comments

        // Getting the level metadata.
        // Finding it's title and getting the string right after
        // it until the end of line.

        std::size_t pos = line.find("name=");
        if (pos != std::string::npos)
            this->name = line.substr(pos + 5, std::string::npos);

        pos = line.find("author=");
        if (pos != std::string::npos)
            this->author = line.substr(pos + 7, std::string::npos);

        pos = line.find("date=");
        if (pos != std::string::npos)
            this->date = line.substr(pos + 5, std::string::npos);

        pos = line.find("comment=");
        if (pos != std::string::npos)
            this->comment = line.substr(pos + 8, std::string::npos);

        // actual level parsing
        if (line == "start")
        {
            levelStarted = true;
            continue;
        }
        if (line == "end")
        {
            levelStarted = false;
            continue;
        }

        if (levelStarted)
        {
            this->rawLevel.push_back(line);

            if (line.length() > levelWidth) // maximum width
                levelWidth = line.length();

            // char-by-char level parsing
            std::vector<Tile::TileContents> levelLine;

            for (unsigned int i = 0; i < (line.size()); i++)
            {
                char c = line[i];

                switch (c)
                {
                    // multiple ways of defining borders
                case '+':
                case '-':
                case '|':
                    levelLine.push_back(Tile::BORDER);
                    break;

                case ' ':
                    levelLine.push_back(Tile::NOTHING);
                    break;

                case '#':
                    levelLine.push_back(Tile::WALL);
                    break;

                case 'S':
                    // get snake starting position
                    this->startingX = i;

                    // Y of the current line is the rawLevel size
                    // minus 1 because we'll pretty soon add it to
                    // rawLevel and it's size will increase by 1.
                    this->startingY = (this->rawLevel.size() - 1);

                    // note that multiple "S" on the file
                    // will always fall back to the last one

                    levelLine.push_back(Tile::NOTHING);
                    break;

                default:
                    // ignoring strange things, good for future
                    // versions.
                    levelLine.push_back(Tile::NOTHING);
                    break;
                }
            }

            // Adding the parsed line to the parsed level container
            this->level.push_back(levelLine);

        } // if (levelStarted)
    } // while (std::getline(file, line))

    if (!(file.eof())) // Finished reading due to some error
        return false;

    this->width  = levelWidth;
    this->height = this->rawLevel.size();

    return true;
}
void Level::clear()
{
    this->filename = "";
    this->rawLevel.clear();

    for (unsigned int i = 0; i < (this->level.size()); i++)
        this->level[i].clear();

    this->level.clear();

    this->width = 0;
    this->height = 0;

    this->startingX = 1; // by default starting the player at
    this->startingY = 1; // position (1, 1) on the level

    this->name = "";
    this->author = "";
    this->date = "";
    this->comment = "";
}
int Level::getWidth()
{
    return (this->width);
}
int Level::getHeight()
{
    return (this->height);
}
Tile::TileContents& Level::at(int x, int y)
{
    if ((x < 0) || (x >= this->width) ||
        (y < 0) || (y >= this->height))
    {
        throw "Level::at() Access to invalid index " +
            Ncurses::intToString(x) +
            ", " +
            Ncurses::intToString(y);
    }

    int a;
    a = x; // we need to switch them
    x = y; // because when we build the level matrix
    y = a; // we made it on reverse:
           // this->level[] are y and
           // this->level[][] are x

    return (this->level[x][y]);
}
int Level::getStartingX()
{
    return (this->startingX);
}
int Level::getStartingY()
{
    return (this->startingY);
}
