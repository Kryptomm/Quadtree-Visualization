#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Box.hpp"
#include "Quadtree.hpp"
#include<cstdlib>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

int main()
{
    /*
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Title");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
        }

        //clear the current Window
        window.clear();

        //Draw everything in the Window

        //Show Window
        window.display();
    }
    */

    sf::Vector2f Center1(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    Box box1 = Box(&Center1, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    Quadtree qt = Quadtree(1, box1);

    sf::Vector2f Point1(50,50);
    sf::Vector2f Point2(750, 750);

    qt.insert(&Point1);
    qt.insert(&Point2);

    std::unique_ptr<Quadtree> topLeft = qt.getTopLeft();
    std::unique_ptr<Quadtree> bottomLeft = qt.getBottomLeft();
    for (auto element : (*bottomLeft).getPoints()) {
        std::cout << element->x << ";" << element->y << " ";
    }
    std::cout << std::endl;

    return 0;
}

