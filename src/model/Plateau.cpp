#include "model/Plateau.hpp"
#include <algorithm>
#include <array>
#include <cassert>
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

        for (Case* voisin : from->getVoisins()) {
            if (!voisin) continue;

            Piece* cible = voisin->getPiece();
            if (cible && cible->getCouleur() == piece->getCouleur()) continue;
            destinations.push_back(voisin->getId());
        }
        return destinations;
    }

    bool Plateau::deplacerPiece(int fromId, int toId) {
        Case* from = getCase(fromId);
        Case* to = getCase(toId);
        if (!from || !to || from->estVide()) return false;

        Piece* piece = from->getPiece();
        if (!piece || piece->getCouleur() != m_tourCourant) return false;
        if (fromId == toId || !sontVoisines(fromId, toId)) return false;

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