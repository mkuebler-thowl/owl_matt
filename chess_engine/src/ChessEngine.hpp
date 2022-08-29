#pragma once

#include <vector>
#include "defines.hpp"

#include "Position.hpp"
#include "Move.hpp"
#include "MinMaxResult.hpp"
#include "ChessValidation.hpp"
#include "RepitionMap.hpp"
#include "EngineOptions.hpp"

#include <mutex>

namespace owl
{
	/**
	 * Schach-Engine: Beinhaltet die Logik von OWL-Matt, wie bspw. Zugfindung, Speicherung von Position und Killerz�gen. 
	 */
	class ChessEngine
	{
	public:
		explicit ChessEngine();
		virtual ~ChessEngine();
		/**
		 * Funktion zur Ermittlung des Zugs der aktuellen Position.
		 * Wird von der UCI-Schnittstelle ermittelt
		 * 
		 * \param player der Spieler Wei�/Schwarz der Schach-Engine (1 oder -1)
		 * \param depth die maximale Suchtiefe f�r die Zugfindung
		 * \param parameter_flags Baustein-Parameter (>= FT_NULL)
		 * \return Den besten Zug und sein korrespondierenden Wert aus der Evaluierung
		 */
		PAIR<Move, EVALUATION_VALUE> searchMove(INT32 player, INT32 depth, UCHAR parameter_flags);
		/**
		 * �berpr�fung, ob der Debug-Modus f�r das UCI aktiviert ist.
		 * 
		 * \return Im Debug-Modus?
		 */
		BOOL inDebugMode() const;
		/**
		 * Aktiviert den Debug-Modus f�r das UCI.
		 * 
		 * \param debug
		 */
		VOID setDebugMode(BOOL debug);
		/**
		 * Stoppe die aktuelle Berechnung. 
		 * Dieser Befehl wird durch das UCI gegebenfalls ausgel�st um die Berechnung der Engine abzubrechen.
		 * 
		 */
		VOID stop();
		/**
		 * Gibt zur�ck, ob OWL-Matt die Berechnung bereits abgeschlossen hat bzw. auf Eingabe wartet.
		 * 
		 * \return F�r die Eingabe bereit?
		 */
		BOOL isReady() const;
		/**
		 * Zur Anpassung der Engine k�nnen �ber das UCI Engine-Optionen aktiviert werden.
		 * 
		 * \param option Optionstag
		 * \param value Wert
		 */
		VOID setOption(const std::string& option, const std::string& value);
		/**
		 * Gibt die Engine-Optionen zur�ck.
		 * 
		 * \return Engine-Optionen
		 */
		const EngineOptions& getOptions() const;
		/**
		 * Aktualisiere die Position in OWL-Matt.
		 * 
		 * \param position Neue Position
		 */
		VOID setPosition(const Position& position);
		/**
		 * Gebe die aktuelle Position zur�ck.
		 * 
		 * \return Aktuelle Position
		 */
		Position& getPosition();
		const Position& getPosition() const;
		/**
		 * Gebe den aktuellen Spieler der Engine zur�ck.
		 * 
		 * \return Aktuellen Spieler der Engine
		 */
		INT32 getPlayer() const;
		/**
		 * Die Anzahl der Abschneidungen der letzten Suche.
		 * Wird mit jeder Suche auf 0 zur�ck gesetzt.
		 * 
		 * \return Anzahl der Prunings
		 */
		INT32 getPrunesCount() const;
		/**
		 * Die Anzahl der durchsuchten Knoten der letzten Suche.
		 * Wird mit jeder Suche auf 0 zur�ck gesetzt.
		 * 
		 * \return Anzahl der besuchten Knoten
		 */
		INT32 getNodesCount() const;
		/**
		 * Die ben�tigte Zeit in Mikrosekunden, die die Zugfindung f�r die letzte Suche ben�tigt hat.
		 * 
		 * \return Ben�tigte Zeit in Mikrosekunden
		 */
		INT64 getSearchTime() const;
	private:
		/**
		 * Der Min-Max-Algorithmus samt seiner Bausteine f�r die Zugfindung zu einer Stellung.
		 * 
		 * \param position Die zu untersuchende Ausgangsstellung
		 * \param player Der Spieler, der in der Iteration am Zug ist (1 oder -1)
		 * \param depth Die aktuelle Tiefe (0 <= d <= m_startedDepth)
		 * \param alpha Die Alpha-Grenze f�r die Alpha-Beta-Suche
		 * \param beta Die Beta-Grenze f�r die Alpha-Beta-Suche
		 * \param parameterFlags Die Parameter zur Aktivierung der Bausteine die mit FT_* beginnen
		 * \return der aktuelle evaluierte Wert aus einer Iteration des Min-Max-Algorithmus
		 */
		EVALUATION_VALUE minMax(Position& position, INT32 player,
			INT32 depth, FLOAT alpha, FLOAT beta, 
			UCHAR parameterFlags);
		/**
		 * Funktion zur Sortierung der legalen Z�ge innerhalb der Iteration.
		 * 
		 * \param moves Der Zeiger auf die Liste der Z�ge, die sortiert werden soll
		 * \param position Die aktuelle Position
		 * \param depth Die momentane Tiefe der Iteration f�r den Vergleich der Killerz�ge (Killer-Heuristik)
		 * \param parameterFlags Bausteine mit FT_SRT_*, die f�r die Sortierung aktiviert werden sollen
		 */
		VOID sortMoves(MOVE_LIST* moves, Position& position, INT32 depth, 
			UCHAR parameterFlags);
		/**
		 * Der Versuch des Hinzuf�gens eines neuen Killerzugs.
		 * Die �berpr�fung, ob der Zug einer Killerzug ist, wird in der Funktion ausgef�hrt.
		 * \param move Der neue Zug, der in Frage kommt
		 * \param ply die aktuelle Suchtiefe aus der Iteration des Min-Max-Algorithmus
		 */
		VOID insertKiller(const Move move, const INT32 ply);
		/**
		 * Funktion die einen Zug auf Killer-Zug �berpr�ft und eine Priorit�t zur�ckliefert.
		 * 
		 * \param move Der Zug, der mit den Killerz�gen verglichen werden soll.
		 * \param ply Die Suchtiefe, in der nach den Killerz�gen verglichen werden soll.
		 * \return Der Priorit�tswert: KILLER_NO_PRIO = 0, KILLER_PRIO_2 = 1, KILLER_PRIO_1 = 2
		 */
		INT32 compareKiller(const Move move, const INT32 ply);

