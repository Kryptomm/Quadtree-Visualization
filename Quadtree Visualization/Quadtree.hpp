#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Quadtree {
public:
    Quadtree(int _maxSize, const Box& _box) : box(_box){
        maxSize = _maxSize;
        divided = false;
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

        topLeft = std::make_unique<Quadtree>(maxSize, boxTopLeft);
        topRight = std::make_unique<Quadtree>(maxSize, boxTopRight);
        bottomLeft = std::make_unique<Quadtree>(maxSize, boxBottomLeft);
        bottomRight = std::make_unique<Quadtree>(maxSize, boxBottomRight);

        for (sf::Vector2f* p : points) {
            insert(p);
        }

        points.clear();
        return;
    }

    void render(sf::RenderWindow& window) {
        sf::RectangleShape square(sf::Vector2f(box.getWidth(), box.getHeigth()));
        square.setOutlineThickness(5);
        square.setOutlineColor(sf::Color::Red);
        square.setFillColor(sf::Color::Transparent);
        square.setPosition(box.getCenter().x - box.getWidth()/2 + 10, box.getCenter().y - box.getHeigth() / 2 + 10);

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
                circle.setPosition(p->x, p->y);
                window.draw(circle);
            }
        }
    }

	std::vector<sf::Vector2f*> getPoints() { return points; }

private:
	std::vector<sf::Vector2f*> points;
	Box box;
	int maxSize;
	bool divided;
    std::unique_ptr<Quadtree> topLeft;
    std::unique_ptr<Quadtree> topRight;
    std::unique_ptr<Quadtree> bottomLeft;
    std::unique_ptr<Quadtree> bottomRight;
};