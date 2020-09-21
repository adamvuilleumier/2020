#ifndef VAMPIRE_H
#define VAMPIRE_H

class Arena;  // This is needed to let the compiler know that Arena is a
			  // type name, since it's mentioned in the Vampire declaration.

class Vampire
{
public:
	// Constructor
	Vampire(Arena* ap, int r, int c);

	// Accessors
	int  row() const;
	int  col() const;
	bool isDead() const;

	// Mutators
	void move();

private:
	Arena* m_arena;
	int    m_row;
	int    m_col;
	int    m_health;
	int    m_idleTurnsRemaining;
};

#endif
