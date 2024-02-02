package application;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Objects;
import java.util.Optional;

import javafx.animation.TranslateTransition;
import javafx.application.Platform;
import javafx.util.Duration;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.effect.Light;
import javafx.scene.effect.Lighting;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Rectangle;
import javafx.scene.shape.Shape;
import javafx.scene.text.Text;
import javafx.scene.text.TextFlow;
import javafx.stage.Stage;
import javafx.fxml.FXML;
import javafx.scene.control.*;

/**
 * Handles all user interactions and manages the game state
 * includes the graphical representation of the game board, user actions, AI moves, and game logic.
 */

public class GameController
{
	// declare panes from GameBoard.fmxl
	@FXML private BorderPane boardPane;
	@FXML private Pane gameBoard;

	// declare menu items from GameBoard.fmxl
	@FXML MenuBar menuBar;
	@FXML Menu fileMenu;
	@FXML Menu Help;
	
	@FXML private MenuItem newGame;
	@FXML private MenuItem changeDifficulty;
	@FXML private MenuItem saveGame;
	@FXML private MenuItem loadGame;
	@FXML private MenuItem Exit;
	@FXML private MenuItem gameManual;

	// AI class; either Random or Smart
	public static AI gameAI;
	
	// Game board constants for dimensions
	public static final int TILE_SIZE = 90; // original: 80
	public static final int COLUMNS = 7;
	public static final int ROWS = 6;
	
	// Boolean that tracks player's turn
	public boolean playerTurn = true; // player is RED chip
	
	/**
	 * @implNote Using HashMap instead of 2D array; Grading requirement (B)
	 * 
	 * Data structure to store the current state of the board
	 */
	private HashMap<String, Chip> boardState = new HashMap<>();
	
	// Pane to hold chips
	private Pane chipPane = new Pane();

    /**
     * Initialize the controller 
     * Set up the game board 
     * Helper methods create the game grid and columns
     */
	@FXML
	private void initialize()
	{
        // Initialize the game board with null values representing empty spaces
		for (int x = 0; x < COLUMNS; x++)
		{
			for (int y = 0; y < ROWS; y++)
			{
				boardState.put(x + "," + y, null); // Initialize all slots as empty
			}
		}
		
		// Add panes and shapes to the game board
		gameBoard.getChildren().add(chipPane);
		Shape grid = makeGrid(); // Create the custom grid
		gameBoard.getChildren().add(grid); // Add the grid to the game board
		gameBoard.getChildren().addAll(makeColumns()); // Add columns to the game board
		boardPane.setCenter(gameBoard); // Set the game board as the center of the BorderPane

		// Show start up dialog and select game difficulty
		changeDifficulty();
	}

	/**
	 * Creates the visual representation of the game grid.
	 * @return Shape that represents the game grid with holes for chips.
	 */
	private Shape makeGrid()
	{
		// Create a rectangle to represent the base of the grid
		Shape shape = new Rectangle((COLUMNS + 1) * TILE_SIZE, (ROWS + 1) * TILE_SIZE);

		// Iterate over the grid positions to create circular holes for the chips
		for (int y = 0; y < ROWS; y++)
		{
			for (int x = 0; x < COLUMNS; x++)
			{
				Circle circle = new Circle(TILE_SIZE / 2);
				circle.setCenterX(TILE_SIZE / 2);
				circle.setCenterY(TILE_SIZE / 2);
				circle.setTranslateX(x * (TILE_SIZE + 5) + TILE_SIZE / 4);
				circle.setTranslateY(y * (TILE_SIZE + 5) + TILE_SIZE / 4);

				// Subtract the circle from the base shape to create holes
				shape = Shape.subtract(shape, circle);
			}
		}
		
		/**
		 * @implNote contributes to 'attractive'; Grading requirement (C+)
		 * 
		 * Apply a lighting effect to the grid to add dimension
		 */
		Light.Distant light = new Light.Distant();
		light.setColor(Color.GHOSTWHITE);
		Lighting lighting = new Lighting();
		lighting.setLight(light);
		lighting.setSurfaceScale(5.0);

		// Set the fill color and effect of the grid shape
		shape.setFill(Color.web("#500000"));	// maroon, Whoop!
		shape.setEffect(lighting);

		return shape;
	}

