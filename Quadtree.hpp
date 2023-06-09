#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <algorithm>
#include <queue>

class Point {
public:
    Point(double xp, double yp, double xv, double yv, double r) : x_pos(xp), y_pos(yp), x_vel(xv), y_vel(yv), radius(r) {}

    void updatePos(Box* box) {
        x_pos += x_vel;
        y_pos += y_vel;

        if (x_pos >= box->getRightBound() || x_pos <= box->getLeftBound()) {
            x_vel *= -1;
            x_pos += x_vel;
        }
        if (y_pos >= box->getLowerBound() || y_pos <= box->getUpperBound()) {
            y_vel *= -1;
            y_pos += y_vel;
        }
    }

    double getDistanceBetweenPoint(Point* other) {
        return sqrt((getPosX() - other->getPosX()) * (getPosX() - other->getPosX()) + (getPosY() - other->getPosY()) * (getPosY() - other->getPosY()));
    }

    bool isCollidingWithPoint(Point* other) {
        double distance = getDistanceBetweenPoint(other);
        return (distance <= getRadius() + other->getRadius());
    }

    void render(sf::RenderWindow& window) {
        sf::CircleShape circle(5.f);
        circle.setFillColor(sf::Color::White);
        circle.setRadius(2);
        circle.setPosition(x_pos, y_pos);
        window.draw(circle);
    }

    double getPosX() const { return x_pos; }
    double getPosY() const { return y_pos; }
    double getVelX() const { return x_vel; }
    double getvelY() const { return y_vel; }
    double getRadius() const { return radius; }

    sf::Vector2f getPos() { return sf::Vector2f(x_pos, y_pos); }
    sf::Vector2f getVel() { return sf::Vector2f(x_vel, y_vel); }

private:
    double x_pos, y_pos, x_vel, y_vel, radius;
};

class Quadtree {
public:
    Quadtree(int _maxSize, const Box& _box) : box(_box) {
        maxSize = _maxSize;
        divided = false;
    }

    Quadtree(int _maxSize, int _depth, const Box& _box) : box(_box) {
        maxSize = _maxSize;
        divided = false;
        depth = _depth;
    }

    bool insert(Point* point) {
        sf::Vector2f pos = point->getPos();
        if (!box.pointInBox(&pos)) return false;
        if (divided) {
            if (topLeft->insert(point)) return true;
            else if (topRight->insert(point)) return true;
            else if (bottomLeft->insert(point)) return true;
            else if (bottomRight->insert(point)) return true;
            return false;
        }

        points.push_back(point);
        if (points.size() > maxSize) {
            subdivide();
        }
        return true;
    }

    void subdivide() {
        divided = true;

        sf::Vector2f center = box.getCenter();
        double width = box.getWidth();
        double heigth = box.getHeigth();

        double newWidth = width / 2;
        double newHeigth = heigth / 2;

        sf::Vector2f newTopLeft(center.x - newWidth / 2, center.y + newHeigth / 2);
        sf::Vector2f newTopRight(center.x + newWidth / 2, center.y + newHeigth / 2);
        sf::Vector2f newBottomleft(center.x - newWidth / 2, center.y - newHeigth / 2);
        sf::Vector2f newBottmRight(center.x + newWidth / 2, center.y - newHeigth / 2);

        Box boxTopLeft(&newTopLeft, newWidth, newHeigth);
        Box boxTopRight(&newTopRight, newWidth, newHeigth);
        Box boxBottomLeft(&newBottomleft, newWidth, newHeigth);
        Box boxBottomRight(&newBottmRight, newWidth, newHeigth);

        topLeft = std::make_unique<Quadtree>(maxSize, depth + 1, boxTopLeft);
        topRight = std::make_unique<Quadtree>(maxSize, depth + 1, boxTopRight);
        bottomLeft = std::make_unique<Quadtree>(maxSize, depth + 1, boxBottomLeft);
        bottomRight = std::make_unique<Quadtree>(maxSize, depth + 1, boxBottomRight);

        for (const auto& p : getPoints()) {
            insert(p);
        }

        points.clear();
        return;
    }

