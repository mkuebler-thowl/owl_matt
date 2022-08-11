#include "Position.hpp"
#include <cassert>
#include "ChessEngine.hpp"
#include <iostream>
#include "ChessEvaluation.hpp"

namespace owl
{

	Position::Position()
		: m_enPassant(false), m_plyCount(0), m_moveNumber(1), m_player(PLAYER_WHITE), m_state(GameState::Active), 
		m_whiteCastlingShort(true), m_whiteCastlingLong(true), m_blackCastlingShort(true), m_blackCastlingLong(true),
		m_gamePhase(GamePhase::Opening), m_movedFirstTime(HAS_NOT_MOVED_BIT)
	{
		// 2D Array mit 'whitespace' initialisieren
		for (auto i = FIRST_ROW_INDEX; i < ROWS; i++)
		{
			for (auto j = FIRST_COLUMN_INDEX; j < COLUMNS; j++)
			{
				m_data[i][j] = EMPTY_PLACE;
			}
		}

		m_moveDataStack.push({ Move{}, EMPTY_PLACE, EMPTY_PLACE, false, m_enPassantPosition, {false,0}, m_movedFirstTime });
	}

	Position::~Position()
	{

	}

	Position::Position(const BoardArray& data, INT16 player,
		BOOL whiteCastlingShort, BOOL whiteCastlingLong, 
		BOOL blackCastlingShort, BOOL blackCastlingLong, 
		BOOL enPassant, std::pair<UINT16, UINT16> enPassantPosition,
		UINT16 moveCount, UINT16 moveNumber)
		: m_data(data), m_player(player), 
		m_whiteCastlingShort(whiteCastlingShort), m_whiteCastlingLong(whiteCastlingLong),
		m_blackCastlingShort(blackCastlingShort), m_blackCastlingLong(blackCastlingLong),
		m_enPassant(enPassant), m_enPassantPosition(enPassantPosition), 
		m_plyCount(moveCount), m_moveNumber(moveNumber), m_gamePhase(GamePhase::Opening), m_state(GameState::Active),
		m_movedFirstTime(HAS_NOT_MOVED_BIT)
	{
		if (m_data[FIRST_ROW_INDEX][FIRST_COLUMN_INDEX] != ROOK_BLACK) m_movedFirstTime |= HAS_BLACK_ROOK_L_MOVED_BIT;
		if (m_data[FIRST_ROW_INDEX][LAST_COLUMN_INDEX] != ROOK_BLACK) m_movedFirstTime |= HAS_BLACK_ROOK_R_MOVED_BIT;
		if (m_data[LAST_ROW_INDEX][LAST_COLUMN_INDEX] != ROOK_WHITE) m_movedFirstTime |= HAS_WHITE_ROOK_L_MOVED_BIT;
		if (m_data[LAST_ROW_INDEX][LAST_COLUMN_INDEX] != ROOK_WHITE) m_movedFirstTime |= HAS_WHITE_ROOK_R_MOVED_BIT;
		if (m_data[FIRST_ROW_INDEX][KING_START_X] != KING_BLACK) m_movedFirstTime |= HAS_BLACK_KING_MOVED_BIT;
		if (m_data[LAST_ROW_INDEX][KING_START_X] != KING_WHITE) m_movedFirstTime |= HAS_WHITE_KING_MOVED_BIT;

		m_moveDataStack.push({ Move{}, EMPTY_PLACE, EMPTY_PLACE, false, m_enPassantPosition, {false,0}, m_movedFirstTime });
	}

	owl::BoardLine& Position::operator[](INT32 index) const
	{
		assert(index < COLUMNS);
		return m_data[index];
	}