	/**
	 * Creates interactive columns that respond to mouse events for chip placement.
	 * @return List of Rectangle objects that represent the interactive columns.
	 */
	private List<Rectangle> makeColumns()
	{
		List<Rectangle> list = new ArrayList<>();

		// Create interactive rectangles for each column
		for (int x = 0; x < COLUMNS; x++)
		{
			Rectangle rectangle = new Rectangle(TILE_SIZE, (ROWS + 1) * TILE_SIZE);
			rectangle.setTranslateX(x * (TILE_SIZE + 5) + TILE_SIZE / 4);
			rectangle.setFill(Color.TRANSPARENT);
			
			// Add mouse hover effects so that player nows what column they are selecting
			rectangle.setOnMouseEntered(e -> rectangle.setFill(Color.rgb(80, 0, 0, 0.1)));
			rectangle.setOnMouseExited(e -> rectangle.setFill(Color.TRANSPARENT));

			/**
			 * @implNote Click on any spot within any column to populate a Chip in that column; Grading requirement (A-) 
			 * Add a mouse click event to drop a chip in the selected column
			 */
			final int columnSelected = x;
			rectangle.setOnMouseClicked(e -> dropChip(new Chip(playerTurn), columnSelected));

			list.add(rectangle);
		}
		return list;
	}

	/**
	 * Retrieves the chip at the specified column and row, if any.
	 * 
	 * @param column = the column index
	 * @param row = the row index
	 * @return an Optional containing the Chip object if present, otherwise empty.
	 */
	private Optional<Chip> getChip(int column, int row)
	{
		String key = column + "," + row;
		return Optional.ofNullable(boardState.get(key));
	}

	/**
	 * Enables or disables mouse click events on all column rectangles. 
	 * @param status = true, disable click events; if false, enable click events.
	 */
	private void setColumnsMouseTransparent(boolean status)
	{
		gameBoard.getChildren().filtered(node -> node instanceof Rectangle).forEach(rectangle -> rectangle.setMouseTransparent(status));
	}

	/**
	 * Attempts to drop a chip into the specified column, handling chip placement and animations.
	 * If the column is full, it will display an alert and not proceed with the move.
	 * 
	 * @param chip = the Chip object to drop
	 * @param column = the column index to drop the chip into
	 */
	private void dropChip(Chip chip, int column)
	{
		/**
		 * @implNote Ease of use (forgiving); Grading requirement (B)
		 * 
		 * Check if the column is full before proceeding
		 * If column is full, show an alert and do not proceed
	     */
	    if (isColumnFull(boardState, column)) 
	    {
	        showAlert("Column Full", "You cannot place a chip here.", "Please choose another column.");
	        return; // Doesn't progress turn
	    }
	    
	    // find lowest empty row in the selected column
		int row = ROWS - 1;
		while (row >= 0 && getChip(column, row).isPresent())
		{
			row--;
		}

		if (row < 0)
		{
			return;	// if column is full, exit method; primarily for AI
		}
		
		// place the chip in the board state and visual pane
		String key = column + "," + row;
		boardState.put(key, chip);
		chipPane.getChildren().add(chip);
		chip.setTranslateX(column * (TILE_SIZE + 5) + TILE_SIZE / 4);

		// Disable clicks during chip drop animation to prevent erroneous clicking / chip creation
		setColumnsMouseTransparent(true);

		// create and play chip drop animation to simulate realistic chip placement
		TranslateTransition animation = new TranslateTransition(Duration.seconds(0.5), chip);
		animation.setToY(row * (TILE_SIZE + 5) + TILE_SIZE / 4);

		final int currentRow = row;
		animation.setOnFinished(e ->
		{
			// Re-enable clicks after animation and check for game-ending conditions
			setColumnsMouseTransparent(false);

			if (checkFourInARow(column, currentRow))
			{
				Platform.runLater(() ->
				{
					gameOver(chip.chipColor);
				});
			} else if (isBoardFull())
			{
				Platform.runLater(() ->
				{
					gameTie();
				});
			} else
			{
				// Toggle turn and call AI move
				playerTurn = !playerTurn;
				if (!playerTurn)
				{
					makeAIMove();
				}
			}
		});
		animation.play();
	}

