#pragma once

#include "../utils/types.hpp"
#include <vector>

namespace Yalta {

    class Piece;  

    class Case {
    private:
        int m_id;
        CoordonneesEcran m_positionEcran;
        Piece* m_piece;
        std::vector<Case*> m_voisins;
        Couleur m_proprietaire;

    public:
        Case(int id, CoordonneesEcran pos, Couleur zone);
        void ajouterVoisin(Case* voisin);
        const std::vector<Case*>& getVoisins() const;
        bool estVide() const;
        void setPiece(Piece* piece);
        Piece* getPiece() const;
        Piece* retirerPiece();
        
        int getId() const { return m_id; }
        CoordonneesEcran getPositionEcran() const { return m_positionEcran; }
        Couleur getProprietaire() const { return m_proprietaire; }
        bool appartientA(Couleur couleur) const;
    };

} 