	VOID Position::applyMove(const Move& move)
	{	
		// Optional TODO: Zug validieren 

		auto move_data = MoveData{move};

		move_data.piece = m_data[move.startY][move.startX];
		move_data.capturedPiece = m_data[move.targetY][move.targetX];

		// Position aktualisieren: Startfeld -> Zielfeld
		m_data[move.targetY][move.targetX] = move_data.piece;
		m_data[move.startY][move.startX] = EMPTY_PLACE;

		// Rochade? Turm ebenfalls bewegen
		if (move.castlingLong)
		{
			auto rook = m_data[move.startY][FIRST_COLUMN_INDEX];
			m_data[move.startY][FIRST_COLUMN_INDEX] = EMPTY_PLACE;
			m_data[move.startY][CASTLING_ROOK_LEFT_X] = rook;
		}
		else if (move.castlingShort)
		{
			auto rook = m_data[move.startY][LAST_COLUMN_INDEX];
			m_data[move.startY][LAST_COLUMN_INDEX] = EMPTY_PLACE;
			m_data[move.startY][CASTLING_ROOK_RIGHT_X] = rook;
		}

		// En Passant Capturee eingelöst? Richtiges Feld löschen
		if (move.enPassantCapture && (move.targetY == EN_PASSANT_WHITE_Y || move.targetY == EN_PASSANT_BLACK_Y))
		{
			m_data[move.startY][move.targetX] = EMPTY_PLACE;

		}

		if (move_data.piece != PAWN_WHITE && move_data.piece != PAWN_BLACK && !move.capture) addPlyCount();
		else
		{
			move_data.plyCountReset = { true, m_plyCount };
			resetPlyCount();
		}

		// En Passant aktiv? zurücksetzen;
		if (isEnPassant())
		{
			resetEnPassant();
			move_data.enPassantFlag = true;
			move_data.enPassantPos = m_enPassantPosition;
		}
		else
		{
			move_data.enPassantFlag = false;
		}

		// Ist der Zug ein En Passant?
		if ((move_data.piece == PAWN_BLACK 
			&& move.startY == PAWN_DOUBLE_MOVE_START_BLACK_Y 
			&& move.targetY == PAWN_DOUBLE_MOVE_TARGET_BLACK_Y)
			|| (move_data.piece == PAWN_WHITE 
				&& move.startY == PAWN_DOUBLE_MOVE_START_WHITE_Y 
				&& move.targetY == PAWN_DOUBLE_MOVE_TARGET_WHITE_Y))
		{
			auto pawn_direction = move_data.piece == PAWN_WHITE ? PAWN_DIRECTION_WHITE : PAWN_DIRECTION_BLACK;
			setEnPassant(move.startX, move.startY + pawn_direction);
		}

		// 50-Züge Grenze überprüfen
		if (m_plyCount >= MAX_PLIES_SINCE_NO_MOVING_PAWNS_AND_CAPTURES) setGameState(GameState::Remis);

		// Turm oder König bewegt? 
		if (move_data.piece == KING_WHITE)
		{
			resetWhiteCastlingLong();
			resetWhiteCastlingShort();
			checkFirstMovement(HAS_WHITE_KING_MOVED_BIT, move_data.movedFirstTimeFlag);
		}
		else if (move_data.piece == KING_BLACK)
		{
			resetBlackCastlingShort();
			resetBlackCastlingLong();
			checkFirstMovement(HAS_BLACK_KING_MOVED_BIT, move_data.movedFirstTimeFlag);
		}
		else if (move_data.piece == ROOK_BLACK && move.startX == FIRST_COLUMN_INDEX)
		{
			resetBlackCastlingLong();
			checkFirstMovement(HAS_BLACK_ROOK_R_MOVED_BIT, move_data.movedFirstTimeFlag);
		}
		else if (move_data.piece == ROOK_BLACK && move.startX == LAST_COLUMN_INDEX)
		{
			resetBlackCastlingShort();
			checkFirstMovement(HAS_BLACK_ROOK_L_MOVED_BIT, move_data.movedFirstTimeFlag);
		}
		else if (move_data.piece == ROOK_WHITE && move.startX == FIRST_COLUMN_INDEX)
		{
			resetWhiteCastlingLong();
			checkFirstMovement(HAS_WHITE_ROOK_L_MOVED_BIT, move_data.movedFirstTimeFlag);
		}
		else if (move_data.piece == ROOK_WHITE && move.startX == LAST_COLUMN_INDEX)
		{
			resetWhiteCastlingShort();
			checkFirstMovement(HAS_WHITE_ROOK_R_MOVED_BIT, move_data.movedFirstTimeFlag);
		}

		// Promotion 
		if (move_data.move.promotion > 0) m_data[move_data.move.targetY][move_data.move.targetX] = m_player == PLAYER_WHITE ? 
			move_data.move.promotion : 
			std::tolower(move_data.move.promotion);

		// Spieler für Halbzug ändern
		changePlayer();

		m_moveDataStack.push(move_data);
	}

