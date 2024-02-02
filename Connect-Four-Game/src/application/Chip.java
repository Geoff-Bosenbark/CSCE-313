package application;

import javafx.scene.effect.Light;
import javafx.scene.effect.Lighting;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;

/**
 * Class for chip creation
 * @implNote RED = player chip, BLUE = AI chip
 */
public class Chip extends Circle
{
	public static final int TILE_SIZE = GameController.TILE_SIZE	;
	public static final int COLUMNS = GameController.COLUMNS;
	public static final int ROWS = GameController.ROWS;
	public final boolean chipColor;
	
	/**
	 * @implNote Similar to use of Boolean PlayerTurn, if chipColor = true, then its the player's chip; false = AI chip
	 */
	public Chip(boolean chipColor)
	{
		/**
		 * @implNote contributes to 'attractive'; Grading requirement (C+)
		 * 
		 * Consulted Human-Computer Interaction student about chip colors
		 * Red and Blue colors were recommended for chips
		 * Played around with the different types or red/blue in Color to find best fit with maroon grid
		 */
		super(TILE_SIZE / 2, chipColor ? Color.DARKRED : Color.DARKBLUE); // true|RED|Player, false|BLUE|AI
		this.chipColor = chipColor;
		
		/**
		 * @implNote contributes to 'attractive'; Grading requirement (C+)
		 * 
		 * Apply a lighting effect to the chip to add dimension
		 */
		Light.Distant light = new Light.Distant();
		light.setColor(Color.GHOSTWHITE);
		Lighting lighting = new Lighting();
		lighting.setLight(light);
		lighting.setSurfaceScale(5.0);
		this.setEffect(lighting);

		setCenterX(TILE_SIZE / 2);
		setCenterY(TILE_SIZE / 2);
	}
}