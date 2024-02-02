======================================================================================================================================
|	    					           ＣＯＮＮＥＣＴ ＦＯＵＲ          	                       		             |
======================================================================================================================================

							   Table of Contents
							   -----------------
							   1. Description
							   2. Included Files
							   3. Demo Video
							   4. Credits

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\
/	1. Description														      \
\	---------------														      /
/	This implementation of Connect Four allows for a single player to play against 'Easy' or 'Hard' AI variants		      \
\	The pieces fall straight down, occupying the next available space within the column.                                          /
/	The objective of the game is to be the first to form a horizontal, vertical, or diagonal line of four of one's own chips.     \
\															      	      /
/	2. Included Files													      \
\	-----------------													      /
/	- 'Main.java': Initializes the game environment, creates main game window, and starts game loop.			      \
\	- 'GameController.java': The main controller class for the game, handling the user interactions and game logic.		      /
/	- 'AI.java': An abstract class that defines the structure for AI players in the game.					      \
\	- 'SmartAI.java': A subclass of AI that implements a more strategic approach to playing the game.			      /
/	- 'RandomAI.java': A subclass of AI that makes random moves.								      \
\	- 'Chip.java': Represents a single chip in the game with its color and properties.					      /
/	- 'Move.java': A utility class used to encapsulate the column and row indices of a potential move on the board		      \
\	- 'GameBoard.fxml': The FXML file defining the UI layout for the game.							      /
/																      \
\	3. Demo Video (playback quality should be adjustable up to 2160p, 4K)							      /
/	-------------														      \
\	- Demo video showing the source code and the game features: https://youtu.be/J-uy46p7uug		      		      /																      /
/																      \
\	4. Credits														      /
/	----------														      \
\	- Author: Geoffrey Bosenbark												      /
/	- UIN:	  232002930													      \
\	- Course: CSCE-314-598													      /
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////