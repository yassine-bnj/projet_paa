#pragma once
#include <SFML/Graphics.hpp>
#include "../model/Plateau.hpp"
#include <vector>
#include <string>

namespace Yalta {
class GameView {
private:
    sf::RenderWindow& window;
    Plateau& plateau;
    std::vector<sf::ConvexShape> cellShapes;
    std::vector<sf::Vector2f> cellCenters;
    std::vector<sf::Color> baseColors;
    std::vector<int> highlightedCases;
    int selectedCaseId = -1;
    sf::Font font;
    bool m_fontLoaded = false;
    std::string infoMsg;
    sf::Vector2f infoPos{10.f, 10.f};
    float tileWidth = 46.f;
    float tileHeight = 24.f;
    float boardRadius = 325.f;
    sf::Vector2f boardCenter{490.f, 360.f};
    sf::CircleShape boardDisc;

    void initHexagons();
    void refreshBoardStyle();
    void drawPieces();
    void drawBoardDecoration();
    sf::Color colorForPlayer(Couleur c) const;
    std::string playerToString(Couleur c) const;
    sf::Vector2f cellCenterForId(int id) const;
    sf::ConvexShape createTile(const std::array<sf::Vector2f, 4>& points, sf::Color color) const;
    sf::Vector2f getHexCenter(int index) const { return cellCenterForId(index); }
    int getClickedCase(sf::Vector2f mousePos) const;

public:
    GameView(sf::RenderWindow& win, Plateau& p);
    void render();
    int handleMouseClick();
    void showInfo(const std::string& msg);
    void setSelectedCase(int caseId);
    void setHighlightedCases(const std::vector<int>& cases);
    void clearSelection();
    sf::RenderWindow& getWindow() { return window; }
};
} 