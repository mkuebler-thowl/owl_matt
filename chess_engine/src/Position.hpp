#pragma once

#include <array>

namespace matt
{
	/// Datenstruktur für eine beliebige Schachposition. 
	/// Beinhaltet ein 8x8 char-Array, um eine jeweilige Stellung zu speichern.
	class Position
	{
	public:
		explicit Position();
		virtual ~Position();

		/// <summary>
		/// Operatorüberladung [index], so dass über Position[x][y] auf die jeweilige Zelle bzw. Feld zugegriffen werden kann.
		/// </summary>
		/// <param name="index">Zeilenindex 0-7 bzw. 8-1 im Schachfeld (mit 0=8, 1=7 usw.)</param>
		/// <returns>Gibt das jeweilige Zeilenarray[index] zurück.</returns>
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
		/// 8x8 char-Array als Datenobjekt für eine Spielposition
		/// </summary>
		mutable std::array<std::array<char, 8>, 8> m_data;
		bool m_enPassant;
	};
}