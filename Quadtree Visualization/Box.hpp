#include <SFML/Graphics.hpp>

class Box {
public:
	sf::Vector2f Center;

	double width;
	double height;

	Box(sf::Vector2f c, double w, double h) {
		Center.x = c.x;
		Center.y = c.y;

		width = w;
		height = h;
	}
	
	bool intersects(Box const &otherBox){
		if (
			Center.x < otherBox.Center.x + otherBox.width &&
			Center.x + width > otherBox.Center.x &&
			Center.y < otherBox.Center.y + otherBox.height &&
			height + Center.y > otherBox.Center.y
			) {
			return true;
		}
		else {
			return false;
		}
	}

	bool pointInBox(sf::Vector2f point) {
		double leftX = Center.x - width / 2;
		double rightX = Center.x + width / 2;

		double topY = Center.y + height / 2;
		double bottomY = Center.y - height / 2;

		if (
			leftX <= point.x && point.x <= rightX &&
			bottomY <= point.y && point.y <= topY
		) {
			return true;
		}
		else {
			return false;
		}
	}
};

std::ostream& operator<<(std::ostream& s, const Box& box) {
	return s << "Center: (" + std::to_string(box.Center.x) + "," + std::to_string(box.Center.y) + ")" + "\n"
			  + "Width: " + std::to_string(box.width) + "\n"
	          + "Height: " + std::to_string(box.height) + "\n";
}