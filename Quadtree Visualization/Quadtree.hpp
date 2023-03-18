#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <algorithm>

class Point {
public:
    Point(double xp, double yp, double xv, double yv) : x_pos(xp), y_pos(yp), x_vel(xv), y_vel(yv) {}

    void updatePos(Box* box) {
        x_pos += x_vel;
        y_pos += y_vel;

        if (x_pos >= box->getReightBound() || x_pos <= box->getLeftBound()) {
            x_vel *= -1;
            x_pos += x_vel;
        }
        if (y_pos >= box->getLowerBound() || y_pos <= box->getUpperBound()) {
            y_vel *= -1;
            y_pos += y_vel;
        }
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

    sf::Vector2f getPos() { return sf::Vector2f(x_pos, y_pos); }
    sf::Vector2f getVel() { return sf::Vector2f(x_vel, y_vel); }

private:
    double x_pos, y_pos, x_vel, y_vel;
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
            if (topRight->insert(point)) return true;
            if (bottomLeft->insert(point)) return true;
            if (bottomRight->insert(point)) return true;
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
        else {
            for (const auto& p : getPoints()) {
                p->render(window);
            }
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

    std::vector<Point*> getPointsInBox(Box* queryBox) {
        std::vector<Point*> pointsInBox;

        if (!box.intersects(queryBox)) return pointsInBox;

        if (divided) {
            std::vector<Point*> childPointsTopLeft = topLeft->getPointsInBox(queryBox);
            pointsInBox.insert(pointsInBox.end(), childPointsTopLeft.begin(), childPointsTopLeft.end());

            std::vector<Point*> childPointsTopRight = topRight->getPointsInBox(queryBox);
            pointsInBox.insert(pointsInBox.end(), childPointsTopRight.begin(), childPointsTopRight.end());

            std::vector<Point*> childPointsBottomLeft = bottomLeft->getPointsInBox(queryBox);
            pointsInBox.insert(pointsInBox.end(), childPointsBottomLeft.begin(), childPointsBottomLeft.end());

            std::vector<Point*> childPointsBottomRight = bottomRight->getPointsInBox(queryBox);
            pointsInBox.insert(pointsInBox.end(), childPointsBottomRight.begin(), childPointsBottomRight.end());
        }
        else {
            for (const auto& p : points) {
                sf::Vector2f pos = p->getPos();
                if (queryBox->pointInBox(&pos)) {
                    pointsInBox.push_back(p);
                }
            }
        }
        return pointsInBox;
    }

    std::vector<Point*> getPoints() { return points; }

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