	/**
	 * Initiates the AI's move. 
	 * Runs in a separate thread to avoid freezing the UI.
	 */
	private void makeAIMove()
	{
		// start new thread for AI move
		new Thread(() ->
		{
			int aiColumn = gameAI.decideMove(boardState, playerTurn);
			Platform.runLater(() -> dropChip(new Chip(false), aiColumn)); // in Chip class, AI's chip color = false; players = true
		}).start();
	}

	/**
	 * Checks if there is a sequence of four matching chips starting from the given column and row.
	 * 
	 * @param column = the starting column index
	 * @param row = the starting row index
	 * @return true if there is a sequence of four matching chips, false otherwise.
	 */
	private boolean checkFourInARow(int column, int row)
	{
		// Retrieve the chip at the starting position
		Chip chip = getChip(column, row).orElse(null);
		if (chip == null)
			return false;	// No chip at the starting position, so no sequence
		boolean color = chip.chipColor;

		// Check for a sequence of four matching chips in all directions
		return checkDirection(column, row, 1, 0, color) || // Horizontal
				checkDirection(column, row, 0, 1, color) || // Vertical
				checkDirection(column, row, 1, 1, color) || // Diagonal (bottom-left to top-right)
				checkDirection(column, row, 1, -1, color); // Diagonal (top-left to bottom-right)
	}

	/**
	 * Helper method to check for a sequence of four matching chips in a single direction.
	 * 
	 * @param column = the starting column index
	 * @param row = the starting row index
	 * @param dx = the horizontal direction increment (1, 0, or -1)
	 * @param dy = the vertical direction increment (1, 0, or -1)
	 * @param color = the color of the chip to check for a match
	 * @return true if a sequence of four matching chips is found, false otherwise.
	 */
	private boolean checkDirection(int column, int row, int dx, int dy, boolean color)
	{
		int count = 0;
		
		// Check for a sequence of four matching chips in the given direction
		for (int i = -3; i <= 3; i++)
		{
			int currentColumn = column + i * dx;
			int currentRow = row + i * dy;

			// Skip positions that are outside bounds of game board
			if (currentColumn < 0 || currentColumn >= COLUMNS || currentRow < 0 || currentRow >= ROWS)
			{
				continue;
			}

			// Check for a chip match at the current position
			Chip chip = getChip(currentColumn, currentRow).orElse(null);
			if (chip != null && chip.chipColor == color)
			{
				count++;
				if (count == 4)
				{
					return true; // Found sequence of four matching chips
				}
			} else
			{
				count = 0; // Reset the count if there is a gap or mismatch
			}
		}
		return false; // No sequence of four matching chips
	}
	
	/**
	 * Retrieves the current player's name based on whose turn it is.
	 * @return String representing the current player ("Player" or "AI").
	 */
	private String getCurrentPlayerName()
	{
		return playerTurn ? "Player" : "AI";
	}

	/**
	 * Displays an information alert dialog to the user with a custom icon.
	 * 
	 * @param title = The title of the alert dialog.
	 * @param header = The header text of the alert.
	 * @param content = The main content message of the alert.
	 */
	private void showAlert(String title, String header, String content)
	{
		// Create an alert with an OK button
		Alert alert = new Alert(AlertType.INFORMATION);
		alert.setTitle(title);
		alert.setHeaderText(header);
		alert.setContentText(content);
		
		// Remove the default graphic and set the window icon
		alert.setGraphic(null);
	    Stage stage = (Stage) alert.getDialogPane().getScene().getWindow();
		stage.getIcons().add(new Image("file:src/application/icon.png"));

		// Show the alert and wait for the user to close it
		alert.showAndWait();
	}

