#include "model/Piece.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"  // Pour getCoupsPossibles
// #include "IMouvement.hpp" 

namespace Yalta {

    Piece::Piece(Couleur couleur, Case* position, IMouvement* strategie)
        : m_couleur(couleur)
        , m_position(position)
        , m_strategieMouvement(strategie)
        , m_aDejaJoue(false)
    {
    }

    std::vector<Case*> Piece::getCoupsPossibles(const Plateau& plateau) const {
        if (m_strategieMouvement == nullptr) return {};
        return m_strategieMouvement->calculerCoups(m_position, plateau);
    }

    void Piece::deplacer(Case* nouvelleCase) {
        if (m_position && nouvelleCase) {
            m_position->setPiece(nullptr);  // Libère l'ancienne case
            nouvelleCase->setPiece(this);   // Place la pièce sur la nouvelle
            m_position = nouvelleCase;
            m_aDejaJoue = true;
        }
    }

} 