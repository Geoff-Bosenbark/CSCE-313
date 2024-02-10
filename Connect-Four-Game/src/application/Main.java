/**
 * Entry point for the Connect Four application. This class extends Application and sets up the primary stage
 * (window) for the game, including loading the main game scene from an FXML file, setting the window title, and
 * applying a custom icon. It defines the initial size of the game window and disables window resizing to preserve
 * the game layout. The application's CSS file is also applied to the scene for styling. The main method launches the
 * JavaFX application.
 */

package application;

import javafx.application.Application;
import javafx.stage.Stage;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.layout.BorderPane;
import javafx.fxml.FXMLLoader;

public class Main extends Application
{
	@Override
	public void start(Stage primaryStage)
	{
		try
		{
			FXMLLoader loader = new FXMLLoader(getClass().getResource("GameBoard.fxml"));
			BorderPane root = loader.load(); 											// Load the FXML
			Scene scene = new Scene(root, 720, 655);									// Set the scene size for tile size 90
			scene.getStylesheets().add(getClass().getResource("application.css").toExternalForm());

			primaryStage.getIcons().add(new Image("file:src/application/icon.png"));	// adding custom window icon
			primaryStage.setTitle("Connect Four");										// Set window title
			primaryStage.setScene(scene);												// Set the scene for the stage
			primaryStage.setResizable(false);											// keep window at the size it opens at
			primaryStage.show();														// Show the stage
		} catch (Exception e)
		{
			e.printStackTrace(); // prints the stack trace of any exception for debug purposes
		}
	}

	public static void main(String[] args)
	{
		launch(args);
	}
}
