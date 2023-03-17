#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <algorithm>

class Quadtree {
public:
    Quadtree(int _maxSize, const Box& _box) : box(_box){
        maxSize = _maxSize;
        divided = false;
    }

    Quadtree(int _maxSize, int _depth,const Box& _box) : box(_box) {
        maxSize = _maxSize;
        divided = false;
        depth = _depth;
    }

    bool insert(sf::Vector2f* point) {
        if (!box.pointInBox(point)) return false;
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

        for (sf::Vector2f* p : points) {
            insert(p);
        }

        points.clear();
        return;
    }

    void render(sf::RenderWindow& window) {
        sf::RectangleShape square(sf::Vector2f(box.getWidth(), box.getHeigth()));

        square.setOutlineThickness(std::max(5-2*depth,1));

        if (depth == 0) square.setOutlineColor(sf::Color::Red);
        else square.setOutlineColor(sf::Color::Green);
        
        square.setFillColor(sf::Color::Transparent);
        square.setPosition(box.getCenter().x - box.getWidth()/2, box.getCenter().y - box.getHeigth() / 2);

        window.draw(square);

        if (divided) {
            topLeft->render(window);
            topRight->render(window);
            bottomLeft->render(window);
            bottomRight->render(window);
        }
        else {
            for (sf::Vector2f* p: points) {
                sf::CircleShape circle(5.f);
                circle.setFillColor(sf::Color::White);
                circle.setRadius(2);
                circle.setPosition(p->x, p->y);
                window.draw(circle);
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

    std::vector<sf::Vector2f*> getPointsInBox(Box* queryBox) {
        std::vector<sf::Vector2f*> pointsInBox;

        if (!box.intersects(queryBox)) return pointsInBox;

        if (divided) {
            std::vector<sf::Vector2f*> childPointsTopLeft = topLeft->getPointsInBox(queryBox);
            pointsInBox.insert(pointsInBox.end(), childPointsTopLeft.begin(), childPointsTopLeft.end());

            std::vector<sf::Vector2f*> childPointsTopRight = topRight->getPointsInBox(queryBox);
            pointsInBox.insert(pointsInBox.end(), childPointsTopRight.begin(), childPointsTopRight.end());

            std::vector<sf::Vector2f*> childPointsBottomLeft = bottomLeft->getPointsInBox(queryBox);
            pointsInBox.insert(pointsInBox.end(), childPointsBottomLeft.begin(), childPointsBottomLeft.end());

            std::vector<sf::Vector2f*> childPointsBottomRight = bottomRight->getPointsInBox(queryBox);
            pointsInBox.insert(pointsInBox.end(), childPointsBottomRight.begin(), childPointsBottomRight.end());
        }
        else {
            for (sf::Vector2f* p : points) {
                if (queryBox->pointInBox(p)) {
                    pointsInBox.push_back(p);
                }
            }
        }
        return pointsInBox;
    }

	std::vector<sf::Vector2f*> getPoints() { return points; }
private:
	int maxSize;
    int depth = 0;

	bool divided;

    Box box;

    std::vector<sf::Vector2f*> points;
    std::unique_ptr<Quadtree> topLeft;
    std::unique_ptr<Quadtree> topRight;
    std::unique_ptr<Quadtree> bottomLeft;
    std::unique_ptr<Quadtree> bottomRight;
};