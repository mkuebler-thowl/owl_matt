#pragma once

#include <string>
#include <vector>

#include "../Position.hpp"
#include "../ChessUtility.hpp"
#include "../ChessEvaluation.hpp"

namespace owl
{
	namespace test
	{
		// Move-Liste von Kasparov gegen Topalov, Wijk aan Zee 1999:
		constexpr auto MASTER_GAME_MOVES_STR = {
			// w:	b:			// Notation:
			"e2e4", "d7d6",		// 1. e4 d6
			"d2d4", "g8f6",		// 2. d4 Nf6
			"b1c3", "g7g6",		// 3. Nc3 g6
			"c1e3", "f8g7",		// 4. Be3 Bg7
			"d1d2", "c7c6",		// 5. Qd2 c6
			"f2f3", "b7b5",		// 6. f3 b5
			"g1e2", "b8d7",		// 7. Nge2 Nbd7
			"e3h6", "g7xh6",	// 8. Bh6 Bxh6
			"d2xh6", "c8b7",	// 9. Qxh6 Bb7
			"a2a3", "e7e5",		// 10. a3 e5
			"e1c1", "d8e7",		// 11. 0-0-0 Qe7
			"c1b1", "a7a6",		// 12. Kb1 a6
			"e2c1", "e8c8",		// 13. Nc1 0-0-0
			"c1b3", "e5xd4",	// 14. Nb3 exd4
			"d1d4", "c6c5",		// 15. Rxd4 c5
			"d4d1", "d7b6",		// 16. Rd1 Nb6
			"g2g3", "c8b8",		// 17. g3 Kb8
			"b3a5", "b7a8",		// 18. Na5 Ba8
			"f1h3", "d6d5",		// 19. Bh3 d5
			"h6f4", "b8a7",		// 20. Qf4+ Ka7
			"h1e1", "d5d4",		// 21. Rhe1 d4
			"c3d5", "b6xd5",	// 22. Nd5 Nbxd5
			"e4xd5", "e7d6",	// 23. exd5 Qd6 
			"d1xd4", "c5xd4",	// 24. Rxd4 cxd4 
			"e1e7", "a7b6",		// 25. Re7+ Kb6
			"f4xd4", "b6xa5",	// 26. Qxd4+ Kxa5 
			"b2b4", "a5a4",		// 27. b4+ Ka4 
			"d4c3", "d6xd5",	// 28. Qc3 Qxd5 
			"e7a7", "a8b7",		// 29. Ra7 Bb7 
			"a7xb7", "d5c4",	// 30. Rxb7 Qc4 
			"c3xf6", "a4xa3",	// 31. Qxf6 Kxa3 
			"f6xa6", "a3xb4",	// 32. Qxa6+ Kxb4 
			"c2c3", "b4xc3",	// 33. c3+ Kxc3 
			"a6a1", "c3d2",		// 34. Qa1+ Kd2 
			"a1b2", "d2d1",		// 35. Qb2+ Kd1 
			"h3f1", "d8d2",		// 36. Bf1 Rd2 
			"b7d7", "d2xd7",	// 37. Rd7 Rxd7 
			"f1xc4", "b5xc4",	// 38. Bxc4 bxc4 
			"b2xh8", "d7d3",	// 39. Qxh8 Rd3 
			"h8a8", "c4c3",		// 40. Qa8 c3
			"a8a4", "d1e1",		// 41. Qa4+ Ke1 
			"f3f4", "f7f5",		// 42. f4 f5 
			"b1c1", "d3d2",		// 43. Kc1 Rd2 
			"a4a7",				// 44. Qa7 1-0
		};

