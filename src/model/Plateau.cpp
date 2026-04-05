#include "model/Plateau.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <map>
#include <vector>

namespace Yalta {

    namespace {
        // Modèle inspiré du script Python: grille logique 12x12 composée
        // de 6 sextants 4x4, soit exactement 96 cases jouables.
        constexpr int kCubeCount = 3;
        constexpr int kFacesPerCube = 2;
        constexpr int kSextants = kCubeCount * kFacesPerCube;
        constexpr int kBoardSize = 12;
        constexpr int kTotalCases = 96;

        struct SextantInterval {
            int x0;
            int x1;
            int y0;
            int y1;
        };

        constexpr std::array<SextantInterval, kSextants> kIntervals = {
            SextantInterval{0, 4, 0, 4},
            SextantInterval{0, 4, 4, 8},
            SextantInterval{8, 12, 4, 8},
            SextantInterval{8, 12, 8, 12},
            SextantInterval{4, 8, 8, 12},
            SextantInterval{4, 8, 0, 4}
        };

        struct LocalCell {
            int x;
            int y;
            int sextant;
        };

        std::array<LocalCell, kTotalCases> g_idToLocal{};
        std::array<std::array<int, kBoardSize>, kBoardSize> g_xyToId{};
        bool g_mapsReady = false;

        int sextantFor(int x, int y) {
            for (int i = 0; i < kSextants; ++i) {
                const SextantInterval& it = kIntervals[i];
                if (x >= it.x0 && x < it.x1 && y >= it.y0 && y < it.y1) {
                    return i;
                }
            }
            return -1;
        }

        Couleur zoneForSextant(int sextant) {
            // Trois zones de 32 cases (2 sextants chacun).
            if (sextant == 0 || sextant == 1) return Couleur::BLANC;
            if (sextant == 2 || sextant == 3) return Couleur::NOIR;
            return Couleur::GRIS;
        }

        void buildLogicalMaps() {
            if (g_mapsReady) return;

            for (auto& row : g_xyToId) {
                row.fill(-1);
            }

            int id = 0;
            for (int y = 0; y < kBoardSize; ++y) {
                for (int x = 0; x < kBoardSize; ++x) {
                    const int sextant = sextantFor(x, y);
                    if (sextant < 0) continue;
                    g_xyToId[y][x] = id;
                    g_idToLocal[id] = {x, y, sextant};
                    ++id;
                }
            }

            assert(id == kTotalCases);
            g_mapsReady = true;
        }

        int idFromXY(int x, int y) {
            if (x < 0 || x >= kBoardSize || y < 0 || y >= kBoardSize) return -1;
            return g_xyToId[y][x];
        }

        bool isInsideBoard(int x, int y) {
            return idFromXY(x, y) >= 0;
        }

        // Table des transitions spiralées de pions - une par couleur
        // Élimine les collisions en séparant les tables par couleur
        int getPawnTransition(int fromId, Couleur couleur) {
            if (couleur == Couleur::BLANC) {
                static const std::map<int, int> transitionsBlanc = {
                    // BLANC (avance vers le centre) - SEXTANTS 0 & 5
                    {31, 91}, {91, 83}, {83, 75},
                    {23, 90}, {90, 82}, {82, 74},
                    {15, 89}, {89, 81}, {81, 73},
                    {7, 88}, {88, 80}, {80, 72},
                    {27, 59}, {59, 58}, {58, 57},
                    {26, 51}, {51, 50}, {50, 49},
                    {25, 43}, {43, 42}, {42, 41},
                    {24, 35}, {35, 34}, {34, 33}
                };
                auto it = transitionsBlanc.find(fromId);
                if (it != transitionsBlanc.end()) return it->second;
            }
            else if (couleur == Couleur::NOIR) {
                static const std::map<int, int> transitionsNoir = {
                    // NOIR - SEXTANTS 1 & 2 (spirales vers centre et retours)
                    {59, 27}, {27, 19},
                    {51, 26}, {26, 18},
                    {43, 25}, {25, 17},
                    {35, 24}, {24, 16},
                    {60, 71}, {71, 70},
                    {61, 79}, {79, 78},
                    {62, 87}, {87, 86},
                    {63, 95}, {95, 94}
                };
                auto it = transitionsNoir.find(fromId);
                if (it != transitionsNoir.end()) return it->second;
            }
            else if (couleur == Couleur::GRIS) {
                static const std::map<int, int> transitionsGris = {
                    // GRIS - SEXTANTS 3 & 4 (spirales vers centre et retours)
                    {71, 60}, {60, 52},
                    {79, 61}, {61, 53},
                    {87, 62}, {62, 54},
                    {95, 63}, {63, 55},
                    {91, 31}, {31, 30},
                    {90, 23}, {23, 22},
                    {89, 15}, {15, 14},
                    {88, 7}, {7, 6}
                };
                auto it = transitionsGris.find(fromId);
                if (it != transitionsGris.end()) return it->second;
            }
            return -1;
        }