	VOID Position::undoLastMove()
	{
		if (m_moveDataStack.size() == 1) return;

		auto& last_move_data = m_moveDataStack.top();

		// Spieler wieder wechseln
		changePlayer(true);

		// Mögliche Promotion rückgängig machen
		if (last_move_data.move.promotion > 0) m_data[last_move_data.move.targetY][last_move_data.move.targetX] = m_player == PLAYER_WHITE ?
			PAWN_WHITE :
			PAWN_BLACK;

		// Turm oder König wurde das erste mal bewegt:
		if (last_move_data.movedFirstTimeFlag > HAS_NOT_MOVED_BIT)
		{
			auto& movedFlag = last_move_data.movedFirstTimeFlag;

			// Falls der schwarze König im letzten Zug bewegt wurde
			if (movedFlag & HAS_BLACK_KING_MOVED_BIT)
			{
				// Prüfe ob die Türme noch nicht bewegt wurden und aktiviere entsprechend das castling
				if (!(m_movedFirstTime & HAS_BLACK_ROOK_L_MOVED_BIT)) m_blackCastlingLong = true;
				if (!(m_movedFirstTime & HAS_BLACK_ROOK_R_MOVED_BIT)) m_blackCastlingShort = true;
			}
			// Falls der weiße König im letzten Zug bewegt wurde
			else if (movedFlag & HAS_WHITE_KING_MOVED_BIT)
			{
				// Prüfe ob die Türme noch nicht bewegt wurden und aktiviere entsprechend das castling
				if (!(m_movedFirstTime & HAS_WHITE_ROOK_L_MOVED_BIT)) m_whiteCastlingLong = true;
				if (!(m_movedFirstTime & HAS_WHITE_ROOK_R_MOVED_BIT)) m_whiteCastlingShort = true;
			}
			// Falls der schwarze Außenturm links bewegt wurde & der schwarze König noch nicht bewegt wurde
			else if (movedFlag & HAS_BLACK_ROOK_L_MOVED_BIT && !(m_movedFirstTime & HAS_BLACK_KING_MOVED_BIT))
			{
				m_blackCastlingLong = true;
			}
			// Falls der schwarze Außenturm rechts bewegt wurde & der schwarze König noch nicht bewegt wurde
			else if (movedFlag & HAS_BLACK_ROOK_R_MOVED_BIT && !(m_movedFirstTime & HAS_BLACK_KING_MOVED_BIT))
			{
				m_blackCastlingShort = true;
			}
			// Falls der weiße Außenturm links bewegt wurde & der weiße König noch nicht bewegt wurde
			else if (movedFlag & HAS_WHITE_ROOK_L_MOVED_BIT && !(m_movedFirstTime & HAS_WHITE_KING_MOVED_BIT))
			{
				m_whiteCastlingLong = true;
			}
			// Falls der weiße Außenturm rechts bewegt wurde & der weiße König noch nicht bewegt wurde
			else if (movedFlag & HAS_WHITE_ROOK_R_MOVED_BIT && !(m_movedFirstTime & HAS_WHITE_KING_MOVED_BIT))
			{
				m_whiteCastlingShort = true;
			}

			m_movedFirstTime -= movedFlag; // Bewegungsflag zurücksetzen
		}
		
		// 50-Züge Regel rückgängig machen
		if (m_plyCount > 0) m_plyCount--;
		if (m_plyCount < MAX_PLIES_SINCE_NO_MOVING_PAWNS_AND_CAPTURES && m_state == GameState::Remis)
		{
			m_state = GameState::Active;
		}

		// War vor dem Zug ein En Passant? 
		if (last_move_data.enPassantFlag)
		{
			m_enPassant = true;
			m_enPassantPosition = last_move_data.enPassantPos;
		}

		// War vorher schon ein En Passant: Setzte En Passant zurück
		else if (isEnPassant()) resetEnPassant();

		// War vorher ein Ply-Count-Reset?
		if (last_move_data.plyCountReset.first)
		{
			m_plyCount = last_move_data.plyCountReset.second;
		}

		// Wurde vorher ein En Passant Capture gespielt? (Setze den Bauer wieder zurück)
		if (last_move_data.move.enPassantCapture)
		{
			m_data[last_move_data.enPassantPos.second][last_move_data.enPassantPos.first] = last_move_data.piece == PAWN_WHITE ? PAWN_BLACK : PAWN_WHITE;
		}

		// lange Rochade durchgeführt? 
		if (last_move_data.move.castlingLong)
		{
			// Turm zurückbewegen
			auto rook = m_data[last_move_data.move.startY][CASTLING_ROOK_LEFT_X];
			m_data[last_move_data.move.startY][CASTLING_ROOK_LEFT_X] = EMPTY_PLACE;
			m_data[last_move_data.move.startY][FIRST_COLUMN_INDEX] = rook;

			if (m_player == PLAYER_WHITE) m_whiteCastlingLong = true;
			else m_blackCastlingLong = true;
		}

		// Oder kurze Rochade durchgeführt? 
		else if (last_move_data.move.castlingShort)
		{
			// Turm zurückbewegen
			auto rook = m_data[last_move_data.move.startY][CASTLING_ROOK_RIGHT_X];
			m_data[last_move_data.move.startY][CASTLING_ROOK_RIGHT_X] = EMPTY_PLACE;
			m_data[last_move_data.move.startY][LAST_COLUMN_INDEX] = rook;

			if (m_player == PLAYER_WHITE) m_whiteCastlingShort = true;
			else m_blackCastlingShort = true;
		}

		// Position wieder zurücksetzen:
		m_data[last_move_data.move.startY][last_move_data.move.startX] = last_move_data.piece;
		m_data[last_move_data.move.targetY][last_move_data.move.targetX] = last_move_data.capturedPiece;

		// Zug vom Stack entfernen
		m_moveDataStack.pop();
	}

