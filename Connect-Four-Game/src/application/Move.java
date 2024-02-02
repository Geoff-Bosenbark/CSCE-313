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
