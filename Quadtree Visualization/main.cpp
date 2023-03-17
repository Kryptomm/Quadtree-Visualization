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
    sf::Vector2f Center1(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    Box box = Box(&Center1, SCREEN_WIDTH, SCREEN_HEIGHT);

    Quadtree qt = Quadtree(1, box);

    sf::Vector2f Point1(10, 10);
    sf::Vector2f Point2(450, 450);
    sf::Vector2f Point3(300, 300);

    qt.insert(&Point1);
    qt.insert(&Point2);
    qt.insert(&Point3);

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH+15, SCREEN_HEIGHT+15), "QuadTree Visualizer");
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
        qt.render(window);

        //Show Window
        window.display();
    }

    return 0;
}