		/**
		 * Berechne den Wert nach MVV-LVA der Figuren zwischen Angreifer und Opfer.
		 * 
		 * \param attacker Angreifer-Figur
		 * \param victim Geschlagene Figur (Opfer)
		 * \return Korrespondierende Wert 
		 */
		static Captures getCaptureValue(CHAR attacker, CHAR victim);

		KILLER_LIST m_killerList;		// Killerz�ge
		Position m_position;			// Aktuelle Position der Engine
		EngineOptions m_engineOptions;	// Engine-Optionen
		RepitionMap m_repitionMap;		// Daten f�r Stellungswiederholung
		MinMaxResult m_result;			// Min-Max-Result
		std::mutex m_mutex;				// Mutex-Objekt zum Blockieren des Threads (Zur Speicherung weiterer Member)

		INT32 m_startedDepth = 0;		// Starttiefe
		INT32 m_player;					// Aktueller Spieler der Engine

		// Effizienzmerkmale:
		INT32 m_nodes;			// Anzahl der durchsuchten Knoten pro Suche
		INT32 m_prunes;			// Anzahl der Abschneidungen pro Suche
		INT64 m_searchTime;		// Zeit f�r die Suche in Mikrosekunden

		BOOL m_debugMode = false;	// Debug-Modus
		BOOL m_ready = true;		// Engine wartet auf Input
		BOOL m_stop = false;		// Soll Suche abgebrochen werden?
	};
}
