#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Title");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
        }

        window.clear();

        window.display();
    }
}