		constexpr double ERROR_VAL = 7777.0;
		constexpr double RESULT_STOCKFISH_PHI_K[] =
		{
			0.28,0.21,0.6,0.56,0.62,0.47,0.66,0.5,0.6,0.42,0.64,0.55,0.62,0.23,0.38,
			0.2,-8.29,0.26,0.72,0.38,0.62,0.2,0.25,0.24,0.73,0.6,0.89,0.61,-0.03,
			0.5,0.17,0.18,0.27,0.15,-0.2,0.25,0.22,-0.07,-0.57,7777.0,0.21,-0.05,
			-0.23,1.02,-3.22,0.85,0.93,1.85,-1.59,7777.0,-0.23,7777.0,0.69,7777.0,
			2.42,1.68,1.45,1.12,1.77,1.33,1.75,0.65,-0.69,7777.0,2.53,7777.0,1.36,
			7777.0,-0.11,7777.0,0.9,-0.2,-0.08,-1.68,-9.66,4.36,-0.97,0.71,3.37,
			-0.01,1.07,7777.0,2.51,1.49,1.86,1.78,2.67,2.23,
		};

		constexpr auto MASTER_GAME_FENS = {
			"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",

			"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
			"rnbqkbnr/ppp1pppp/3p4/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2",

			"rnbqkbnr/ppp1pppp/3p4/8/3PP3/8/PPP2PPP/RNBQKBNR b KQkq d3 0 2",
			"rnbqkb1r/ppp1pppp/3p1n2/8/3PP3/8/PPP2PPP/RNBQKBNR w KQkq - 1 3",

			"rnbqkb1r/ppp1pppp/3p1n2/8/3PP3/2N5/PPP2PPP/R1BQKBNR b KQkq - 2 3",
			"rnbqkb1r/ppp1pp1p/3p1np1/8/3PP3/2N5/PPP2PPP/R1BQKBNR w KQkq - 0 4",

			"rnbqkb1r/ppp1pp1p/3p1np1/8/3PP3/2N1B3/PPP2PPP/R2QKBNR b KQkq - 1 4",
			"rnbqk2r/ppp1ppbp/3p1np1/8/3PP3/2N1B3/PPP2PPP/R2QKBNR w KQkq - 2 5",

			"rnbqk2r/ppp1ppbp/3p1np1/8/3PP3/2N1B3/PPPQ1PPP/R3KBNR b KQkq - 3 5",
			"rnbqk2r/pp2ppbp/2pp1np1/8/3PP3/2N1B3/PPPQ1PPP/R3KBNR w KQkq - 0 6",

			"rnbqk2r/pp2ppbp/2pp1np1/8/3PP3/2N1BP2/PPPQ2PP/R3KBNR b KQkq - 0 6",
			"rnbqk2r/p3ppbp/2pp1np1/1p6/3PP3/2N1BP2/PPPQ2PP/R3KBNR w KQkq b6 0 7",

			"rnbqk2r/p3ppbp/2pp1np1/1p6/3PP3/2N1BP2/PPPQN1PP/R3KB1R b KQkq - 1 7",
			"r1bqk2r/p2nppbp/2pp1np1/1p6/3PP3/2N1BP2/PPPQN1PP/R3KB1R w KQkq - 2 8",

			"r1bqk2r/p2nppbp/2pp1npB/1p6/3PP3/2N2P2/PPPQN1PP/R3KB1R b KQkq - 3 8",
			"r1bqk2r/p2npp1p/2pp1npb/1p6/3PP3/2N2P2/PPPQN1PP/R3KB1R w KQkq - 0 9",

			"r1bqk2r/p2npp1p/2pp1npQ/1p6/3PP3/2N2P2/PPP1N1PP/R3KB1R b KQkq - 0 9",
			"r2qk2r/pb1npp1p/2pp1npQ/1p6/3PP3/2N2P2/PPP1N1PP/R3KB1R w KQkq - 1 10",

			"r2qk2r/pb1npp1p/2pp1npQ/1p6/3PP3/P1N2P2/1PP1N1PP/R3KB1R b KQkq - 0 10",
			"r2qk2r/pb1n1p1p/2pp1npQ/1p2p3/3PP3/P1N2P2/1PP1N1PP/R3KB1R w KQkq e6 0 11",

			"r2qk2r/pb1n1p1p/2pp1npQ/1p2p3/3PP3/P1N2P2/1PP1N1PP/2KR1B1R b kq - 1 11",
			"r3k2r/pb1nqp1p/2pp1npQ/1p2p3/3PP3/P1N2P2/1PP1N1PP/2KR1B1R w kq - 2 12",

			"r3k2r/pb1nqp1p/2pp1npQ/1p2p3/3PP3/P1N2P2/1PP1N1PP/1K1R1B1R b kq - 3 12",
			"r3k2r/1b1nqp1p/p1pp1npQ/1p2p3/3PP3/P1N2P2/1PP1N1PP/1K1R1B1R w kq - 0 13",

			"r3k2r/1b1nqp1p/p1pp1npQ/1p2p3/3PP3/P1N2P2/1PP3PP/1KNR1B1R b kq - 1 13",
			"2kr3r/1b1nqp1p/p1pp1npQ/1p2p3/3PP3/P1N2P2/1PP3PP/1KNR1B1R w - - 2 14",

			"2kr3r/1b1nqp1p/p1pp1npQ/1p2p3/3PP3/PNN2P2/1PP3PP/1K1R1B1R b - - 3 14",
			"2kr3r/1b1nqp1p/p1pp1npQ/1p6/3pP3/PNN2P2/1PP3PP/1K1R1B1R w - - 0 15",

			"2kr3r/1b1nqp1p/p1pp1npQ/1p6/3RP3/PNN2P2/1PP3PP/1K3B1R b - - 0 15",
			"2kr3r/1b1nqp1p/p2p1npQ/1pp5/3RP3/PNN2P2/1PP3PP/1K3B1R w - - 0 16",

			"2kr3r/1b1nqp1p/p2p1npQ/1pp5/4P3/PNN2P2/1PP3PP/1K1R1B1R b - - 1 16",
			"2kr3r/1b2qp1p/pn1p1npQ/1pp5/4P3/PNN2P2/1PP3PP/1K1R1B1R w - - 2 17",

			"2kr3r/1b2qp1p/pn1p1npQ/1pp5/4P3/PNN2PP1/1PP4P/1K1R1B1R b - - 0 17",
			"1k1r3r/1b2qp1p/pn1p1npQ/1pp5/4P3/PNN2PP1/1PP4P/1K1R1B1R w - - 1 18",

			"1k1r3r/1b2qp1p/pn1p1npQ/Npp5/4P3/P1N2PP1/1PP4P/1K1R1B1R b - - 2 18",
			"bk1r3r/4qp1p/pn1p1npQ/Npp5/4P3/P1N2PP1/1PP4P/1K1R1B1R w - - 3 19",

			"bk1r3r/4qp1p/pn1p1npQ/Npp5/4P3/P1N2PPB/1PP4P/1K1R3R b - - 4 19",
			"bk1r3r/4qp1p/pn3npQ/Nppp4/4P3/P1N2PPB/1PP4P/1K1R3R w - - 0 20",

			"bk1r3r/4qp1p/pn3np1/Nppp4/4PQ2/P1N2PPB/1PP4P/1K1R3R b - - 1 20",
			"b2r3r/k3qp1p/pn3np1/Nppp4/4PQ2/P1N2PPB/1PP4P/1K1R3R w - - 2 21",

			"b2r3r/k3qp1p/pn3np1/Nppp4/4PQ2/P1N2PPB/1PP4P/1K1RR3 b - - 3 21",
			"b2r3r/k3qp1p/pn3np1/Npp5/3pPQ2/P1N2PPB/1PP4P/1K1RR3 w - - 0 22",

			"b2r3r/k3qp1p/pn3np1/NppN4/3pPQ2/P4PPB/1PP4P/1K1RR3 b - - 1 22",
			"b2r3r/k3qp1p/p4np1/Nppn4/3pPQ2/P4PPB/1PP4P/1K1RR3 w - - 0 23",

			"b2r3r/k3qp1p/p4np1/NppP4/3p1Q2/P4PPB/1PP4P/1K1RR3 b - - 0 23",
			"b2r3r/k4p1p/p2q1np1/NppP4/3p1Q2/P4PPB/1PP4P/1K1RR3 w - - 1 24",

			"b2r3r/k4p1p/p2q1np1/NppP4/3R1Q2/P4PPB/1PP4P/1K2R3 b - - 0 24",
			"b2r3r/k4p1p/p2q1np1/Np1P4/3p1Q2/P4PPB/1PP4P/1K2R3 w - - 0 25",

			"b2r3r/k3Rp1p/p2q1np1/Np1P4/3p1Q2/P4PPB/1PP4P/1K6 b - - 1 25",
			"b2r3r/4Rp1p/pk1q1np1/Np1P4/3p1Q2/P4PPB/1PP4P/1K6 w - - 2 26",

			"b2r3r/4Rp1p/pk1q1np1/Np1P4/3Q4/P4PPB/1PP4P/1K6 b - - 0 26",
			"b2r3r/4Rp1p/p2q1np1/kp1P4/3Q4/P4PPB/1PP4P/1K6 w - - 0 27",

			"b2r3r/4Rp1p/p2q1np1/kp1P4/1P1Q4/P4PPB/2P4P/1K6 b - b3 0 27",
			"b2r3r/4Rp1p/p2q1np1/1p1P4/kP1Q4/P4PPB/2P4P/1K6 w - - 1 28",

			"b2r3r/4Rp1p/p2q1np1/1p1P4/kP6/P1Q2PPB/2P4P/1K6 b - - 2 28",
			"b2r3r/4Rp1p/p4np1/1p1q4/kP6/P1Q2PPB/2P4P/1K6 w - - 0 29",

			"b2r3r/R4p1p/p4np1/1p1q4/kP6/P1Q2PPB/2P4P/1K6 b - - 1 29",
			"3r3r/Rb3p1p/p4np1/1p1q4/kP6/P1Q2PPB/2P4P/1K6 w - - 2 30",

			"3r3r/1R3p1p/p4np1/1p1q4/kP6/P1Q2PPB/2P4P/1K6 b - - 0 30",
			"3r3r/1R3p1p/p4np1/1p6/kPq5/P1Q2PPB/2P4P/1K6 w - - 1 31",

			"3r3r/1R3p1p/p4Qp1/1p6/kPq5/P4PPB/2P4P/1K6 b - - 0 31",
			"3r3r/1R3p1p/p4Qp1/1p6/1Pq5/k4PPB/2P4P/1K6 w - - 0 32",

			"3r3r/1R3p1p/Q5p1/1p6/1Pq5/k4PPB/2P4P/1K6 b - - 0 32",
			"3r3r/1R3p1p/Q5p1/1p6/1kq5/5PPB/2P4P/1K6 w - - 0 33",

			"3r3r/1R3p1p/Q5p1/1p6/1kq5/2P2PPB/7P/1K6 b - - 0 33",
			"3r3r/1R3p1p/Q5p1/1p6/2q5/2k2PPB/7P/1K6 w - - 0 34",

			"3r3r/1R3p1p/6p1/1p6/2q5/2k2PPB/7P/QK6 b - - 1 34",
			"3r3r/1R3p1p/6p1/1p6/2q5/5PPB/3k3P/QK6 w - - 2 35",

			"3r3r/1R3p1p/6p1/1p6/2q5/5PPB/1Q1k3P/1K6 b - - 3 35",
			"3r3r/1R3p1p/6p1/1p6/2q5/5PPB/1Q5P/1K1k4 w - - 4 36",

			"3r3r/1R3p1p/6p1/1p6/2q5/5PP1/1Q5P/1K1k1B2 b - - 5 36",
			"7r/1R3p1p/6p1/1p6/2q5/5PP1/1Q1r3P/1K1k1B2 w - - 6 37",

			"7r/3R1p1p/6p1/1p6/2q5/5PP1/1Q1r3P/1K1k1B2 b - - 7 37",
			"7r/3r1p1p/6p1/1p6/2q5/5PP1/1Q5P/1K1k1B2 w - - 0 38",

			"7r/3r1p1p/6p1/1p6/2B5/5PP1/1Q5P/1K1k4 b - - 0 38",
			"7r/3r1p1p/6p1/8/2p5/5PP1/1Q5P/1K1k4 w - - 0 39",

			"7Q/3r1p1p/6p1/8/2p5/5PP1/7P/1K1k4 b - - 0 39",
			"7Q/5p1p/6p1/8/2p5/3r1PP1/7P/1K1k4 w - - 1 40",

			"Q7/5p1p/6p1/8/2p5/3r1PP1/7P/1K1k4 b - - 2 40",
			"Q7/5p1p/6p1/8/8/2pr1PP1/7P/1K1k4 w - - 0 41",

			"8/5p1p/6p1/8/Q7/2pr1PP1/7P/1K1k4 b - - 1 41",
			"8/5p1p/6p1/8/Q7/2pr1PP1/7P/1K2k3 w - - 2 42",

			"8/5p1p/6p1/8/Q4P2/2pr2P1/7P/1K2k3 b - - 0 42",
			"8/7p/6p1/5p2/Q4P2/2pr2P1/7P/1K2k3 w - f6 0 43",

			"8/7p/6p1/5p2/Q4P2/2pr2P1/7P/2K1k3 b - - 1 43",
			"8/7p/6p1/5p2/Q4P2/2p3P1/3r3P/2K1k3 w - - 2 44",

			"8/Q6p/6p1/5p2/5P2/2p3P1/3r3P/2K1k3 b - - 3 44"
		};

