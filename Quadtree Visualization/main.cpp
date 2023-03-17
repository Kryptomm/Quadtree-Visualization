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

    sf::Vector2f inBoxCenter(300, 300);
    Box inBox = Box(&inBoxCenter, 500, 500);

    Quadtree qt = Quadtree(5, box);

    sf::Vector2f Point1(10, 10);
    sf::Vector2f Point2(450, 450);
    sf::Vector2f Point3(300, 300);

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

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                sf::Vector2f newPoint(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
                qt.insert(new sf::Vector2f(newPoint));
            }
        }

        //clear the current Window
        window.clear();

        //Draw everything in the Window
        qt.render(window);

        text.setString("Points: " + std::to_string(qt.getSize()));
        window.draw(text);

        std::vector<sf::Vector2f*> inBoxPoints = qt.getPointsInBox(&inBox);
        for (sf::Vector2f* p : inBoxPoints) {
            sf::CircleShape circle(5.f);
            circle.setFillColor(sf::Color::Red);
            circle.setRadius(2);
            circle.setPosition(p->x, p->y);
            window.draw(circle);
        }

        sf::RectangleShape square(sf::Vector2f(inBox.getWidth(), inBox.getHeigth()));
        square.setOutlineThickness(5);
        square.setFillColor(sf::Color::Transparent);
        square.setPosition(inBox.getCenter().x - inBox.getWidth() / 2, inBox.getCenter().y - inBox.getHeigth() / 2);
        window.draw(square);

        //Show Window
        window.display();
    }

    return 0;
}