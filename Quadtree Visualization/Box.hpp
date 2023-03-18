#include <SFML/Graphics.hpp>

class Box {
public:
	Box(sf::Vector2f* c, double w, double h) : center(*c), width(w), height(h) {}
	
	bool intersects(Box* const otherBox){
		// Calculate half width and half height of each rectangle
		float halfWidth1 = width / 2;
		float halfHeight1 = height / 2;
		float halfWidth2 = otherBox->getWidth() / 2;
		float halfHeight2 = otherBox->getHeigth() / 2;

		// Calculate the minimum and maximum x and y coordinates of each rectangle
		float minX1 = center.x - halfWidth1;
		float minY1 = center.y - halfHeight1;
		float maxX1 = center.x + halfWidth1;
		float maxY1 = center.y + halfHeight1;

		float minX2 = otherBox->center.x - halfWidth2;
		float minY2 = otherBox->center.y - halfHeight2;
		float maxX2 = otherBox->center.x + halfWidth2;
		float maxY2 = otherBox->center.y + halfHeight2;

		// Check for intersection
		return (minX1 <= maxX2 && maxX1 >= minX2 && minY1 <= maxY2 && maxY1 >= minY2);
	}

	bool pointInBox(sf::Vector2f* point) {
		double leftX = center.x - width / 2;
		double rightX = center.x + width / 2;

		double topY = center.y + height / 2;
		double bottomY = center.y - height / 2;

		if (
			leftX <= point->x && point->x <= rightX &&
			bottomY <= point->y && point->y <= topY
		) {
			return true;
		}
		else {
			return false;
		}
	}

	sf::Vector2f getCenter() const { return center; }
	double getWidth() const { return width; }
	double getHeigth() const { return height; }
	double getRightBound() const { return center.x + width / 2; }
	double getLeftBound() const { return center.x - width / 2; }
	double getUpperBound() const { return center.y - height / 2; }
	double getLowerBound() const { return center.y + height / 2; }

private:
	sf::Vector2f center;

	int width;
	int height;
};

std::ostream& operator<<(std::ostream& s, const Box& box) {
	return s << "Center: (" + std::to_string(box.getCenter().x) + "," + std::to_string(box.getCenter().y) + ")" + "\n"
			  + "Width: " + std::to_string(box.getWidth()) + "\n"
	          + "Height: " + std::to_string(box.getHeigth()) + "\n";
}