	/**
	 * Initializes the AI based on the chosen difficulty.
	 * @param choice = The selected difficulty level ("Easy" or "Hard").
	 */
	private void initializeAI(String choice)
	{
		switch (choice)
		{
		case "Easy":
			gameAI = new RandomAI(COLUMNS, ROWS);
			break;
		case "Hard":
			gameAI = new SmartAI(COLUMNS, ROWS);
			break;
		default:
			gameAI = new RandomAI(COLUMNS, ROWS);
			break;
		}

		// proceed with new game
		// must have when changing difficulty mid-game
		startNewGame();
	}
	
	/**
	 * Initializes the AI when a game is loaded, without starting a new game.
	 * Same as initializeAI, without startNewGame() call
	 * @param choice = The AI difficulty type ("Easy" or "Hard") loaded from the save file.
	 */
	private void loadAI(String choice) 
	{
	    switch (choice) 
	    {
	        case "Easy":
	            gameAI = new RandomAI(COLUMNS, ROWS);
	            break;
	        case "Hard":
	            gameAI = new SmartAI(COLUMNS, ROWS);
	            break;
	        default:
	            gameAI = new RandomAI(COLUMNS, ROWS);
	            break;
	    }
	}

	/**
	 * Initial message when new game is started
	 * Presents a choice dialog to change the AI difficulty and handles the user's selection.
	 */
	@FXML
	private void changeDifficulty()
	{
		// List of choices
		List<String> choices = new ArrayList<>();
		choices.add("Easy");
		choices.add("Hard");

		// instructions for game and difficulty selection
		ChoiceDialog<String> dialog = new ChoiceDialog<>("Easy", choices);
		dialog.setTitle("Instructions and Difficulty Selection");
		dialog.setHeaderText("Get Four Chips in a line vertically, horizontally, or diagonally to win!");
		dialog.setContentText(
				"Choose your difficulty.\n" + 
				"If you have questions, refer to the 'Help' tab at the top of the game window.");
		
		// replace default graphin within dialog window with custom graphic
		ImageView customGraphic = new ImageView(new Image("file:src/application/icon.png"));
		customGraphic.setFitWidth(64);
		customGraphic.setFitHeight(64);
		dialog.setGraphic(customGraphic);
		
		// set window icon
	    Stage stage = (Stage) dialog.getDialogPane().getScene().getWindow();
		stage.getIcons().add(new Image("file:src/application/icon.png"));


		// Event handler for when the dialog is closed
	    Optional<String> result = dialog.showAndWait();
	    if (result.isPresent()) 
	    {
	        // User made a selection
	        initializeAI(result.get()); // Initialize the AI based on the selected difficulty
	    } else 
	    {
	        // User canceled the dialog.
	        showAlert("AI Selection", "No AI difficulty selected.", "Defaulting to Easy difficulty.");
	        initializeAI("Easy"); // Default to Easy difficulty
	    }
	}

	/**
	 * @implNote Reset to next game at any time; Grading requirement (C+)
	 * 
	 * Clears the board and starts a new game.
	 */
	@FXML
	private void startNewGame()
	{
		boardState.clear();
		chipPane.getChildren().clear(); // Clear visual chips

		// Reset board state for all positions to null (empty)
		for (int x = 0; x < COLUMNS; x++)
		{
			for (int y = 0; y < ROWS; y++)
			{
				boardState.put(x + "," + y, null); // Initialize all slots as empty
			}
		}

		playerTurn = true;	// set turn to player
	    showAlert("Match Commence", getCurrentPlayerName() + "'s turn!", "Have fun!");
	}

