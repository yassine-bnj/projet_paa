#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    std::cout << "Demarrage..." << std::endl;

    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "Yalta Chess"
    );

    std::cout << "Fenetre creee !" << std::endl;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);
        window.display();
    }

    return 0;
}