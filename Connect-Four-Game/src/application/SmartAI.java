package application;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Random;

/**
 * @implNote Grading requirement (B+)
 * 
 * Class for the 'thoughtful' AI, extending generic AI class 
 * AI initialized in 'Hard' game difficulty
 *           
 * @implNote Smart AI has the capability to recognize and prioritize making a winning move
 * @implNote Smart AI has the capability to recognize and block a player's winning move
 */
public class SmartAI extends AI
{
	// constants for board dimensions
	public static final int COLUMNS = GameController.COLUMNS;
	public static final int ROWS = GameController.ROWS;

	// Shadow data structure to keep track of which columns are already full
	private HashSet<Integer> fullColumns;

	/*
	 * Possible directions to check for a Connect Four, represented as coordinate changes. Used for searching HashMap
	 */
	private static final int[][] DIRECTIONS = { 
			{ 1, 0 }, // Horizontal right
			{ 0, 1 }, // Vertical down
			{ 1, 1 }, // Diagonal down-right
			{ -1, 1 } // Diagonal down-left
	};

	/**
	 * Constructor for the SmartAI class.
	 * 
	 * @param columns = Number of columns on the board.
	 * @param rows    = Number of rows on the board.
	 */
	public SmartAI(int columns, int rows)
	{
		super(columns, rows);
		fullColumns = new HashSet<>();
	}

	/**
	 * Decides the next move for the AI. Prints the type of move the AI has decided to make to the Console
	 * 
	 * @param boardState = Current state of the board.
	 * @param playerTurn Indicates whether it's the player's turn.
	 * @return The column in which to drop the next chip.
	 */
	@Override
	public int decideMove(HashMap<String, Chip> boardState, boolean playerTurn)
	{
		// Update the full columns before deciding the move.
		updateFullColumns(boardState);

		// First, check if the AI can win in the next move.
		Move winMove = findWinningMove(boardState, false); // false = AI's chips
		if (winMove != null)
		{
			System.out.println("AI move type: attempting to win");
			return winMove.column;
		}

		// If the AI cannot win, check if it needs to block the player's winning move.
		Move blockMove = findWinningMove(boardState, true); // true = player's chips
		if (blockMove != null)
		{
			System.out.println("AI move type: attempting to block player win");
			return blockMove.column;
		}

		// If there are no immediate win or block moves, extend an existing line of chips.
		Move extendMove = findBestExtendMove(boardState);
		if (extendMove != null)
		{
			System.out.println("AI move type: extending line");
			return extendMove.column;
		}

		// Make a random move as a last resort.
		int randomMove = getRandomMove(boardState);
		System.out.println("AI move type: random");
		return randomMove;
	}

	/**
	 * Finds a winning or blocking move for the given player.
	 * 
	 * @param boardState = Current state of the board.
	 * @param forPlayer  = True if looking for a player move, false for AI.
	 * @return The column that would result in a win/block, or -1 if not found.
	 */
	private Move findWinningMove(HashMap<String, Chip> boardState, boolean forPlayer)
	{
		for (int x = 0; x < COLUMNS; x++)
		{
			for (int y = 0; y < ROWS; y++)
			{
				for (int[] dir : DIRECTIONS)
				{
					Move potentialMove = getLineInfo(boardState, x, y, dir[0], dir[1], forPlayer);
					if (potentialMove != null)
					{
						return potentialMove;
					}
				}
			}
		}
		return null; // No winning or blocking move found
	}

	/**
	 * Gets information about a line of chips in a specific direction.
	 * 
	 * @param boardState = Current state of the board.
	 * @param x          = X coordinate of the starting chip.
	 * @param y          = Y coordinate of the starting chip.
	 * @param dx         = X direction to check.
	 * @param dy         = Y direction to check.
	 * @param forPlayer  = True if checking for player's chips, false for AI's.
	 * @return The column where a winning/blocking move can be made, or -1 if not applicable.
	 */
	private Move getLineInfo(HashMap<String, Chip> boardState, int x, int y, int dx, int dy, boolean forPlayer)
	{
		int countChips = 0;
		Move potentialMove = null;

		for (int i = 0; i < 4; i++)
		{
			int newX = x + i * dx;
			int newY = y + i * dy;

			// Check bounds
			if (newX < 0 || newX >= COLUMNS || newY < 0 || newY >= ROWS)
			{
				return null; // Invalid line
			}

			Chip chip = boardState.get(newX + "," + newY);
			if (chip != null && chip.chipColor == forPlayer)
			{
				countChips++;
			} else if (chip == null && canPlayHere(boardState, newX, newY))
			{
				if (potentialMove == null)
				{
					int lowestEmptyRow = findLowestEmptyRow(boardState, newX);
					if (lowestEmptyRow != -1)
					{
						potentialMove = new Move(newX, lowestEmptyRow);
					}
				} else
				{
					return null; // More than one potential move, not a valid line
				}
			}
		}

		if (countChips == 3 && potentialMove != null)
		{
			// Found a line of three with a playable fourth position
			return potentialMove;
		}
		return null; // No line of three found
	}

	// Helper method to find the lowest empty row in a given column
	private int findLowestEmptyRow(HashMap<String, Chip> boardState, int column)
	{
		for (int row = ROWS - 1; row >= 0; row--)
		{
			if (boardState.get(column + "," + row) == null)
			{
				return row;
			}
		}
		return -1; // No empty row found
	}

