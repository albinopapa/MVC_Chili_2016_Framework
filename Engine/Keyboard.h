/****************************************************************************************** 
 *	Chili DirectX Framework Version 14.03.22											  *	
 *	Keyboard.h																			  *
 *	Copyright 2014 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#pragma once
#include <queue>
#include <bitset>

class Keyboard
{
	friend class MainWindow;
public:
	class Event
	{
	public:
		enum Type
		{
			Press,
			Release,
			Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event( Type type,unsigned char code )
			:
			type( type ),
			code( code )
		{}
		bool IsPress() const
		{
			return type == Press;
		}
		bool IsRelease() const
		{
			return type == Release;
		}
		bool IsValid() const
		{
			return type != Invalid;
		}
		unsigned char GetCode() const
		{
			return code;
		}
	};
public:
	bool KeyIsPressed( unsigned char keycode ) const;
	Event ReadKey();
	Event PeekKey() const;
	bool KeyEmpty() const;
	unsigned char ReadChar();
	unsigned char PeekChar() const;
	bool CharEmpty() const;
	void FlushKeyBuffer();
	void FlushCharBuffer();
	void FlushBuffers();
	void EnableAutorepeat();
	void DisableAutorepeat();
	bool IsAutorepeatEnabled() const;
private:
	void OnKeyPressed( unsigned char keycode );
	void OnKeyReleased( unsigned char keycode );
	void OnChar( unsigned char character );
	template<typename T>
	void TrimBuffer( std::queue<T>& buffer );
private:
	static constexpr unsigned int nKeys = 256;
	static constexpr unsigned int bufferSize = 4;
	bool autorepeatEnabled = false;
	std::bitset<nKeys> keystates;
	std::queue<Event> keybuffer;
	std::queue<unsigned char> charbuffer;
};