#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <tuple>
#include "Box.hpp"
#include "Quadtree.hpp"


const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;
const int BRUSH_SIZE = 100;

//Define the Size of the Quadtree
sf::Vector2f Center1(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
Box box = Box(&Center1, SCREEN_WIDTH, SCREEN_HEIGHT);

void updatePoints(std::vector<Point*> points, int start, int end) {
    for (int i = start; i < end; ++i) {
        points[i]->updatePos(&box);
    }
}

std::tuple<std::vector<Point*>,int> optCheckCollisions(std::vector<Point*> points, Quadtree* quadtree) {
    std::vector<Point*> collidedPoints;
    int checks = 0;


    for (Point* point : points) {
        sf::Vector2f center = point->getPos();
        Box box = Box(&center, 2 * 5 + 1, 2 * 5 + 1);

        std::vector<Point*> pointsInBoxRange(quadtree->getPointsInBox(&box));

        for (Point* otherPoint : pointsInBoxRange) {
            if (point != otherPoint) {
                if (point->isCollidingWithPoint(otherPoint)) {
                    collidedPoints.push_back(point);
                }
                checks++;
            }
        }

    }
    return std::make_tuple(collidedPoints, checks);
}

std::tuple<std::vector<Point*>, int> CheckCollisions(std::vector<Point*> points, Quadtree* quadtree) {
    std::vector<Point*> collidedPoints;
    int checks = 0;

    for (int i = 0; i < points.size(); i++) {
        for(int j = i + 1; j < points.size(); j++){
            if (points[i]->isCollidingWithPoint(points[j])) {
                collidedPoints.push_back(points[i]);
                collidedPoints.push_back(points[j]);
            }
            checks++;
        }
    }
    return std::make_tuple(collidedPoints, checks);
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
        std::cout << "Please insert arial.tff into this folger" << std::endl;
    }

    //Text Settings
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);

    sf::Text pointsText = text;
    pointsText.setPosition(0, 0);

    sf::Text fpsText = text;
    fpsText.setPosition(0, 24);

    sf::Text collisionChecksText = text;
    collisionChecksText.setPosition(0, 48);

    sf::Text optCollisionChecksText = text;
    optCollisionChecksText.setPosition(0, 72);

    sf::Text collisionsText = text;
    collisionsText.setPosition(0, 96);

    //Saves all Points to a list that can be used to build new Quadtrees
    std::vector<Point*> points;

    //Generate the first Point and initalize it as the Querypoint
    srand(time(nullptr));
    int random_posx = rand() % SCREEN_WIDTH;
    int random_posy = rand() % SCREEN_HEIGHT;
    float random_velx = (float)rand() / RAND_MAX * 4.0f - 2;
    float random_vely = (float)rand() / RAND_MAX * 4.0f - 2;

    Point* queryPoint = new Point(random_posx, random_posy, random_velx, random_vely, 0);

    points.push_back(queryPoint);

    Quadtree qt = Quadtree(5, box);
    for (const auto& point : points) {
        qt.insert(point);
    }

    //Calculation Settings
    const int batch_size = 100;

    //Window Loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();

            //Create a new Points on click
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                for (int i = 0; i < BRUSH_SIZE; i++) {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

                    float random_velx = (float)rand() / RAND_MAX * 4.0f - 2;
                    float random_vely = (float)rand() / RAND_MAX * 4.0f - 2;
                    float random_radius = (float)rand() / RAND_MAX * 4.0f + 1;

                    Point* newPoint = new Point(mousePosition.x, mousePosition.y, random_velx, random_vely, random_radius);

                    points.push_back(newPoint);
                }
            }
        }

        //Update all Positions of the Points based on their Velocity.
        //Settings for updating Positions
        const int num_batches = (points.size() + batch_size - 1) / batch_size;

        std::vector<std::thread> threads;
        for (int i = 0; i < num_batches; i++) {
            int start = i * batch_size;
            int end = std::min(start + batch_size, static_cast<int>(points.size()));
            threads.push_back(std::thread(updatePoints, std::ref(points), start, end));
        }

        for (auto& thread : threads) {
            thread.join();
        }

        //Create a new Quadtree based on the new Positions of the points.
        qt = Quadtree(5, box);
        for (const auto& point : points) {
            qt.insert(point);
        }

        //Check for Collisions
        std::vector<Point*> collidingPoints;
        int calculationsCount = 0;

        //auto start = std::chrono::high_resolution_clock::now();

        tie(collidingPoints, calculationsCount) = optCheckCollisions(points, &qt);
        //tie(collidingPoints, calculationsCount) = CheckCollisions(points, &qt);

        //auto end = std::chrono::high_resolution_clock::now();
        //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        //std::cout << "Time taken by code: " << duration.count()/1000 << " ms." << std::endl;


        //clear the current Window
        window.clear();

        //Draw everything in the Window

        /*
        Drawing the Boxes of the Quadtree by adding Vertices in a Vertexarray that are going
        from TL -> TR, TR -> BR, BR -> BL, BL -> TL.
        */
        std::vector<Box*> boxes = qt.getBoxes();
        sf::VertexArray lines(sf::Lines, boxes.size() * 8);

        sf::Color lineColor(0, 255, 0);
        for (int i = 0; i < boxes.size(); i++) {
            Box* box = boxes[i];
            int leftBound = box->getLeftBound();
            int rightBound = box->getRightBound();
            int upperBound = box->getUpperBound();
            int lowerBound = box->getLowerBound();

            lines[8 * i + 0] = sf::Vertex(sf::Vector2f(leftBound, upperBound), lineColor);
            lines[8 * i + 1] = sf::Vertex(sf::Vector2f(rightBound, upperBound), lineColor);

            lines[8 * i + 2] = sf::Vertex(sf::Vector2f(rightBound, upperBound), lineColor);
            lines[8 * i + 3] = sf::Vertex(sf::Vector2f(rightBound, lowerBound), lineColor);

            lines[8 * i + 4] = sf::Vertex(sf::Vector2f(rightBound, lowerBound), lineColor);
            lines[8 * i + 5] = sf::Vertex(sf::Vector2f(leftBound, lowerBound), lineColor);

            lines[8 * i + 6] = sf::Vertex(sf::Vector2f(leftBound, lowerBound), lineColor);
            lines[8 * i + 7] = sf::Vertex(sf::Vector2f(leftBound, upperBound), lineColor);
        }

        window.draw(lines);

        //Draw Points
        sf::VertexArray vertices(sf::Points);
        
        for (int i = 0; i < points.size(); i++) {
            Point* point = points[i];
            sf::Color color(sf::Color::White);
            sf::Vertex vertex(sf::Vector2f(point->getPosX(), point->getPosY()), color);
            vertices.append(vertex);
            float radius = point->getRadius();
            for (int j = 0; j <= 360; j += 40) {
                float angle = static_cast<float>(j) * 3.14159f / 180.f;
                float px = point->getPosX() + std::cos(angle) * radius;
                float py = point->getPosY() + std::sin(angle) * radius;
                vertices.append(sf::Vertex(sf::Vector2f(px, py), color));
            }
        }
        window.draw(vertices);

        //Text Modifications
        int normalCollisionChecks = points.size() * (points.size() - 1) / 2;

        pointsText.setString("Points: " + std::to_string(points.size()));
        collisionChecksText.setString("Normal Checks: " + std::to_string(normalCollisionChecks));
        optCollisionChecksText.setString("opt Checks: " + std::to_string(calculationsCount));
        collisionsText.setString("Collisions: " + std::to_string(collidingPoints.size() / 2));

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
        Box queryBox2(&queryCenter, 200, 200);
        pointsInBox = qt.getPointsInBox(&queryBox2);

        sf::VertexArray verticesInBox(sf::Points);
        for (int i = 0; i < pointsInBox.size(); i++) {
            Point* point = pointsInBox[i];
            sf::Color color(sf::Color::Red);
            sf::Vertex vertex(sf::Vector2f(point->getPosX(), point->getPosY()), color);
            verticesInBox.append(vertex);
            float radius = point->getRadius();
            for (int j = 0; j <= 360; j += 10) {
                float angle = static_cast<float>(j) * 3.14159f / 180.f;
                float px = point->getPosX() + std::cos(angle) * radius;
                float py = point->getPosY() + std::sin(angle) * radius;
                verticesInBox.append(sf::Vertex(sf::Vector2f(px, py), color));
            }
        }
        window.draw(verticesInBox);

        //Draw Text
        window.draw(pointsText);
        window.draw(fpsText);
        window.draw(collisionChecksText);
        window.draw(optCollisionChecksText);
        window.draw(collisionsText);

        //Show Window
        window.display();

        frame_count++;
        if (clock.getElapsedTime().asSeconds() >= 1.0f) {
            fpsText.setString("FPS: " + std::to_string(frame_count));
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