	/**
	 * Checks if a chip can be played at the given coordinates.
	 * 
	 * @param boardState = Current state of the board.
	 * @param x          = X coordinate to check.
	 * @param y          = Y coordinate to check.
	 * @return True if the move can be played, false otherwise.
	 */
	private boolean canPlayHere(HashMap<String, Chip> boardState, int x, int y)
	{
		// The move is playable if it's in the bottom row or there's a chip beneath it
		return y == 0 || boardState.containsKey(x + "," + (y - 1));
	}

	/**
	 * Finds the best move to extend the AI's line of chips.
	 * 
	 * @param boardState = Current state of the board.
	 * @return The best column to extend the line, or -1 if no such column found.
	 */
	// Helper method to find the best move to extend AI's line of chips
	private Move findBestExtendMove(HashMap<String, Chip> boardState)
	{
		Move bestMove = null;
		int longestLine = 0;

		for (int x = 0; x < COLUMNS; x++)
		{
			// Find the lowest empty row in the current column
			int lowestEmptyRow = findLowestEmptyRow(boardState, x);
			if (lowestEmptyRow != -1)
			{
				// Check if placing a chip here would extend a line of AI chips
				int lineLength = getLineLength(boardState, x, lowestEmptyRow, false); // false for AI's chips
				if (lineLength > longestLine)
				{
					longestLine = lineLength;
					bestMove = new Move(x, lowestEmptyRow);
				}
			}
		}
		return bestMove; // Return the best move found or null if no extension is possible.
	}

	/**
	 * Makes a random move by selecting a non-full column randomly.
	 * 
	 * @param boardState = Current state of the board.
	 * @return A randomly chosen column that is not full.
	 */
	private int getRandomMove(HashMap<String, Chip> boardState)
	{
		Random random = new Random();
		int column;
		do
		{
			column = random.nextInt(COLUMNS);
		} while (fullColumns.contains(column)); // Check against the set of full columns
		return column;
	}

	/**
	 * Calculates the length of the line of matching chips from a starting position. Helper function for AI's 'Extend' move
	 * type
	 * 
	 * @param boardState = Current state of the board.
	 * @param x          = X coordinate of the starting chip.
	 * @param y          = Y coordinate of the starting chip.
	 * @param checkForAI = True if checking AI's chips, false for player's.
	 * @return The length of the longest line of chips found.
	 */
	private int getLineLength(HashMap<String, Chip> boardState, int x, int y, boolean checkForAI)
	{
		// initialize return variable
		int lineLength = 0;

		// Check in each direction for consecutive chips.
		for (int[] dir : DIRECTIONS)
		{
			int dx = dir[0];
			int dy = dir[1];
			int count = 1; // Start with 1 to include the initial chip in the line

			/**
			 * Check in the positive direction Increment 'i' to move one step at a time in the positive direction If next position
			 * is valid and chip matches, count is incremented
			 */
			int i = 1;
			while (isValidPosition(x + i * dx, y + i * dy) && isChipMatch(boardState, x + i * dx, y + i * dy, checkForAI))
			{
				count++;
				i++;
			}

			/**
			 * Check in the negative direction (only for horizontal and diagonal, vertical is not necessary) Increment 'i' to move
			 * one step at a time in the negative direction If next position is valid and chip matches, count is incremented
			 */
			if (dx != 0) //
			{
				i = 1;
				while (isValidPosition(x - i * dx, y - i * dy) && isChipMatch(boardState, x - i * dx, y - i * dy, checkForAI))
				{
					count++;
					i++;
				}
			}
			lineLength = Math.max(lineLength, count); // Update the line's length
		}
		// return longest line found
		return lineLength;
	}

	/**
	 * Checks if the given coordinates are within the bounds of the board. Helper function for getLineLength
	 * 
	 * @param x = X coordinate to check.
	 * @param y = Y coordinate to check.
	 * @return True if the position is valid, false otherwise.
	 */
	private boolean isValidPosition(int x, int y)
	{
		return x >= 0 && x < COLUMNS && y >= 0 && y < ROWS;
	}

	/**
	 * Checks if the chip at the given coordinates matches belongs to the AI or player, depending on checkForAI value Helper
	 * function for getLineLength
	 * 
	 * @param boardState = Current state of the board.
	 * @param x          = X coordinate of the chip.
	 * @param y          = Y coordinate of the chip.
	 * @param checkForAI = False (because chipColor = False if AI)
	 * @return True if the chip matches the specified player/AI, false otherwise.
	 */
	private boolean isChipMatch(HashMap<String, Chip> boardState, int x, int y, boolean checkForAI)
	{
		Chip chip = boardState.get(x + "," + y);
		return chip != null && chip.chipColor == checkForAI;
	}

	/**
	 * Updates the set of full columns based on the current board state. Important piece of overall SmartAI decision-making
	 * 
	 * @param boardState = Current state of the board.
	 */
	private void updateFullColumns(HashMap<String, Chip> boardState)
	{
		for (int x = 0; x < COLUMNS; x++)
		{
			if (GameController.isColumnFull(boardState, x))
			{
				fullColumns.add(x); // Add full column to the set.
			} else
			{
				fullColumns.remove(x); // Remove non-full column from the set.
			}
		}
	}
}