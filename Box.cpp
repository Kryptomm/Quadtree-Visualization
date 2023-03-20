#include "Box.hpp"

Box::Box(sf::Vector2f* c, double w, double h) : center(*c), width(w), height(h) {
    rightBound = center.x + width / 2;
    leftBound = center.x - width / 2;
    upperBound = center.y - height / 2;
    lowerBound = center.y + height / 2;
}


bool Box::intersects(Box* const otherBox)
{
    return !(getLeftBound() > otherBox->getRightBound()
        || getRightBound() < otherBox->getLeftBound()
        || getLowerBound() < otherBox->getUpperBound()
        || getUpperBound() > otherBox->getLowerBound());
}

bool Box::pointInBox(sf::Vector2f* point)
{
    double leftX = center.x - width / 2;
    double rightX = center.x + width / 2;

    double topY = center.y + height / 2;
    double bottomY = center.y - height / 2;

    if (leftX <= point->x && point->x <= rightX && bottomY <= point->y && point->y <= topY) {
        return true;
    }
    else {
        return false;
    }
}

sf::Vector2f Box::getCenter() const { return center; }
double Box::getWidth() const { return width; }
double Box::getHeigth() const{ return height; }
double Box::getRightBound() const { return rightBound; }
double Box::getLeftBound() const { return leftBound; }
double Box::getUpperBound() const { return  upperBound; }
double Box::getLowerBound() const { return  lowerBound; }


std::ostream& operator<<(std::ostream& s, const Box& box) {
    return s << "Center: (" + std::to_string(box.getCenter().x) + "," + std::to_string(box.getCenter().y) + ")" + "\n"
        + "Width: " + std::to_string(box.getWidth()) + "\n"
        + "Height: " + std::to_string(box.getHeigth()) + "\n";
}