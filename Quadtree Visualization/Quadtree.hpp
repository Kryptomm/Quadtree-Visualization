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
			if ((*topRight).insert(point)) return true;
			else if ((*topRight).insert(point)) return true;
			else if ((*bottomLeft).insert(point)) return true;
			else if ((*bottomRight).insert(point)) return true;
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

		for (auto p : getPoints()) {
			if ((*topLeft).insert(p)) continue;
			else if ((*topRight).insert(p)) continue;
			else if ((*bottomLeft).insert(p)) continue;
			else if ((*bottomRight).insert(p)) continue;
		}

		points.clear();
		return;
	}

	std::vector<sf::Vector2f*> getPoints() { return points; }
	std::unique_ptr<Quadtree> getTopLeft() { return std::move(topLeft); }
	std::unique_ptr<Quadtree> getTopRight() { return std::move(topRight); }
	std::unique_ptr<Quadtree> getBottomLeft() { return std::move(bottomLeft); }
	std::unique_ptr<Quadtree> getBottomRight() { return std::move(bottomRight); }

private:
	std::unique_ptr<Quadtree> topLeft;
	std::unique_ptr<Quadtree> topRight;
	std::unique_ptr<Quadtree> bottomLeft;
	std::unique_ptr<Quadtree> bottomRight;

	std::vector<sf::Vector2f*> points;
	Box box;
	int maxSize;
	bool divided;
};