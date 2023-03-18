#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include "Box.hpp"
#include "Quadtree.hpp"


const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

int main()
{
    sf::Vector2f Center1(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    Box box = Box(&Center1, SCREEN_WIDTH, SCREEN_HEIGHT);

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "QuadTree Visualizer");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        // handle error
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    text.setPosition(0, 0);

    std::vector<Point*> points;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                sf::Vector2f newPointPos(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));

                float random_velx = (float)rand() / RAND_MAX * 4.0f - 2;
                float random_vely = (float)rand() / RAND_MAX * 4.0f - 2;

                Point* newPoint = new Point(newPointPos.x, newPointPos.y, random_velx, random_vely);

                points.push_back(newPoint);
            }
        }

        for (auto& point : points) {
            point->updatePos(&box);
        }

        Quadtree qt = Quadtree(1, box);
        for (const auto& point : points) {
            qt.insert(point);
        }

        //clear the current Window
        window.clear();

        //Draw everything in the Window
        qt.render(window);

        text.setString("Points: " + std::to_string(qt.getSize()));
        window.draw(text);

        //Show Window
        window.display();
    }

    for (const auto& point : points) {
        delete point;
    }

    return 0;
}