	/**
	 * @implNote Grading requirement (A)
	 * 
	 * Loads the game state from a save file.
	 */
	@FXML
	private void loadGame()
	{
		boardState.clear();
		chipPane.getChildren().clear();
		
		/**
		 * @implNote log of moves; Grading requirement (A-)
		 * Reads savegame.txt and sets game state, player turn, and difficulty
		 */
		try (BufferedReader reader = new BufferedReader(new FileReader("savegame.txt")))
		{
			for (int y = 0; y < ROWS; y++)
			{
				String[] line = reader.readLine().trim().split(" ");
				for (int x = 0; x < COLUMNS; x++)
				{
					String chipData = line[x];
					Chip chip = null;
					if (!"N".equals(chipData))
					{
						chip = new Chip("R".equals(chipData));
						chip.setTranslateX(x * (TILE_SIZE + 5) + TILE_SIZE / 4);
						chip.setTranslateY(y * (TILE_SIZE + 5) + TILE_SIZE / 4);
						chipPane.getChildren().add(chip);
					}
					boardState.put(x + "," + y, chip);
				}
			}
			playerTurn = "R".equals(reader.readLine());
			
	        // Read and initialize AI type from the file
	        String aiType = reader.readLine();
	        loadAI(aiType);

			showAlert("Game Loaded", getCurrentPlayerName() + "'s turn!", "");
		} catch (IOException e)
		{
			e.printStackTrace();
			showAlert("Error", "Load Error", "Failed to load the game.");
		}
	}

	/**
	 * @implNote Grading requirement (A)
	 * 
	 * Saves the current game state to a file.
	 */
	@FXML
	private void saveGame()
	{
		/**
		 * @implNote log of moves; Grading requirement (A-)
		 * Creates savegame.txt and writes game state, player turn, and difficulty to the log
		 */
		try (PrintWriter out = new PrintWriter("savegame.txt"))
		{
	        // Save the current board state
			for (int y = 0; y < ROWS; y++)
			{
				for (int x = 0; x < COLUMNS; x++)
				{
					Chip chip = getChip(x, y).orElse(null);
					out.print(chip == null ? "N " : (chip.chipColor ? "R " : "Y "));
				}
				out.println();
			}
			
			// Save the current player's turn
			out.println(playerTurn ? "R" : "Y");
			
	        // Write the AI type to the file
	        String aiType = (gameAI instanceof SmartAI) ? "Hard" : "Easy";
	        out.println(aiType);

			showAlert("Saved Game", "Game successfully saved!", "");
		} catch (FileNotFoundException e)
		{
			e.printStackTrace();
			showAlert("Error", "Save Error", "Failed to save the game.");
		}
	}

	/**
	 * Closes the application.
	 */
	@FXML
	private void exitGame()
	{
		Platform.exit();
	}

	/**
	 * Checks if all columns in the board are full.
	 * 
	 * @param boardState = The current state of the board.
	 * @param column = The column to check.
	 * @return true if the specified column is full, false otherwise.
	 */
	public static boolean isColumnFull(HashMap<String, Chip> boardState, int column)
	{
		return boardState.get(column + ",0") != null;
	}
	
	/**
	 * Checks if the entire board is full, used for indicating a tie game.
	 * @return true if the board is full, false otherwise.
	 */
	private boolean isBoardFull()
	{
		return boardState.values().stream().noneMatch(Objects::isNull);
	}

	/**
	 * Handles the end of the game when there is a tie.
	 */
	private void gameTie()
	{
		showAlert("Game Over", "It's a tie!", "No more moves left.");
		showNewGameConfirmation();
	}

	/**
	 * Announces the game over condition and which player has won. 
	 * @param chipColor = The color of the winning chip; true = player (RED), false = AI (BLUE)
	 */
	private void gameOver(boolean chipColor)
	{
		// Announce winner
		if (chipColor)
			showAlert("Winner Announcement", "The Player won!", "");	// true
		else
			showAlert("Winner Announcement", "The AI won!", "");		// false

		// Reset game state or exit depending on user choice
		showNewGameConfirmation();
	}