        // Cherche le voisin valide le plus aligné avec la direction (fx, fy)
        // Utilisé uniquement comme fallback quand idFromXY échoue
        int findBestNeighbor(int x, int y, int fx, int fy) {
            int bestId = -1;
            float bestScore = -2.0f;
            
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    int nx = x + dx;
                    int ny = y + dy;
                    int id = idFromXY(nx, ny);
                    if (id < 0) continue;
                    
                    // Score: alignement avec direction (fx, fy)
                    float score = static_cast<float>(dx * fx + dy * fy);
                    if (score > bestScore) {
                        bestScore = score;
                        bestId = id;
                    }
                }
            }
            return bestId;
        }

        LocalCell decodeId(int id) {
            return g_idToLocal[id];
        }

        CoordonneesEcran toScreen(int x, int y) {
            const float cx = 490.f;
            const float cy = 360.f;
            const float colStep = 34.f;
            const float rowStep = 28.f;
            const float shear = 10.f;

            const float dx = static_cast<float>(x) - 5.5f;
            const float dy = static_cast<float>(y) - 5.5f;
            return {cx + dx * colStep + dy * shear, cy + dy * rowStep};
        }

        bool containsId(const std::vector<int>& ids, int id) {
            return std::find(ids.begin(), ids.end(), id) != ids.end();
        }

        void addIfValid(
            std::vector<int>& out,
            int id,
            const Piece* piece,
            const std::array<Case*, 96>& cases,
            bool captureOnly = false,
            bool quietOnly = false)
        {
            if (id < 0) return;
            Case* c = cases[id];
            if (!c) return;

            Piece* target = c->getPiece();
            if (target) {
                if (quietOnly) return;
                if (target->getCouleur() == piece->getCouleur()) return;
                if (!containsId(out, id)) out.push_back(id);
                return;
            }

            if (captureOnly) return;
            if (!containsId(out, id)) out.push_back(id);
        }

        void addSliding(
            std::vector<int>& out,
            const Piece* piece,
            const std::array<Case*, 96>& cases,
            int startX,
            int startY,
            const std::vector<std::pair<int, int>>& directions)
        {
            for (const auto& d : directions) {
                int x = startX + d.first;
                int y = startY + d.second;

                while (isInsideBoard(x, y)) {
                    const int id = idFromXY(x, y);
                    if (id < 0) break;

                    Piece* target = cases[id]->getPiece();
                    if (!target) {
                        out.push_back(id);
                    } else {
                        if (target->getCouleur() != piece->getCouleur()) {
                            out.push_back(id);
                        }
                        break;
                    }

                    x += d.first;
                    y += d.second;
                }
            }
        }
    }

    Plateau::Plateau()
        : m_tourCourant(Couleur::BLANC)
        , m_etatPartie(EtatPartie::EN_COURS)
    {
        buildLogicalMaps();

        for (int i = 0; i < kTotalCases; ++i) {
            const LocalCell cell = decodeId(i);
            const CoordonneesEcran pos = toScreen(cell.x, cell.y);
            const Couleur zone = zoneForSextant(cell.sextant);
            m_cases[i] = new Case(i, pos, zone);
        }

        initialiserGrapheCases();
        initialiserPieces();
    }

    Plateau::~Plateau() {
        for (Case* c : m_cases) delete c;
    }

    void Plateau::initialiserGrapheCases() {
        static constexpr std::array<std::pair<int, int>, 6> kDirs = {
            std::pair<int, int>{1, 0}, std::pair<int, int>{-1, 0},
            std::pair<int, int>{0, 1}, std::pair<int, int>{0, -1},
            std::pair<int, int>{1, -1}, std::pair<int, int>{-1, 1}
        };

        for (int id = 0; id < kTotalCases; ++id) {
            Case* current = m_cases[id];
            const LocalCell cell = decodeId(id);

            for (const auto& d : kDirs) {
                const int nx = cell.x + d.first;
                const int ny = cell.y + d.second;
                const int nid = idFromXY(nx, ny);
                if (nid >= 0) {
                    current->ajouterVoisin(m_cases[nid]);
                }
            }
        }
    }

    void Plateau::initialiserPieces() {
        // Placement fidèle au tableau Python fourni par l'utilisateur.
        // Mapping couleurs: white -> BLANC, red -> NOIR, black -> GRIS.
        auto placer = [&](int x, int y, Couleur couleur, TypePiece type) {
            const int id = idFromXY(x, y);
            if (id < 0) return;
            Case* c = m_cases[id];
            m_pieces.push_back(std::make_unique<Piece>(couleur, c, nullptr, type));
            if (type == TypePiece::PION) {
                const LocalCell start = decodeId(id);
                Piece* p = m_pieces.back().get();
                // Direction fixée selon le SEXTANT DE DÉPART, ne change jamais.
                if (start.sextant == 0 || start.sextant == 2 || start.sextant == 4) {
                    p->setPawnDirection(0, 1);
                } else {
                    p->setPawnDirection(1, 0);
                }
            }
            c->setPiece(m_pieces.back().get());
        };

        // White camp (BLANC)
        placer(0, 0, Couleur::BLANC, TypePiece::TOUR);
        placer(1, 0, Couleur::BLANC, TypePiece::CAVALIER);
        placer(2, 0, Couleur::BLANC, TypePiece::FOU);
        placer(3, 0, Couleur::BLANC, TypePiece::REINE);
        placer(4, 0, Couleur::BLANC, TypePiece::TOUR);
        placer(5, 0, Couleur::BLANC, TypePiece::PION);

        placer(0, 1, Couleur::BLANC, TypePiece::PION);
        placer(1, 1, Couleur::BLANC, TypePiece::PION);
        placer(2, 1, Couleur::BLANC, TypePiece::PION);
        placer(3, 1, Couleur::BLANC, TypePiece::PION);
        placer(4, 1, Couleur::BLANC, TypePiece::CAVALIER);
        placer(5, 1, Couleur::BLANC, TypePiece::PION);

        placer(4, 2, Couleur::BLANC, TypePiece::FOU);
        placer(5, 2, Couleur::BLANC, TypePiece::PION);

        placer(4, 3, Couleur::BLANC, TypePiece::ROI);
        placer(5, 3, Couleur::BLANC, TypePiece::PION);

        // Red camp from Python mapped to NOIR
        placer(0, 4, Couleur::NOIR, TypePiece::TOUR);
        placer(1, 4, Couleur::NOIR, TypePiece::PION);
        placer(8, 4, Couleur::NOIR, TypePiece::TOUR);
        placer(9, 4, Couleur::NOIR, TypePiece::CAVALIER);
        placer(10, 4, Couleur::NOIR, TypePiece::FOU);
        placer(11, 4, Couleur::NOIR, TypePiece::REINE);

        placer(0, 5, Couleur::NOIR, TypePiece::CAVALIER);
        placer(1, 5, Couleur::NOIR, TypePiece::PION);
        placer(8, 5, Couleur::NOIR, TypePiece::PION);
        placer(9, 5, Couleur::NOIR, TypePiece::PION);
        placer(10, 5, Couleur::NOIR, TypePiece::PION);
        placer(11, 5, Couleur::NOIR, TypePiece::PION);

        placer(0, 6, Couleur::NOIR, TypePiece::FOU);
        placer(1, 6, Couleur::NOIR, TypePiece::PION);

        placer(0, 7, Couleur::NOIR, TypePiece::ROI);
        placer(1, 7, Couleur::NOIR, TypePiece::PION);

        // Black camp from Python mapped to GRIS
        placer(4, 8, Couleur::GRIS, TypePiece::TOUR);
        placer(5, 8, Couleur::GRIS, TypePiece::CAVALIER);
        placer(6, 8, Couleur::GRIS, TypePiece::FOU);
        placer(7, 8, Couleur::GRIS, TypePiece::REINE);
        placer(8, 8, Couleur::GRIS, TypePiece::TOUR);
        placer(9, 8, Couleur::GRIS, TypePiece::PION);

        placer(4, 9, Couleur::GRIS, TypePiece::PION);
        placer(5, 9, Couleur::GRIS, TypePiece::PION);
        placer(6, 9, Couleur::GRIS, TypePiece::PION);
        placer(7, 9, Couleur::GRIS, TypePiece::PION);
        placer(8, 9, Couleur::GRIS, TypePiece::CAVALIER);
        placer(9, 9, Couleur::GRIS, TypePiece::PION);

        placer(8, 10, Couleur::GRIS, TypePiece::FOU);
        placer(9, 10, Couleur::GRIS, TypePiece::PION);

        placer(8, 11, Couleur::GRIS, TypePiece::ROI);
        placer(9, 11, Couleur::GRIS, TypePiece::PION);
    }

    Case* Plateau::getCase(int id) const {
        if (id < 0 || id >= 96) return nullptr;
        return m_cases[id];
    }

    std::vector<CoupValide> Plateau::getCoupsPossibles(Couleur joueur) const {
        std::vector<CoupValide> coups;
        for (int from = 0; from < 96; ++from) {
            Case* c = m_cases[from];
            if (!c || c->estVide()) continue;
            Piece* p = c->getPiece();
            if (!p || p->getCouleur() != joueur) continue;

            for (int to : getDestinationsLegales(from)) {
                Case* cible = m_cases[to];
                const bool capture = cible && !cible->estVide();
                coups.emplace_back(from, to, capture);
            }
        }
        return coups;
    }

    std::vector<int> Plateau::getDestinationsLegales(int fromId) const {
        std::vector<int> destinations;
        Case* from = getCase(fromId);
        if (!from || from->estVide()) return destinations;

        Piece* piece = from->getPiece();
        if (!piece || piece->getCouleur() != m_tourCourant) return destinations;

        const LocalCell pos = decodeId(fromId);
        const int x = pos.x;
        const int y = pos.y;

        static const std::vector<std::pair<int, int>> kOrthoDirs = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1}
        };

        static const std::vector<std::pair<int, int>> kDiagDirs = {
            {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
        };

        static const std::vector<std::pair<int, int>> kKnightOffsets = {
            {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
            {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
        };

        switch (piece->getType()) {
            case TypePiece::TOUR: {
                addSliding(destinations, piece, m_cases, x, y, kOrthoDirs);
                break;
            }
            case TypePiece::FOU: {
                addSliding(destinations, piece, m_cases, x, y, kDiagDirs);
                break;
            }
            case TypePiece::REINE: {
                std::vector<std::pair<int, int>> dirs = kOrthoDirs;
                dirs.insert(dirs.end(), kDiagDirs.begin(), kDiagDirs.end());
                addSliding(destinations, piece, m_cases, x, y, dirs);
                break;
            }
            case TypePiece::ROI: {
                for (const auto& d : kOrthoDirs) {
                    addIfValid(destinations, idFromXY(x + d.first, y + d.second), piece, m_cases);
                }
                for (const auto& d : kDiagDirs) {
                    addIfValid(destinations, idFromXY(x + d.first, y + d.second), piece, m_cases);
                }
                break;
            }
            case TypePiece::CAVALIER: {
                for (const auto& d : kKnightOffsets) {
                    addIfValid(destinations, idFromXY(x + d.first, y + d.second), piece, m_cases);
                }
                break;
            }
            case TypePiece::PION: {
                const int fx = piece->getPawnDirX();
                const int fy = piece->getPawnDirY();

                // === Avance d'une case (case vide uniquement) ===
                // PRIORITÉ: d'abord la table de transitions, sinon le mouvement direct
                int oneStepId = -1;
                int transitionId = getPawnTransition(fromId, piece->getCouleur());
                
                if (transitionId >= 0) {
                    // Utiliser la table de transitions (priorité absolue)
                    if (m_cases[transitionId]->estVide()) {
                        destinations.push_back(transitionId);
                        oneStepId = transitionId;
                    }
                } else {
                    // Sinon, essayer le mouvement direct
                    oneStepId = idFromXY(x + fx, y + fy);
                    if (oneStepId >= 0 && m_cases[oneStepId]->estVide()) {
                        destinations.push_back(oneStepId);
                    }

                    // === Double pas initial (si n'a pas encore joué) ===
                    if (oneStepId >= 0 && !piece->aDejaJoue()) {
                        LocalCell nextPos = decodeId(oneStepId);
                        int twoStepId = idFromXY(nextPos.x + fx, nextPos.y + fy);
                        if (twoStepId >= 0 && m_cases[twoStepId]->estVide()) {
                            destinations.push_back(twoStepId);
                        }
                    }

                    // === Captures diagonales (seulement si pas de transition) ===
                    const int cap1Id = idFromXY(x + fx + fy, y + fy - fx);
                    if (cap1Id >= 0) {
                        Piece* target = m_cases[cap1Id]->getPiece();
                        if (target && target->getCouleur() != piece->getCouleur()) {
                            destinations.push_back(cap1Id);
                        }
                    }

                    const int cap2Id = idFromXY(x + fx - fy, y + fy + fx);
                    if (cap2Id >= 0) {
                        Piece* target = m_cases[cap2Id]->getPiece();
                        if (target && target->getCouleur() != piece->getCouleur()) {
                            destinations.push_back(cap2Id);
                        }
                    }
                }
                break;
            }
            default:
                break;
        }
        return destinations;
    }

    bool Plateau::deplacerPiece(int fromId, int toId) {
        Case* from = getCase(fromId);
        Case* to = getCase(toId);
        if (!from || !to || from->estVide()) return false;

        Piece* piece = from->getPiece();
        if (!piece || piece->getCouleur() != m_tourCourant) return false;
        if (fromId == toId) return false;

        const std::vector<int> legals = getDestinationsLegales(fromId);
        if (std::find(legals.begin(), legals.end(), toId) == legals.end()) return false;

        if (!to->estVide()) {
            Piece* cible = to->getPiece();
            if (cible && cible->getCouleur() == piece->getCouleur()) return false;

            auto it = std::find_if(m_pieces.begin(), m_pieces.end(), [cible](const std::unique_ptr<Piece>& p) {
                return p.get() == cible;
            });
            if (it != m_pieces.end()) m_pieces.erase(it);
        }

        piece->deplacer(to);
        tourSuivant();
        return true;
    }

    bool Plateau::estEchec(int roiId) const {
        // TODO: Vérifier si le roi est menacé par une pièce adverse
        return false;
    }

    bool Plateau::estMat(int roiId) const {
        if (!estEchec(roiId)) return false;
        return false;
    }

    bool Plateau::sontVoisines(int a, int b) const {
        Case* ca = getCase(a);
        Case* cb = getCase(b);
        if (!ca || !cb) return false;

        for (Case* voisin : ca->getVoisins()) {
            if (voisin == cb) return true;
        }
        return false;
    }

    void Plateau::tourSuivant() {
        // Cycle BLANC -> NOIR -> GRIS -> BLANC...
        switch (m_tourCourant) {
            case Couleur::BLANC: m_tourCourant = Couleur::NOIR; break;
            case Couleur::NOIR:  m_tourCourant = Couleur::GRIS; break;
            case Couleur::GRIS:  m_tourCourant = Couleur::BLANC; break;
            default: break;
        }
    }

} // namespace Yalta