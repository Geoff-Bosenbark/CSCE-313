package application;

import java.util.HashMap;

public abstract class AI
{
	protected int columns;
	protected int rows;

	/**
	 * initializes AI with dimensions of game board
	 * 
	 * @param columns = number of columns in board
	 * @param rows 	  = number of rows in board
	 */
	public AI(int columns, int rows)
	{
		this.columns = columns;
		this.rows = rows;
	}

	/**
	 * Abstract method for subclasses 'RandomAI' and 'SmartAI' to implement
	 * 
	 * @param boardState = current state of board
	 * @param playerTurn = true if player's turn, false if AI's turn
	 * @return column index where AI will place chip
	 */
	public abstract int decideMove(HashMap<String, Chip> boardState, boolean playerTurn);
}
