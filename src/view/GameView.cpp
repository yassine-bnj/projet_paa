#include "view/GameView.hpp"
#include "model/Piece.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cmath>
#include <sstream>

namespace {
    bool inSextantIntervals(int x, int y, int& sextant) {
        static constexpr std::array<std::array<int, 4>, 6> intervals = {
            std::array<int, 4>{0, 4, 0, 4},
            std::array<int, 4>{0, 4, 4, 8},
            std::array<int, 4>{8, 12, 4, 8},
            std::array<int, 4>{8, 12, 8, 12},
            std::array<int, 4>{4, 8, 8, 12},
            std::array<int, 4>{4, 8, 0, 4}
        };

        for (int i = 0; i < 6; ++i) {
            const auto& it = intervals[i];
            if (x >= it[0] && x < it[1] && y >= it[2] && y < it[3]) {
                sextant = i;
                return true;
            }
        }
        return false;
    }

    sf::Vector2f add(const sf::Vector2f& a, const sf::Vector2f& b) {
        return {a.x + b.x, a.y + b.y};
    }

    sf::Vector2f sub(const sf::Vector2f& a, const sf::Vector2f& b) {
        return {a.x - b.x, a.y - b.y};
    }

    sf::Vector2f mul(const sf::Vector2f& a, float k) {
        return {a.x * k, a.y * k};
    }
}

