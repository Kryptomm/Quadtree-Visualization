#include <SFML/Graphics.hpp>

class Box {
public:
	Box(sf::Vector2f* c, double w, double h) : Center(*c), width(w), height(h) {}
	
	bool intersects(Box* const otherBox){
		if (
			Center.x < otherBox->Center.x + otherBox->width &&
			Center.x + width > otherBox->Center.x &&
			Center.y < otherBox->Center.y + otherBox->height &&
			height + Center.y > otherBox->Center.y
			) {
			return true;
		}
		else {
			return false;
		}
	}

	bool pointInBox(sf::Vector2f* point) {
		double leftX = Center.x - width / 2;
		double rightX = Center.x + width / 2;

		double topY = Center.y + height / 2;
		double bottomY = Center.y - height / 2;

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
	sf::Vector2f getCenter() const { return Center; }
	double getWidth() const { return width; }
	double getHeigth() const { return height; }

private:
	sf::Vector2f Center;

	double width;
	double height;

};

std::ostream& operator<<(std::ostream& s, const Box& box) {
	return s << "Center: (" + std::to_string(box.getCenter().x) + "," + std::to_string(box.getCenter().y) + ")" + "\n"
			  + "Width: " + std::to_string(box.getWidth()) + "\n"
	          + "Height: " + std::to_string(box.getHeigth()) + "\n";
}