	VOID Position::setEnPassant(INT32 x, INT32 y)
	{
		m_enPassant = true;
		m_enPassantPosition = { x,y };
	}

	VOID Position::resetEnPassant()
	{
		m_enPassant = false;
		m_enPassantPosition = { 0,0 };
	}

	BOOL Position::isEnPassant() const
	{
		return m_enPassant;
	}
	const std::pair<INT32, INT32>& Position::getEnPassant() const
	{
		return m_enPassantPosition;
	}
	VOID Position::addPlyCount()
	{
		m_plyCount++;
	}
	VOID Position::resetPlyCount()
	{
		m_plyCount = 0;
	}
	UINT16 Position::getPlyCount() const
	{
		return m_plyCount;
	}
	UINT16 Position::getMoveNumber() const
	{
		return m_moveNumber;
	}
	VOID Position::changePlayer(BOOL undoMove)
	{
		// Zugnummer erhöhen
		if (m_player == PLAYER_BLACK && !undoMove) m_moveNumber++;
		else if (m_player == PLAYER_WHITE && undoMove) m_moveNumber--;
		// Spieler wechseln
		m_player = m_player == PLAYER_WHITE ? PLAYER_BLACK : PLAYER_WHITE;
	}
	INT16 Position::getPlayer() const
	{
		return m_player;
	}
	VOID Position::setGameState(GameState state) const
	{
		m_state = state;
	}

	GameState Position::getGameState() const
	{
		return m_state;
	}
	BOOL Position::getWhiteCastlingShort() const
	{
		return m_whiteCastlingShort;
	}
	BOOL Position::getWhiteCastlingLong() const
	{
		return m_whiteCastlingLong;
	}
	BOOL Position::getBlackCastlingShort() const
	{
		return m_blackCastlingShort;
	}
	BOOL Position::getBlackCastlingLong() const
	{
		return m_blackCastlingLong;
	}
	VOID Position::resetWhiteCastlingShort()
	{
		m_whiteCastlingShort = false;
	}
	VOID Position::resetWhiteCastlingLong()
	{
		m_whiteCastlingLong = false;
	}
	VOID Position::resetBlackCastlingShort()
	{
		m_blackCastlingShort = false;
	}
	VOID Position::resetBlackCastlingLong()
	{
		m_blackCastlingLong = false;
	}
	VOID Position::enterNextGamePhase() const
	{
		if (m_gamePhase == GamePhase::Mid) m_gamePhase = GamePhase::End;
		else if (m_gamePhase == GamePhase::Opening) m_gamePhase = GamePhase::Mid;
	}

	GamePhase Position::getGamePhase() const
	{
		return m_gamePhase;
	}
	VOID Position::printPosition(const Position& position)
	{
		std::string out = "";

		for (auto i = 0; i < ROWS; i++)
		{
			for (auto j = 0; j < COLUMNS; j++)
			{
				out += position[i][j];
			}
			out += "\n";
		}

		std::cout << out;
	}

	VOID Position::checkFirstMovement(UCHAR check, UCHAR& movedFirstTime)
	{
		if (!(m_movedFirstTime & check)) 
		{
			movedFirstTime |= check;
			m_movedFirstTime |= check;
		}
	}

}