	/**
	 * Displays the game manual in a dialog.
	 */
	@FXML
	private void showGameManual() 
	{
	    Dialog<String> dialog = new Dialog<>();
	    dialog.setTitle("Game Manual");
	    dialog.setHeaderText(
	            "Game Objective: Line up four chips in any direction to win\n"
	            + "Player Chip = RED | AI Chip = BLUE");
	    
	    Stage stage = (Stage) dialog.getDialogPane().getScene().getWindow();
		stage.getIcons().add(new Image("file:src/application/icon.png"));

	    // Create content for the manual with formatted text
	    TextFlow gameManualContent = new TextFlow();
	    gameManualContent.getChildren().addAll(
	        new Text("File Menu Information\n- "),
	        createBoldText("Start New Game"),
	        new Text(": Select this option to start over\n- "),
	        createBoldText("Change Difficulty"),
	        new Text(": Select this to switch between Easy and Hard modes"),
	        createWarningText(" Warning! "),
	        new Text("This will restart the match\n- "),
	        createBoldText("Save Game"),
	        new Text(": Use this to save the board to resume later\n- "),
	        createBoldText("Load Game"),
	        new Text(": Use this to load a previously saved game"),
	        createWarningText(" Warning! "),
	        new Text("This will overwrite any match currently in progress\n- "),
	        createBoldText("Exit Game"),
	        new Text(": Use this to close the game\n\nGame Information:\n- Place your mouse over the desired column and click to place your chip.")
	    );

	    // Wrap the content in a scroll pane
	    ScrollPane scrollPane = new ScrollPane(gameManualContent);
	    scrollPane.setPrefSize(650, 250); // window size
	    scrollPane.setFitToWidth(true);
	    dialog.getDialogPane().setContent(scrollPane);

	    // button to close the dialog.
	    ButtonType closeButton = new ButtonType("Close", ButtonBar.ButtonData.OK_DONE);
	    dialog.getDialogPane().getButtonTypes().add(closeButton);

	    // Show the dialog and wait for it to be closed.
	    dialog.showAndWait();
	}

	/**
	 * Creates bold-styled text on specified String
	 * 
	 * @param text = The string to be bolded.
	 * @return Text with bold styling.
	 */
	private Text createBoldText(String text) 
	{
	    Text t = new Text(text);
	    t.setStyle("-fx-font-weight: bold;");
	    return t;
	}

	/**
	 * Creates a red/italicized text node.
	 * 
	 * @param text = The string to be formatted as a 'warning'.
	 * @return A Text node with red italic styling.
	 */
	private Text createWarningText(String text) 
	{
	    Text t = new Text(text);
	    t.setFill(Color.RED);
	    t.setStyle("-fx-font-style: italic;");
	    return t;
	}

	/**
	 * Displays a confirmation dialog asking the user if they want to start a new game or exit.
	 */
	private void showNewGameConfirmation()
	{
		Alert alert = new Alert(Alert.AlertType.CONFIRMATION);
		alert.setTitle("New Game Confirmation");
		alert.setHeaderText("Start a New Game?");
		alert.setContentText("If not, the game will exit");
		
		// remove default window graphic
		alert.setGraphic(null);
		
		// customize window icon
	    Stage stage = (Stage) alert.getDialogPane().getScene().getWindow();
		stage.getIcons().add(new Image("file:src/application/icon.png"));

		// Custom buttons
		ButtonType buttonTypeYes = new ButtonType("Yes");
		ButtonType buttonTypeNo = new ButtonType("No");

		// Remove default ButtonTypes and add custom ones
		alert.getButtonTypes().setAll(buttonTypeYes, buttonTypeNo);

		Optional<ButtonType> result = alert.showAndWait();
		if (result.isPresent())
		{
			if (result.get() == buttonTypeYes)
			{
				// User chose "Yes"
				startNewGame();
			} else if (result.get() == buttonTypeNo)
			{
				// User chose "No"
				Platform.exit();
			}
		}
	}
}