		struct EvaluationTest
		{
			inline static void deltaPhiCut()
			{
				Position pos = ChessUtility::fenToPosition(STARTPOS_FEN);
				std::vector<EVALUATION_VALUE> owl_phi_k;
				size_t sf_count = sizeof(RESULT_STOCKFISH_PHI_K) / sizeof(double);


				owl_phi_k.push_back(ChessEvaluation::evaluate(pos, PLAYER_WHITE, EVAL_FT_STANDARD, true));

				double delta_phi_k = 0;

				for (int i = 0; i < sf_count-1; i++)
				{
					auto mvs = *(MASTER_GAME_MOVES_STR.begin() + i);
					
					std::string fen_f = ChessUtility::positionToFen(pos);

					pos.applyMove(ChessUtility::stringToMove(mvs, &pos));
					
					std::string fen_t = std::string(*(MASTER_GAME_FENS.begin() + i));


					if (fen_f != fen_t) std::cout << fen_f << " != " << fen_t << std::endl;

					owl_phi_k.push_back(ChessEvaluation::evaluate(pos, PLAYER_WHITE, EVAL_FT_STANDARD, true));
				}

				double errors = 0.0;
				for (int i = 0; i < sf_count; i++)
				{					


					if (RESULT_STOCKFISH_PHI_K[i] == ERROR_VAL) 
					{
						errors++; 
						std::cout << " [Stockfish-Check-No-Eval -> Skip Move]\n";
						continue;
					}

					auto a = static_cast<double>(owl_phi_k[i].score);
					auto b = RESULT_STOCKFISH_PHI_K[i];
					
					std::cout << "[" << a << " - " << b << " = " << (a - b) << " (abs " << abs(a - b) << ")] ";
					std::string fen = std::string(*(MASTER_GAME_FENS.begin() + i));
					std::cout << " [FEN: " << fen << "]\n";

					delta_phi_k += abs(a - b);
				}
				std::cout << "delta-phi-cut = " << delta_phi_k << "/ (" << sf_count << "-" << errors << ")\n";

				delta_phi_k = delta_phi_k / (static_cast<double>(sf_count)-errors);

				std::cout << "delta-phi-cut = " << delta_phi_k << std::endl;
			}
		};
	}
}