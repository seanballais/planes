#include <SFML/Window.hpp>

int main(int argc, char* argv[])
{
    sf::Window window;
    window.create(sf::VideoMode(640, 480), "Planes");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }

    return 0;
}