    void render(sf::RenderWindow& window) {
        sf::RectangleShape square(sf::Vector2f(box.getWidth(), box.getHeigth()));

        square.setOutlineThickness(std::max(5 - 2 * depth, 1));
        square.setOutlineColor(sf::Color::Green);
        square.setFillColor(sf::Color::Transparent);
        square.setPosition(box.getCenter().x - box.getWidth() / 2, box.getCenter().y - box.getHeigth() / 2);

        window.draw(square);

        if (divided) {
            topLeft->render(window);
            topRight->render(window);
            bottomLeft->render(window);
            bottomRight->render(window);
        }
    }

    int getSize() {
        if (divided) {
            return topLeft->getSize() + topRight->getSize() + bottomLeft->getSize() + bottomRight->getSize();
        }
        else {
            return getPoints().size();
        }
    }

    std::vector<Point*> getPointsInBox(Box* const queryBox) {
        std::vector<Point*> pointsInBox;
        pointsInBox.reserve(points.size());

        if (!box.intersects(queryBox)) return pointsInBox;

        if (divided) {
            std::vector<Point*> children;

            children = topLeft->getPointsInBox(queryBox);
            pointsInBox.insert(pointsInBox.end(), children.begin(), children.end());

            children = topRight->getPointsInBox(queryBox);
            pointsInBox.insert(pointsInBox.end(), children.begin(), children.end());

            children = bottomLeft->getPointsInBox(queryBox);
            pointsInBox.insert(pointsInBox.end(), children.begin(), children.end());

            children = bottomRight->getPointsInBox(queryBox);
            pointsInBox.insert(pointsInBox.end(), children.begin(), children.end());
        }
        else {
            for (const auto& p : points) {
                sf::Vector2f pos = p->getPos();
                if (queryBox->pointInBox(&pos)) {
                    pointsInBox.emplace_back(p);
                }
            }
        }
        return pointsInBox;
    }

    std::vector<Point*> getPointsInBoxIte(Box* const queryBox) {
        std::vector<Point*> pointsInBox;

        if (!divided) {
            for (const auto& p : points) {
                sf::Vector2f pos = p->getPos();
                if (queryBox->pointInBox(&pos)) {
                    pointsInBox.emplace_back(p);
                }
            }
            return pointsInBox;
        }

        std::queue<Quadtree*> queue;
        queue.push(topLeft.get());
        queue.push(topRight.get());
        queue.push(bottomLeft.get());
        queue.push(bottomRight.get());

        while (!queue.empty()) {
            Quadtree* currentTree = queue.front();
            queue.pop();

            if (currentTree->isDivided()) {
                if (currentTree->getBox().intersects(queryBox)) {
                    queue.push(currentTree->topLeft.get());
                    queue.push(currentTree->topRight.get());
                    queue.push(currentTree->bottomLeft.get());
                    queue.push(currentTree->bottomRight.get());

                }
            }
            else {
                for (const auto& p : currentTree->getPoints()) {
                    sf::Vector2f pos = p->getPos();
                    if (queryBox->pointInBox(&pos)) {
                        pointsInBox.emplace_back(p);
                    }
                }
            }
        }
        return pointsInBox;
    }


    std::vector<Box*> getBoxes() {
        std::vector<Box*> boxes;

        boxes.push_back(&box);

        if (!divided) return boxes;

        std::vector<Box*> children;
        children = topLeft->getBoxes();
        boxes.insert(boxes.end(), children.begin(), children.end());

        children = topRight->getBoxes();
        boxes.insert(boxes.end(), children.begin(), children.end());

        children = bottomLeft->getBoxes();
        boxes.insert(boxes.end(), children.begin(), children.end());

        children = bottomRight->getBoxes();
        boxes.insert(boxes.end(), children.begin(), children.end());

        return boxes;
    }

    std::vector<Point*> getPoints() { return points; }
    Box getBox() { return box; }
    bool isDivided() { return divided; }


private:
    int maxSize;
    int depth = 0;

    bool divided;

    Box box;

    std::vector<Point*> points;
    std::unique_ptr<Quadtree> topLeft;
    std::unique_ptr<Quadtree> topRight;
    std::unique_ptr<Quadtree> bottomLeft;
    std::unique_ptr<Quadtree> bottomRight;
};