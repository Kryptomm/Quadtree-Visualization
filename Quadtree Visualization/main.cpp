#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <thread>
#include "Box.hpp"
#include "Quadtree.hpp"


const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;
const int BRUSH_SIZE = 10;

//Define the Size of the Quadtree
sf::Vector2f Center1(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
Box box = Box(&Center1, SCREEN_WIDTH, SCREEN_HEIGHT);

void updatePoints(std::vector<Point*> points, int start, int end) {
    for (int i = start; i < end; ++i) {
        points[i]->updatePos(&box);
    }
}

int main()
{
    //Window Settings
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "QuadTree Visualizer");
    window.setFramerateLimit(60);
    sf::Clock clock;
    int frame_count = 0;

    //load Font
    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        // handle error
    }

    //Text Settings
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    text.setPosition(0, 0);

    //Saves all Points to a list that can be used to build new Quadtrees
    std::vector<Point*> points;

    //Generate the first Point and initalize it as the Querypoint
    srand(time(nullptr));
    int random_posx = rand() % SCREEN_WIDTH;
    int random_posy = rand() % SCREEN_HEIGHT;
    float random_velx = (float)rand() / RAND_MAX * 4.0f - 2;
    float random_vely = (float)rand() / RAND_MAX * 4.0f - 2;

    Point* queryPoint = new Point(random_posx, random_posy, random_velx, random_vely);

    points.push_back(queryPoint);

    Quadtree qt = Quadtree(10, box);
    for (const auto& point : points) {
        qt.insert(point);
    }

    //Window Loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();

            //Create a new Point on click
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                for (int i = 0; i < BRUSH_SIZE; i++) {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

                    float random_velx = (float)rand() / RAND_MAX * 4.0f - 2;
                    float random_vely = (float)rand() / RAND_MAX * 4.0f - 2;

                    Point* newPoint = new Point(mousePosition.x, mousePosition.y, random_velx, random_vely);

                    points.push_back(newPoint);
                }
            }
        }

        //Update all Positions of the Points based on their Velocity.
        //Settings for updating Positions
        const int batch_size = 100;
        const int num_batches = (points.size() + batch_size - 1) / batch_size;

        std::vector<std::thread> threads;
        for (int i = 0; i < num_batches; ++i) {
            int start = i * batch_size;
            int end = std::min(start + batch_size, static_cast<int>(points.size()) - 1);
            threads.push_back(std::thread(updatePoints, std::ref(points), start, end));
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        

        //Create a new Quadtree based on the new Positions of the points.
        qt = Quadtree(10, box);
        for (const auto& point : points) {
            qt.insert(point);
        }

        //clear the current Window
        window.clear();

        //Draw everything in the Window
        qt.render(window);

        //Draw Points
        sf::VertexArray vertices(sf::Points);
        float radius = 2;
        for (int i = 0; i < points.size(); i++) {
            sf::Color color(sf::Color::White);
            sf::Vertex vertex(sf::Vector2f(points[i]->getPosX(), points[i]->getPosY()), color);
            vertices.append(vertex);
        }
        window.draw(vertices);

        //Draw Querybox
        sf::Vector2f queryBox(200, 200);
        sf::RectangleShape square(queryBox);
        square.setOutlineThickness(3);
        square.setOutlineColor(sf::Color::Red);
        square.setFillColor(sf::Color::Transparent);
        square.setPosition(queryPoint->getPosX() - queryBox.x / 2, queryPoint->getPosY() - queryBox.y / 2);
        window.draw(square);

        //Querybox Draw Points in Box
        std::vector<Point*> pointsInBox;
        sf::Vector2f queryCenter(queryPoint->getPosX(), queryPoint->getPosY());
        Box queryBox2(&queryCenter, 200,200);

        pointsInBox = qt.getPointsInBox(&queryBox2);

        for (auto& point : pointsInBox) {
            sf::CircleShape circle(5.f);
            circle.setFillColor(sf::Color::Red);
            circle.setRadius(2);
            circle.setPosition(point->getPosX(), point->getPosY());
            window.draw(circle);
        }

        //Draw Text
        text.setString("Points: " + std::to_string(points.size()));
        window.draw(text);

        //Show Window
        window.display();

        frame_count++;
        if (clock.getElapsedTime().asSeconds() >= 1.0f) {
            std::cout << "FPS: " << frame_count << std::endl;
            frame_count = 0;
            clock.restart();
        }

    }

    //Free Memory of the Points
    for (const auto& point : points) {
        delete point;
    }

    return 0;
}