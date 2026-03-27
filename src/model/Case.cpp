#include "model/Case.hpp"
namespace Yalta {

    Case::Case(int id, CoordonneesEcran pos, Couleur zone)
        : m_id(id)
        , m_positionEcran(pos)
        , m_piece(nullptr)  // On manipule juste un pointeur, pas besoin de la définition complète
        , m_proprietaire(zone)
    {
        m_voisins.reserve(6);
    }

    void Case::ajouterVoisin(Case* voisin) {
        if (voisin == nullptr) return;
        for (Case* v : m_voisins) {
            if (v == voisin) return;
        }
        m_voisins.push_back(voisin);
    }

    const std::vector<Case*>& Case::getVoisins() const {
        return m_voisins;
    }

    bool Case::estVide() const {
        return m_piece == nullptr;  // Comparaison de pointeur
    }

    void Case::setPiece(Piece* piece) {
        m_piece = piece;
    }

    Piece* Case::getPiece() const {
        return m_piece;
    }

    Piece* Case::retirerPiece() {
        Piece* temp = m_piece;
        m_piece = nullptr;
        return temp;
    }

    bool Case::appartientA(Couleur couleur) const {
        return m_proprietaire == couleur;
    }

} 