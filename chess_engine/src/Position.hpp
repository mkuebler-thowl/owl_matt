#pragma once

#include <array>

namespace matt
{
	/// Datenstruktur f�r eine beliebige Schachposition. 
	/// Beinhaltet ein 8x8 char-Array, um eine jeweilige Stellung zu speichern.
	class Position
	{
	public:
		explicit Position();
		virtual ~Position();

		/// <summary>
		/// Operator�berladung [index], so dass �ber Position[x][y] auf die jeweilige Zelle bzw. Feld zugegriffen werden kann.
		/// </summary>
		/// <param name="index">Zeilenindex 0-7 bzw. 8-1 im Schachfeld (mit 0=8, 1=7 usw.)</param>
		/// <returns>Gibt das jeweilige Zeilenarray[index] zur�ck.</returns>
		std::array<char, 8>& operator[](int index) const;

		/// <summary>
		/// En Passant setzen
		/// </summary>
		void setEnPassant(bool value);
		/// <summary>
		/// En Passant auslesen
		/// </summary>
		bool getEnPassant() const;
	private:
		/// <summary>
		/// 8x8 char-Array als Datenobjekt f�r eine Spielposition
		/// </summary>
		mutable std::array<std::array<char, 8>, 8> m_data;
		bool m_enPassant;
	};
}