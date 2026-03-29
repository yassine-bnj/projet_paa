#include "model/Piece.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"  // Pour getCoupsPossibles
// #include "IMouvement.hpp" 

namespace Yalta {

    Piece::Piece(Couleur couleur, Case* position, IMouvement* strategie, TypePiece type)
        : m_couleur(couleur)
        , m_position(position)
        , m_strategieMouvement(strategie)
        , m_aDejaJoue(false)
        , m_type(type)
    {
    }

    std::string Piece::getNom() const {
        switch (m_type) {
            case TypePiece::PION: return "Pion";
            case TypePiece::TOUR: return "Tour";
            case TypePiece::FOU: return "Fou";
            case TypePiece::CAVALIER: return "Cavalier";
            case TypePiece::REINE: return "Reine";
            case TypePiece::ROI: return "Roi";
            default: return "Piece";
        }
    }

    int Piece::getValeur() const {
        switch (m_type) {
            case TypePiece::PION: return 1;
            case TypePiece::CAVALIER:
            case TypePiece::FOU: return 3;
            case TypePiece::TOUR: return 5;
            case TypePiece::REINE: return 9;
            case TypePiece::ROI: return 100;
            default: return 0;
        }
    }

    char Piece::getSymbole() const {
        switch (m_type) {
            case TypePiece::PION: return 'P';
            case TypePiece::TOUR: return 'T';
            case TypePiece::FOU: return 'F';
            case TypePiece::CAVALIER: return 'C';
            case TypePiece::REINE: return 'Q';
            case TypePiece::ROI: return 'K';
            default: return '?';
        }
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