#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "model/Plateau.hpp"
#include "view/GameView.hpp"


int main() {
    std::cout << "Demarrage..." << std::endl;

    // 1. Initialisation du modèle
    Yalta::Plateau plateau;
    int selectedCase = -1;
    std::vector<int> legalTargets;
    
    // 2. Fenêtre SFML 3
    sf::RenderWindow window(sf::VideoMode({980, 780}), "Yalta Chess - Graphique");
    
    // 3. Vue liée au plateau
    Yalta::GameView view(window, plateau);
    view.showInfo("Selectionne une piece du joueur courant");

    while (window.isOpen()) {
        // Gestion des événements SFML 3
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            
            if (event->is<sf::Event::MouseButtonPressed>()) {
                if (event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left) {
                    int caseId = view.handleMouseClick();
                    if (caseId != -1) {
                        if (selectedCase == -1) {
                            auto coups = plateau.getDestinationsLegales(caseId);
                            if (!coups.empty()) {
                                selectedCase = caseId;
                                legalTargets = coups;
                                view.setSelectedCase(selectedCase);
                                view.setHighlightedCases(legalTargets);
                                view.showInfo("Piece selectionnee en " + std::to_string(caseId) + ", choisis une destination");
                            } else {
                                view.clearSelection();
                                view.showInfo("Case " + std::to_string(caseId) + " non jouable pour ce tour");
                            }
                        } else {
                            bool legal = false;
                            for (int target : legalTargets) {
                                if (target == caseId) {
                                    legal = true;
                                    break;
                                }
                            }

                            if (legal && plateau.deplacerPiece(selectedCase, caseId)) {
                                view.showInfo("Coup joue : " + std::to_string(selectedCase) + " -> " + std::to_string(caseId));
                                std::cout << "[Move] " << selectedCase << " -> " << caseId << "\n";
                            } else {
                                view.showInfo("Destination invalide");
                            }

                            selectedCase = -1;
                            legalTargets.clear();
                            view.clearSelection();
                        }
                    } else {
                        view.showInfo("Aucune case detectee");
                    }
                }
            }
        }

        view.render();
    }

    return 0;
}