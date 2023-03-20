#ifndef BOX_HPP
#define BOX_HPP

#include <SFML/Graphics.hpp>
#include <iostream>

class Box {
public:
    Box(sf::Vector2f* c, double w, double h);

    bool intersects(Box* const otherBox);
    bool pointInBox(sf::Vector2f* point);

    sf::Vector2f getCenter() const;

    double getWidth() const;
    double getHeigth() const;
    double getRightBound() const;
    double getLeftBound() const;
    double getUpperBound() const;
    double getLowerBound() const;

private:
    sf::Vector2f center;

    double width;
    double height;

    double leftBound;
    double rightBound;
    double upperBound;
    double lowerBound;
};

std::ostream& operator<<(std::ostream& s, const Box& box);

#endif