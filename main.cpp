#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <deque>

using namespace std;

struct PathSegment
{
    unsigned int id;
    bool visited;
    bool wall;
    vector <struct Edge*> edges;
    int distance = 1;
    int x;
    int y;
    sf::RectangleShape shape;
    Edge* backlink;
};

struct Edge
{

    PathSegment* from;
    PathSegment* to;
    int distance = 1;

};

class Grid
{
    public:
        Grid();
        ~Grid();
        vector <PathSegment*>* get_segments();
        Edge* build_edge(PathSegment* n1, PathSegment* n2);
        bool build_wall(int x, int y);
        bool bfs();
        void reset();
        const unsigned int WINX = 1057;
        const unsigned int WINY = 1057;
    protected:
        PathSegment* start;
        PathSegment* end;
        vector <PathSegment*> s;
        vector <PathSegment*> path;
        vector <Edge*> edges;
        const unsigned int numsquares = 400;
        const unsigned int ROWS = 20;
        const unsigned int COLS = 20;
        const float RX = 50;
        const float RY = 50;
};

Grid::Grid()
{

    // declare variables 
    unsigned int XPOS, YPOS;
    unsigned int i, j, id;
    PathSegment* p;
    Edge* e; 

    XPOS = 0;
    YPOS = 0;   

    s.resize(numsquares);

    // create squares and their positions
    for (i = 0; i < numsquares; i++) {
        p = new PathSegment;
        p->id = i;
        p->visited = false;
        p->wall = false;
        p->shape.setSize(sf::Vector2f(RX, RY));
        p->shape.setFillColor(sf::Color::White);
        p->shape.setPosition(XPOS, YPOS);
        p->x = XPOS;
        p->y = YPOS;
        p->backlink = NULL;
        s[i] = p;

        if (i == 0) start = p;
        if (i == numsquares - 1) end = p;

        // update XPOS and if at end of WINX then reset to 0 and increment YPOS
        XPOS += (3 + RX);
        if (XPOS >= WINX) {
            XPOS = 0;
            YPOS += (3 + RY);
        }
    }

    // set start and stop positions
    start->shape.setFillColor(sf::Color::Green);
    end->shape.setFillColor(sf::Color::Red);

    // connect all the edges
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            // connect node to right
            if (j != COLS - 1) build_edge(s[i * COLS + j], s[i * COLS + (j + 1)]);
            // connect node above
            if (i != 0) build_edge(s[i * COLS + j], s[(i-1) * COLS + j]);
            // connect node to left
            if (j != 0) build_edge(s[i * COLS + j], s[i * COLS + (j - 1)]);
            // connect node below
            if (i != (ROWS - 1)) build_edge(s[i * COLS + j], s[(i + 1) * COLS + j]);
        }
    }
    
}

// destructor 
Grid::~Grid()
{
    unsigned int i;
    for (i = 0; i < s.size(); i++) delete s[i];
    for (i = 0; i < edges.size(); i++) delete edges[i];
}

Edge* Grid::build_edge(PathSegment* n1, PathSegment* n2)
{
    Edge* e;

    e = new Edge;
    e->from = n1;
    e->to = n2;

    n1->edges.push_back(e);
    edges.push_back(e);
    
    return e;
}

// build walls
bool Grid::build_wall(int x, int y)
{   
    unsigned int i;

    for (i = 0; i < s.size(); i++)
    {
        
        if (s[i] == start || s[i] == end) continue;
        
        // check if x coordinates are in bound
        if (x >= s[i]->x && x <= s[i]->x + RX)
        {
            // check if y coordinates are in bound 
            if (y >= s[i]->y && y <= s[i]->y + RY)
            {
                if (s[i]->wall == false)
                {
                    s[i]->shape.setFillColor(sf::Color::Black);
                    s[i]->wall = true;
                    return true;
                }
                else
                {
                    s[i]->shape.setFillColor(sf::Color::White);
                    s[i]->wall = false;
                    return true;
                }
            }
        }
    }
    return false;
}

// return pointer to s
vector <PathSegment*> * Grid::get_segments()
{
    return &s; 
}

bool Grid::bfs()
{
    // declare variables
    deque <PathSegment*> q;
    vector <int> distance;
    PathSegment* n;
    int id;
    unsigned int i;

    // resize data to numsquares
    distance.resize(numsquares, 0);
    q.push_back(start);
    distance[start->id] = 0;

    while (!q.empty())
    {
        n = q[0];

        if (n->wall == false)
        {
            for (i = 0; i < n->edges.size(); i++)
            {
                if (n->edges[i]->to->visited == false && n->edges[i]->to->wall == false)
                {
                    id = n->edges[i]->to->id;
                    distance[id] = distance[n->id] + 1;
                    n->edges[i]->to->backlink = n->edges[i];
                    n->edges[i]->to->visited = true;
                    q.push_back(n->edges[i]->to);
                }
            }

        }

        n->visited = true;
        q.pop_front();
    }

    n = end;
    
    // if no end then return false
    if (end->backlink == NULL) return false;

    // push all path segments onto path
    while (n != start)
    {
        n = n->backlink->from;
        if(n != start) path.push_back(n);
    }

    // change color of final path
    for (i = 0; i < path.size(); i++) path[i]->shape.setFillColor(sf::Color::Blue);
    cout << "LENGTH " << distance[end->id] << endl;

    return true;
}

void Grid::reset()
{
    unsigned int i;

    // set path back to white
    for (i = 0; i < path.size(); i++) {
        path[i]->shape.setFillColor(sf::Color::White);
    }

    path.clear();
    
    // reset the graph
    for (i = 0; i < s.size(); i++){
        
        // if wall change back to white
        if (s[i]->wall == true) {
            s[i]->shape.setFillColor(sf::Color::White);
            s[i]->wall = false;
        }

        s[i]->backlink = NULL;
        s[i]->visited = false;
    }
}

int main()
{
    // declare variables 
    unsigned int i;
    int x, y;
    vector <PathSegment*> *p;
    Grid g;

    // create window object
    sf::RenderWindow window(sf::VideoMode(g.WINX,  g.WINY), "Breadth First Search");
    
    p = g.get_segments();

    // display and draw shapes 
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                // get mouse coordinates and check if inside box
                sf::Vector2i position = sf::Mouse::getPosition(window);
                x = position.x;
                y = position.y;
                g.build_wall(x, y);
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::S) g.bfs();
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::R) g.reset();
            }
        }

        window.clear();
        for (i = 0; i < p->size(); i++) window.draw(p->at(i)->shape);
        window.display();
    }

	return 0;
}