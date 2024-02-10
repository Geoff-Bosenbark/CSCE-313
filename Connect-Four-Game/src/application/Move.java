/**
 * Represents a move in the game by encapsulating the row and column indices into a single object.
 * This class is utilized primarily within the SmartAI class to facilitate more strategic AI decisions by
 * allowing the AI to consider and evaluate potential moves based on their specific locations on the game board.
 * It simplifies the process of passing around move coordinates within the AI's logic.
 */

package application;

/**
 * store row and column indices in a single object
 * @impltNote Helps SmartAI make better placement decisions
 */
public class Move
{
	int column;
	int row;

	Move(int column, int row)
	{
		this.column = column;
		this.row = row;
	}
}
