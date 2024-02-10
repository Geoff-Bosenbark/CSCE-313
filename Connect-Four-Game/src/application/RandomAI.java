/**
 * Implements a simple AI opponent that selects moves randomly. This represent's the game's 'Easy' mode.
 * This class extends the abstract AI class, providing a concrete implementation for the decideMove method,
 * which picks a column at random for chip placement. This AI does not attempt to block the player or strategically
 * win the game but ensures the selected column is not already full. 
 */

package application;

import java.util.HashMap;
import java.util.Random;

/**
 * @param boardState = The current state of the board represented as a HashMap where the key is a String 
 *        formatted as "column,row" and the value is the Chip at that position.
 * @param playerTurn = A boolean indicating whether it is the player's turn. 
 * @implNote playerTurn is not needed for random move logic
 * 
 * @return An integer representing the column index where the AI will place its next chip.
 */
public class RandomAI extends AI
{
	/**
	 * Constructor for the RandomAI class.
	 * 
	 * @param columns = Number of columns on the board.
	 * @param rows    = Number of rows on the board.
	 */
	public RandomAI(int columns, int rows)
	{
		super(columns, rows);
	}

	@Override
	public int decideMove(HashMap<String, Chip> boardState, boolean playerTurn)
	{
		Random randomizer = new Random();
		int column;

		do
		{
			column = randomizer.nextInt(columns);					// Random column index
		} while (GameController.isColumnFull(boardState, column));	// Continue if column is full

		return column; // Return the found column that is not full
	}
}
