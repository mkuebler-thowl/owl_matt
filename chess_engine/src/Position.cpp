#include "Position.hpp"
#include <cassert>
#include "ChessEngine.hpp"
#include <iostream>
#include "ChessEvaluation.hpp"
#include "ChessUtility.hpp"

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
				m_data[i][j] = EMPTY_FIELD;
			}
		}

		m_kingPosition[FIRST][WHITE_INDEX] = 0;
		m_kingPosition[SECOND][WHITE_INDEX] = 0;
		m_kingPosition[FIRST][BLACK_INDEX] = 0;
		m_kingPosition[SECOND][BLACK_INDEX] = 0;

		m_moveDataStack.push({ Move{}, EMPTY_FIELD, EMPTY_FIELD, false, m_enPassantPosition, {false,0}, m_movedFirstTime });
	}

	Position::~Position()
	{

	}

	Position::Position(const BOARD_ARRAY& data, INT32 player,
		BOOL whiteCastlingShort, BOOL whiteCastlingLong, 
		BOOL blackCastlingShort, BOOL blackCastlingLong, 
		BOOL enPassant, PAIR<INT32, INT32> enPassantPosition,
		INT32 moveCount, INT32 moveNumber)
		: m_data(data), m_player(player), 
		m_whiteCastlingShort(whiteCastlingShort), m_whiteCastlingLong(whiteCastlingLong),
		m_blackCastlingShort(blackCastlingShort), m_blackCastlingLong(blackCastlingLong),
		m_enPassant(enPassant), m_enPassantPosition(enPassantPosition), 
		m_plyCount(moveCount), m_moveNumber(moveNumber), m_gamePhase(GamePhase::Opening), m_state(GameState::Active),
		m_movedFirstTime(HAS_NOT_MOVED_BIT)
	{
		if (m_data[FIRST_ROW_INDEX][FIRST_COLUMN_INDEX] != BLACK_ROOK) m_movedFirstTime |= HAS_BLACK_ROOK_L_MOVED_BIT;
		if (m_data[FIRST_ROW_INDEX][LAST_COLUMN_INDEX] != BLACK_ROOK) m_movedFirstTime |= HAS_BLACK_ROOK_R_MOVED_BIT;
		if (m_data[LAST_ROW_INDEX][LAST_COLUMN_INDEX] != WHITE_ROOK) m_movedFirstTime |= HAS_WHITE_ROOK_L_MOVED_BIT;
		if (m_data[LAST_ROW_INDEX][LAST_COLUMN_INDEX] != WHITE_ROOK) m_movedFirstTime |= HAS_WHITE_ROOK_R_MOVED_BIT;
		if (m_data[FIRST_ROW_INDEX][KING_START_X] != BLACK_KING) m_movedFirstTime |= HAS_BLACK_KING_MOVED_BIT;
		if (m_data[LAST_ROW_INDEX][KING_START_X] != WHITE_KING) m_movedFirstTime |= HAS_WHITE_KING_MOVED_BIT;

		m_moveDataStack.push({ Move{}, EMPTY_FIELD, EMPTY_FIELD, false, m_enPassantPosition, {false,0}, m_movedFirstTime });

		calculateKingPositions();

#if DEBUG
		M_FEN = ChessUtility::positionToFen(*this);
#endif
	}

	owl::BOARD_LINE& Position::operator[](INT32 index) const
	{
		assert(index < COLUMNS);
		return m_data[index];
	}

	VOID Position::applyMove(const Move& move)
	{	
		auto move_data = MoveData{move};
		move_data.lastGamePhase = m_gamePhase;

		move_data.piece = m_data[move.startY][move.startX];
		move_data.capturedPiece = m_data[move.targetY][move.targetX];

		// Position aktualisieren: Startfeld -> Zielfeld
		m_data[move.targetY][move.targetX] = move_data.piece;
		m_data[move.startY][move.startX] = EMPTY_FIELD;

		// Rochade? Turm ebenfalls bewegen
		if (move.castlingLong)
		{
			auto rook = m_data[move.startY][FIRST_COLUMN_INDEX];
			m_data[move.startY][FIRST_COLUMN_INDEX] = EMPTY_FIELD;
			m_data[move.startY][CASTLING_ROOK_LEFT_X] = rook;
		}
		else if (move.castlingShort)
		{
			auto rook = m_data[move.startY][LAST_COLUMN_INDEX];
			m_data[move.startY][LAST_COLUMN_INDEX] = EMPTY_FIELD;
			m_data[move.startY][CASTLING_ROOK_RIGHT_X] = rook;
		}

		// En Passant Capturee eingel�st? Richtiges Feld l�schen
		if (move.enPassantCapture && (move.targetY == EN_PASSANT_WHITE_Y || move.targetY == EN_PASSANT_BLACK_Y))
		{
			m_data[move.startY][move.targetX] = EMPTY_FIELD;
		}

		if (move_data.piece != WHITE_PAWN && move_data.piece != BLACK_PAWN && !move.capture) addPlyCount();
		else
		{
			move_data.plyCountReset = { true, m_plyCount };
			resetPlyCount();
		}

		// En Passant aktiv? zur�cksetzen;
		if (isEnPassant())
		{
			move_data.enPassantFlag = true;
			move_data.enPassantPos = m_enPassantPosition;
			resetEnPassant();

		}
		else
		{
			move_data.enPassantFlag = false;
		}

		// Ist der Zug ein En Passant?
		if ((move_data.piece == BLACK_PAWN 
			&& move.startY == PAWN_DOUBLE_MOVE_START_BLACK_Y 
			&& move.targetY == PAWN_DOUBLE_MOVE_TARGET_BLACK_Y)
			|| (move_data.piece == WHITE_PAWN 
				&& move.startY == PAWN_DOUBLE_MOVE_START_WHITE_Y 
				&& move.targetY == PAWN_DOUBLE_MOVE_TARGET_WHITE_Y))
		{
			auto pawn_direction = move_data.piece == WHITE_PAWN ? PAWN_DIRECTION_WHITE : PAWN_DIRECTION_BLACK;
			setEnPassant(move.startX, move.startY + pawn_direction);
		}

		// 50-Z�ge Grenze �berpr�fen
		if (m_plyCount >= MAX_PLIES_SINCE_NO_MOVING_PAWNS_AND_CAPTURES) setGameState(GameState::Remis);

		// Turm oder K�nig bewegt? 
		if (move_data.piece == WHITE_KING)
		{
			resetWhiteCastlingLong();
			resetWhiteCastlingShort();
			checkFirstMovement(HAS_WHITE_KING_MOVED_BIT, move_data.movedFirstTimeFlag);
			setKingPosition(WHITE_INDEX, move_data.move.targetX, move_data.move.targetY);
		}
		else if (move_data.piece == BLACK_KING)
		{
			resetBlackCastlingShort();
			resetBlackCastlingLong();
			checkFirstMovement(HAS_BLACK_KING_MOVED_BIT, move_data.movedFirstTimeFlag);
			setKingPosition(BLACK_INDEX, move_data.move.targetX, move_data.move.targetY);
		}
		else if (move_data.piece == BLACK_ROOK && move.startX == FIRST_COLUMN_INDEX)
		{
			resetBlackCastlingLong();
			checkFirstMovement(HAS_BLACK_ROOK_L_MOVED_BIT, move_data.movedFirstTimeFlag);
		}
		else if (move_data.piece == BLACK_ROOK && move.startX == LAST_COLUMN_INDEX)
		{
			resetBlackCastlingShort();
			checkFirstMovement(HAS_BLACK_ROOK_R_MOVED_BIT, move_data.movedFirstTimeFlag);
		}
		else if (move_data.piece == WHITE_ROOK && move.startX == FIRST_COLUMN_INDEX)
		{
			resetWhiteCastlingLong();
			checkFirstMovement(HAS_WHITE_ROOK_L_MOVED_BIT, move_data.movedFirstTimeFlag);
		}
		else if (move_data.piece == WHITE_ROOK && move.startX == LAST_COLUMN_INDEX)
		{
			resetWhiteCastlingShort();
			checkFirstMovement(HAS_WHITE_ROOK_R_MOVED_BIT, move_data.movedFirstTimeFlag);
		}

		// Promotion 
		if (move_data.move.promotion > 0) m_data[move_data.move.targetY][move_data.move.targetX] = m_player == PLAYER_WHITE ? 
			std::toupper(move_data.move.promotion) : 
			std::tolower(move_data.move.promotion);

		// Spieler f�r Halbzug �ndern
		changePlayer();

		m_moveDataStack.push(move_data);

#if DEBUG
		M_FEN = ChessUtility::positionToFen(*this);
#endif
	}

	VOID Position::undoLastMove()
	{
		if (m_moveDataStack.size() == 1) return;

		auto& last_move_data = m_moveDataStack.top();
		m_gamePhase = last_move_data.lastGamePhase;

		// Spieler wieder wechseln
		changePlayer(true);

		// M�gliche Promotion r�ckg�ngig machen
		if (last_move_data.move.promotion > 0) m_data[last_move_data.move.targetY][last_move_data.move.targetX] = m_player == PLAYER_WHITE ?
			WHITE_PAWN :
			BLACK_PAWN;

		// Turm oder K�nig wurde das erste mal bewegt:
		if (last_move_data.movedFirstTimeFlag > HAS_NOT_MOVED_BIT)
		{
			auto& movedFlag = last_move_data.movedFirstTimeFlag;

			// Falls der schwarze K�nig im letzten Zug bewegt wurde
			if (movedFlag & HAS_BLACK_KING_MOVED_BIT)
			{
				// Pr�fe ob die T�rme noch nicht bewegt wurden und aktiviere entsprechend das castling
				if (!(m_movedFirstTime & HAS_BLACK_ROOK_L_MOVED_BIT)) m_blackCastlingLong = true;
				if (!(m_movedFirstTime & HAS_BLACK_ROOK_R_MOVED_BIT)) m_blackCastlingShort = true;
			}
			// Falls der wei�e K�nig im letzten Zug bewegt wurde
			else if (movedFlag & HAS_WHITE_KING_MOVED_BIT)
			{
				// Pr�fe ob die T�rme noch nicht bewegt wurden und aktiviere entsprechend das castling
				if (!(m_movedFirstTime & HAS_WHITE_ROOK_L_MOVED_BIT)) m_whiteCastlingLong = true;
				if (!(m_movedFirstTime & HAS_WHITE_ROOK_R_MOVED_BIT)) m_whiteCastlingShort = true;
			}
			// Falls der schwarze Au�enturm links bewegt wurde & der schwarze K�nig noch nicht bewegt wurde
			else if (movedFlag & HAS_BLACK_ROOK_L_MOVED_BIT && !(m_movedFirstTime & HAS_BLACK_KING_MOVED_BIT))
			{
				m_blackCastlingLong = true;
			}
			// Falls der schwarze Au�enturm rechts bewegt wurde & der schwarze K�nig noch nicht bewegt wurde
			else if (movedFlag & HAS_BLACK_ROOK_R_MOVED_BIT && !(m_movedFirstTime & HAS_BLACK_KING_MOVED_BIT))
			{
				m_blackCastlingShort = true;
			}
			// Falls der wei�e Au�enturm links bewegt wurde & der wei�e K�nig noch nicht bewegt wurde
			else if (movedFlag & HAS_WHITE_ROOK_L_MOVED_BIT && !(m_movedFirstTime & HAS_WHITE_KING_MOVED_BIT))
			{
				m_whiteCastlingLong = true;
			}
			// Falls der wei�e Au�enturm rechts bewegt wurde & der wei�e K�nig noch nicht bewegt wurde
			else if (movedFlag & HAS_WHITE_ROOK_R_MOVED_BIT && !(m_movedFirstTime & HAS_WHITE_KING_MOVED_BIT))
			{
				m_whiteCastlingShort = true;
			}

			m_movedFirstTime -= movedFlag; // Bewegungsflag zur�cksetzen
		}
		
		// 50-Z�ge Regel r�ckg�ngig machen
		if (m_plyCount > 0) m_plyCount--;

		// War vor dem Zug ein En Passant? 
		if (last_move_data.enPassantFlag)
		{
			m_enPassant = true;
			m_enPassantPosition = last_move_data.enPassantPos;
		}

		// War vorher schon ein En Passant: Setzte En Passant zur�ck
		else if (isEnPassant()) resetEnPassant();

		// War vorher ein Ply-Count-Reset?
		if (last_move_data.plyCountReset.first)
		{
			m_plyCount = last_move_data.plyCountReset.second;
		}

		// Wurde vorher ein En Passant Capture gespielt? (Setze den Bauer wieder zur�ck)
		if (last_move_data.move.enPassantCapture)
		{
			if (last_move_data.piece == WHITE_PAWN)
			{
				m_data[last_move_data.enPassantPos.second+ONE_FIELD_DOWN][last_move_data.enPassantPos.first] = BLACK_PAWN;
			}
			else
			{
				m_data[last_move_data.enPassantPos.second+ONE_FIELD_UP][last_move_data.enPassantPos.first] = WHITE_PAWN;
			}
		}

		// lange Rochade durchgef�hrt? 
		if (last_move_data.move.castlingLong)
		{
			// Turm zur�ckbewegen
			auto rook = m_data[last_move_data.move.startY][CASTLING_ROOK_LEFT_X];
			m_data[last_move_data.move.startY][CASTLING_ROOK_LEFT_X] = EMPTY_FIELD;
			m_data[last_move_data.move.startY][FIRST_COLUMN_INDEX] = rook;

			if (m_player == PLAYER_WHITE) m_whiteCastlingLong = true;
			else m_blackCastlingLong = true;
		}

		// Oder kurze Rochade durchgef�hrt? 
		else if (last_move_data.move.castlingShort)
		{
			// Turm zur�ckbewegen
			auto rook = m_data[last_move_data.move.startY][CASTLING_ROOK_RIGHT_X];
			m_data[last_move_data.move.startY][CASTLING_ROOK_RIGHT_X] = EMPTY_FIELD;
			m_data[last_move_data.move.startY][LAST_COLUMN_INDEX] = rook;

			if (m_player == PLAYER_WHITE) m_whiteCastlingShort = true;
			else m_blackCastlingShort = true;
		}

		// Falls der K�nig zuvor bewegt wurde, setzte die K�nigsposition zur�ck:
		if (last_move_data.piece == WHITE_KING) setKingPosition(WHITE_INDEX, last_move_data.move.startX, last_move_data.move.startY);
		else if (last_move_data.piece == BLACK_KING) setKingPosition(BLACK_INDEX, last_move_data.move.startX, last_move_data.move.startY);

		// Position wieder zur�cksetzen:
		m_data[last_move_data.move.startY][last_move_data.move.startX] = last_move_data.piece;
		m_data[last_move_data.move.targetY][last_move_data.move.targetX] = last_move_data.capturedPiece;

		// Falls Endstellung:
		// Logik: Jeder Vogegangene Zug einer Endstellung ist keine Endstellung:
		// => Setzte GameState wieder auf Aktiv!
		setGameState(GameState::Active);

		// Zug vom Stack entfernen
		m_moveDataStack.pop();

#if DEBUG
		M_FEN = ChessUtility::positionToFen(*this);
#endif
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
	const PAIR<INT32, INT32>& Position::getEnPassant() const
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
	INT32 Position::getPlyCount() const
	{
		return m_plyCount;
	}
	INT32 Position::getMoveNumber() const
	{
		return m_moveNumber;
	}
	VOID Position::changePlayer(BOOL undoMove)
	{
		// Zugnummer erh�hen
		if (m_player == PLAYER_BLACK && !undoMove) m_moveNumber++;
		else if (m_player == PLAYER_WHITE && undoMove) m_moveNumber--;
		// Spieler wechseln
		m_player = m_player == PLAYER_WHITE ? PLAYER_BLACK : PLAYER_WHITE;
	}
	INT32 Position::getPlayer() const
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
	VOID Position::print() const
	{
		std::string out = "";

		for (auto i = 0; i < ROWS; i++)
		{
			for (auto j = 0; j < COLUMNS; j++)
			{
				out += m_data[i][j];
			}
			out += "\n";
		}

		std::cout << out;
	}

	PAIR<INT32, INT32> Position::getKingPosition(INT32 index) const
	{
		return { m_kingPosition[FIRST][index], m_kingPosition[SECOND][index] };
	}

	const std::stack<Position::MoveData>& Position::getMoveDataStack() const
	{
		return m_moveDataStack;
	}

	VOID Position::checkFirstMovement(UCHAR check, UCHAR& movedFirstTime)
	{
		if (!(m_movedFirstTime & check)) 
		{
			movedFirstTime |= check;
			m_movedFirstTime |= check;
		}
	}

	VOID Position::calculateKingPositions()
	{
		auto white_king_already_found = false;
		// Suche schwarzen K�nig von oben nach unten:
		// Gegebenfalls betrachten ob wei�er K�nig h�her steht als schwarzer K�nig
		for (auto y = FIRST_ROW_INDEX; y < ROWS; y++)
		{
			for (auto x = FIRST_COLUMN_INDEX; x < COLUMNS; x++)
			{
				auto piece = m_data[y][x];

				if (piece == BLACK_KING)
				{
					setKingPosition(BLACK_INDEX, x, y);
					break;
				}
				else if (piece == WHITE_KING)
				{
					setKingPosition(WHITE_INDEX, x, y);
					white_king_already_found = true;
				}
			}
		}

		if (white_king_already_found) return;

		// Suche wei�en K�nig von unten nach oben - sofern noch nicht gefunden
		for (auto y = LAST_ROW_INDEX; y > ROWS; y--)
		{
			for (auto x = FIRST_COLUMN_INDEX; x < COLUMNS; x++)
			{
				auto piece = m_data[y][x];
				
				if (piece == WHITE_KING)
				{
					setKingPosition(WHITE_INDEX, x, y);
					break;
				}
			}
		}
	}

	VOID Position::setKingPosition(INT32 index, INT32 x, INT32 y)
	{
		m_kingPosition[FIRST][index] = x;
		m_kingPosition[SECOND][index] = y;
	}

}