namespace Yalta {

GameView::GameView(sf::RenderWindow& win, Plateau& p)
    : window(win), plateau(p), infoMsg("Selectionne une piece sur le plateau Yalta")
{
    boardDisc = sf::CircleShape(boardRadius);
    boardDisc.setOrigin(sf::Vector2f(boardRadius, boardRadius));
    boardDisc.setPosition(boardCenter);
    boardDisc.setFillColor(sf::Color(228, 220, 196));
    boardDisc.setOutlineColor(sf::Color(40, 84, 50));
    boardDisc.setOutlineThickness(3.f);

    // Chargement de la police (chemin Windows standard)
    if (font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        m_fontLoaded = true;
    }

    initHexagons();
}

void GameView::initHexagons() {
    const float boardSize = 265.f;
    const float side = boardSize * 0.5f;
    const float height = std::sqrt(boardSize * boardSize - side * side);
    const float c30 = std::cos(30.f * 3.1415926535f / 180.f);
    const float s30 = std::sin(30.f * 3.1415926535f / 180.f);
    const float c60 = std::cos(60.f * 3.1415926535f / 180.f);

    const sf::Vector2f mid = boardCenter;

    const std::array<sf::Vector2f, 6> v123 = {
        sf::Vector2f(-boardSize * c60, -height),
        sf::Vector2f( boardSize * c60, -height),
        sf::Vector2f( boardSize, 0.f),
        sf::Vector2f( boardSize * c60, height),
        sf::Vector2f(-boardSize * c60, height),
        sf::Vector2f(-boardSize, 0.f)
    };

    const std::array<sf::Vector2f, 6> vabc = {
        sf::Vector2f(-height * c30, -height * s30),
        sf::Vector2f(0.f, -height),
        sf::Vector2f(height * c30, -height * s30),
        sf::Vector2f(height * c30, height * s30),
        sf::Vector2f(0.f, height),
        sf::Vector2f(-height * c30, height * s30)
    };

    std::vector<std::array<int, 2>> idToXY;
    idToXY.reserve(96);
    for (int y = 0; y < 12; ++y) {
        for (int x = 0; x < 12; ++x) {
            int sextant = -1;
            if (inSextantIntervals(x, y, sextant)) {
                idToXY.push_back({x, y});
            }
        }
    }

    for (int i = 0; i < 96; ++i) {
        const int x = idToXY[i][0];
        const int y = idToXY[i][1];
        int sextant = -1;
        inSextantIntervals(x, y, sextant);

        const float ratioX1 = static_cast<float>(x % 4) / 4.f;
        const float ratioY1 = static_cast<float>(y % 4) / 4.f;
        const float ratioX2 = static_cast<float>(x % 4 + 1) / 4.f;
        const float ratioY2 = static_cast<float>(y % 4 + 1) / 4.f;

        const float midRatioX = (ratioX1 + ratioX2) * 0.5f;
        const float midRatioY = (ratioY1 + ratioY2) * 0.5f;

        const sf::Vector2f s1 = mul(v123[sextant], 0.5f);
        const sf::Vector2f s2 = mul(v123[(sextant + 2) % 6], 0.5f);
        const sf::Vector2f corner = add(mid, v123[(sextant + 4) % 6]);

        const sf::Vector2f U1 = add(sub(mul(vabc[(sextant + 1) % 6], ratioY1), mul(s1, ratioY1)), s2);
        const sf::Vector2f U2 = add(sub(mul(vabc[(sextant + 1) % 6], ratioY2), mul(s1, ratioY2)), s2);
        const sf::Vector2f midU = add(sub(mul(vabc[(sextant + 1) % 6], midRatioY), mul(s1, midRatioY)), s2);

        const sf::Vector2f p1 = add(corner, add(mul(s1, ratioY1), mul(U1, ratioX1)));
        const sf::Vector2f p2 = add(corner, add(mul(s1, ratioY1), mul(U1, ratioX2)));
        const sf::Vector2f p3 = add(corner, add(mul(s1, ratioY2), mul(U2, ratioX2)));
        const sf::Vector2f p4 = add(corner, add(mul(s1, ratioY2), mul(U2, ratioX1)));

        const sf::Vector2f center = add(corner, add(mul(s1, midRatioY), mul(midU, midRatioX)));
        cellCenters.push_back(center);

        const std::array<sf::Vector2f, 4> points = {p1, p2, p3, p4};

        Case* c = plateau.getCase(i);
        (void)c;
        const sf::Color dark(54, 39, 32);
        const sf::Color light(229, 210, 170);
        const sf::Color fillColor = ((x + y + sextant) % 2 == 0) ? dark : light;

        baseColors.push_back(fillColor);
        cellShapes.push_back(createTile(points, fillColor));
    }
}

void GameView::refreshBoardStyle() {
    for (size_t i = 0; i < cellShapes.size(); ++i) {
        cellShapes[i].setFillColor(baseColors[i]);
        cellShapes[i].setOutlineColor(sf::Color(247, 243, 228));
        cellShapes[i].setOutlineThickness(1.2f);
    }

    if (selectedCaseId >= 0 && selectedCaseId < static_cast<int>(cellShapes.size())) {
        cellShapes[selectedCaseId].setOutlineColor(sf::Color(255, 208, 0));
        cellShapes[selectedCaseId].setOutlineThickness(3.5f);
    }

    for (int id : highlightedCases) {
        if (id < 0 || id >= static_cast<int>(cellShapes.size())) continue;
        sf::Color blended = baseColors[id];
        blended.r = static_cast<std::uint8_t>(std::min(255, blended.r + 25));
        blended.g = static_cast<std::uint8_t>(std::min(255, blended.g + 25));
        blended.b = static_cast<std::uint8_t>(std::min(255, blended.b + 12));
        cellShapes[id].setFillColor(blended);
        cellShapes[id].setOutlineColor(sf::Color(255, 220, 80));
        cellShapes[id].setOutlineThickness(2.8f);
    }
}

sf::Color GameView::colorForPlayer(Couleur c) const {
    switch (c) {
        case Couleur::BLANC: return sf::Color(250, 250, 250);
        case Couleur::NOIR: return sf::Color(25, 25, 25);
        case Couleur::GRIS: return sf::Color(90, 110, 130);
        default: return sf::Color::Transparent;
    }
}

std::string GameView::playerToString(Couleur c) const {
    switch (c) {
        case Couleur::BLANC: return "BLANC";
        case Couleur::NOIR: return "NOIR";
        case Couleur::GRIS: return "GRIS";
        default: return "AUCUN";
    }
}

void GameView::drawPieces() {
    for (int i = 0; i < 96; ++i) {
        Case* c = plateau.getCase(i);
        if (!c || c->estVide()) continue;

        Piece* p = c->getPiece();
        if (!p) continue;

        sf::CircleShape token(11.f);
        token.setOrigin(sf::Vector2f(11.f, 11.f));
        token.setPosition(cellCenters[i] + sf::Vector2f(0.f, -6.f));
        token.setFillColor(colorForPlayer(p->getCouleur()));
        token.setOutlineColor(sf::Color(0, 0, 0, 90));
        token.setOutlineThickness(1.5f);
        window.draw(token);

        if (m_fontLoaded) {
            sf::Text label(font, std::string(1, p->getSymbole()), 14);
            if (p->getCouleur() == Couleur::NOIR) {
                label.setFillColor(sf::Color(20, 20, 20));
            } else {
                label.setFillColor(sf::Color(248, 248, 248));
            }

            sf::FloatRect b = label.getLocalBounds();
            label.setOrigin(sf::Vector2f(b.position.x + b.size.x * 0.5f, b.position.y + b.size.y * 0.5f));
            label.setPosition(cellCenters[i] + sf::Vector2f(0.f, -6.f));
            window.draw(label);
        }
    }
}

sf::Vector2f GameView::cellCenterForId(int id) const {
    Case* c = plateau.getCase(id);
    if (!c) return boardCenter;
    const CoordonneesEcran p = c->getPositionEcran();
    return {p.x, p.y};
}

sf::ConvexShape GameView::createTile(const std::array<sf::Vector2f, 4>& points, sf::Color color) const {
    sf::ConvexShape tile(4);
    tile.setPoint(0, points[0]);
    tile.setPoint(1, points[1]);
    tile.setPoint(2, points[2]);
    tile.setPoint(3, points[3]);
    tile.setFillColor(color);
    tile.setOutlineColor(sf::Color(30, 20, 15, 120));
    tile.setOutlineThickness(0.7f);
    return tile;
}

void GameView::render() {
    refreshBoardStyle();

    window.clear(sf::Color(187, 184, 182));
    drawBoardDecoration();
    window.draw(boardDisc);
    for (const auto& tile : cellShapes) window.draw(tile);
    drawPieces();

    if (m_fontLoaded) {
        std::ostringstream oss;
        oss << "Tour: " << playerToString(plateau.getTourCourant()) << " | " << infoMsg;
        sf::Text text(font, oss.str(), 18);
        text.setFillColor(sf::Color::White);
        text.setPosition(infoPos);
        window.draw(text);
    }
    window.display();
}

void GameView::drawBoardDecoration() {
    sf::CircleShape shadow(boardRadius + 10.f);
    shadow.setOrigin(sf::Vector2f(boardRadius + 10.f, boardRadius + 10.f));
    shadow.setPosition(boardCenter + sf::Vector2f(5.f, 6.f));
    shadow.setFillColor(sf::Color(0, 0, 0, 24));
    window.draw(shadow);
}

int GameView::handleMouseClick() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
    return getClickedCase(worldPos);
}

int GameView::getClickedCase(sf::Vector2f pos) const {
    const float threshold = 26.f;
    float minDist = threshold;
    int clickedId = -1;

    for (size_t i = 0; i < cellCenters.size(); ++i) {
        sf::Vector2f diff = pos - cellCenters[i];
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist < minDist) {
            minDist = dist;
            clickedId = static_cast<int>(i);
        }
    }
    return clickedId;
}

void GameView::showInfo(const std::string& msg) {
    infoMsg = msg;
}

void GameView::setSelectedCase(int caseId) {
    selectedCaseId = caseId;
}

void GameView::setHighlightedCases(const std::vector<int>& cases) {
    highlightedCases = cases;
}

void GameView::clearSelection() {
    selectedCaseId = -1;
    highlightedCases.clear();
}

} 