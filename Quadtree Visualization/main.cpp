#include <SFML/Graphics.hpp>
#include <iostream>
#include "Box.hpp"
#include "Quadtree.hpp"

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

    sf::Vector2f Center1(0,0);

    Box box1 = Box(&Center1, 2, 2);
    std::cout << box1 << std::endl;
    
    sf::Vector2f Center2(2,2);

    Box box2 = Box(&Center2, 2, 2);
    std::cout << box2 << std::endl;

    sf::Vector2f Point(-5, -5);

    std::cout << box1.intersects(&box2) << std::endl;
    std::cout << box1.pointInBox(&Point) << std::endl;
    return 0;
}

