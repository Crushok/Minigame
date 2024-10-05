#include <iostream>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::ostream;
std::string output;

using std::vector;

// Function to set the console window size (for example, 1280x720)
void SetConsoleSize(int width, int height)
{
    // Get the console handle
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Set the buffer size
    COORD bufferSize;
    bufferSize.X = width / 8;   // Character columns (based on approx 8px per column)
    bufferSize.Y = height / 16; // Character rows (based on approx 16px per row)
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    // Set the console window size
    SMALL_RECT windowSize;
    windowSize.Top = 0;
    windowSize.Left = 0;
    windowSize.Right = bufferSize.X - 1;
    windowSize.Bottom = bufferSize.Y - 1;
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
}

// Enum to represent the possible directions of the ball
enum eDir { STOP = 0, LEFT = 1, UPLEFT = 2, DOWNLEFT = 3, RIGHT = 4, UPRIGHT = 5, DOWNRIGHT = 6 };

// Class representing the ball in the game
class cPelota
{
private:
    int x, y; // Current coordinates of the ball
    int originalX, originalY; // Original coordinates where the ball starts
    eDir direction; // Current direction of the ball

public:
    // Constructor to initialize the ball's position and direction
    cPelota(int posX, int posY)
    {
        originalX = posX;
        originalY = posY;
        x = posX;
        y = posY;
        direction = STOP; // Start with the ball not moving
    }

    // Method to reset the ball's position to the original coordinates
    void Reset()
    {
        x = originalX;
        y = originalY;
        direction = STOP;
    }

    // Method to change the direction of the ball
    void CambioDir(eDir d)
    {
        direction = d;
    }

    // Randomly assigns a new direction to the ball when needed
    void randomDirection()
    {
        direction = (eDir)((rand() % 6) + 1); // Randomizes between 1 and 6 directions
    }

    // Getters to retrieve the ball's current position and direction
    inline int getX() { return x; }
    inline int getY() { return y; }
    inline eDir getDireccion() { return direction; }

    // Moves the ball according to its current direction
    void Movimiento()
    {
        switch (direction)
        {
        case STOP:
            break;
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
        case UPLEFT:
            x--; y--;
            break;
        case DOWNLEFT:
            x--; y++;
            break;
        case UPRIGHT:
            x++; y--;
            break;
        case DOWNRIGHT:
            x++; y++;
            break;
        default:
            break;
        }
    }

    // Overloads the << operator to print the ball's state (position and direction)
    friend ostream& operator<<(ostream& o, cPelota c)
    {
        o << "Ball [" << c.x << "," << c.y << "][" << c.direction << "]";
        return o;
    }
};

// Class representing the paddles used by the players
class cPaleta
{
private:
    int x, y; // Current coordinates of the paddle
    int originalX, originalY; // Original coordinates where the paddle starts

public:
    // Default constructor initializes the paddle position to (0,0)
    cPaleta()
    {
        x = y = 0;
    }

    // Constructor to initialize paddle's position
    cPaleta(int posX, int posY) : cPaleta()
    {
        originalX = posX;
        originalY = posY;
        x = posX;
        y = posY;
    }

    // Resets the paddle's position to the original coordinates
    inline void Reset() { x = originalX; y = originalY; }

    // Getters for the paddle's current position
    inline int getX() { return x; }
    inline int getY() { return y; }

    // Moves the paddle up or down
    inline void moveUp() { y--; }
    inline void moveDown() { y++; }

    // Overloads the << operator to print the paddle's state (position)
    friend ostream& operator<<(ostream& o, cPaleta c)
    {
        o << "Paddle [" << c.x << "," << c.y << "]";
        return o;
    }
};

// Class to manage the game, including ball movement, paddle control, scoring, and rendering
class cManejoJuego
{
private:
    int width, height; // Width and height of the game board
    int score1, score2; // Scores of player 1 and player 2
    char up1, down1, up2, down2; // Keys to control the paddles
    bool quit; // Flag to indicate if the game should stop
    cPelota* ball; // The ball object
    cPaleta* player1; // Player 1's paddle
    cPaleta* player2; // Player 2's paddle

public:
    // Constructor to initialize the game with a specific board size
    cManejoJuego(int w, int h)
    {
        srand(time(NULL)); // Seed for randomizing the ball direction
        quit = false; // Start the game in a running state
        up1 = 'w'; up2 = 'o'; // Keys for moving paddles up
        down1 = 's'; down2 = 'l'; // Keys for moving paddles down
        score1 = score2 = 0; // Initial scores
        width = w; height = h;
        ball = new cPelota(w / 2, h / 2); // Ball starts at the center
        player1 = new cPaleta(1, h / 2 - 3); // Player 1 paddle on the left side
        player2 = new cPaleta(w - 2, h / 2 - 3); // Player 2 paddle on the right side
    }

    // Destructor to clean up dynamically allocated memory
    ~cManejoJuego()
    {
        delete ball;
        delete player1;
        delete player2;
    }

    // Increments the score of the appropriate player when they score
    void puntajes(cPaleta* player)
    {
        if (player == player1)
            score1++;
        else if (player == player2)
            score2++;

        // Reset ball and paddle positions after each score
        ball->Reset();
        player1->Reset();
        player2->Reset();
    }

    // Renders the game board, paddles, and ball
    void renderizado()
    {
        // Moves the cursor to the top of the screen to overwrite previous frame
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0,0 });

        // Draw top border
        for (int i = 0; i < width + 2; i++) cout << "\xB2";
        cout << endl;

        // Draw the game board row by row
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                // Get ball and paddle positions
                int ballx = ball->getX();
                int bally = ball->getY();
                int player1x = player1->getX();
                int player2x = player2->getX();
                int player1y = player1->getY();
                int player2y = player2->getY();

                // Draw left border
                if (j == 0) cout << "\xB2";

                // Draw the ball or paddles, or leave empty space
                if (ballx == j && bally == i)
                    cout << "O"; // Ball
                else if (player1x == j && (player1y <= i && i < player1y + 4))
                    cout << "\xDB"; // Player 1 paddle
                else if (player2x == j && (player2y <= i && i < player2y + 4))
                    cout << "\xDB"; // Player 2 paddle
                else
                    cout << " "; // Empty space

                // Draw right border
                if (j == width - 1) cout << "\xB2";
            }
            cout << endl;
        }

        // Draw bottom border
        for (int i = 0; i < width + 2; i++) cout << "\xB2";
        cout << endl;

        // Display the current scores
        cout << "Score 1: " << score1 << "   Score 2: " << score2 << endl;

        // Display the controls for Player 1 and Player 2
        cout << endl;
        cout << "Controls for Player 1: " << endl;
        cout << "  Move Up: " << up1 << endl;
        cout << "  Move Down: " << down1 << endl;

        cout << endl;
        cout << "Controls for Player 2: " << endl;
        cout << "  Move Up: " << up2 << endl;
        cout << "  Move Down: " << down2 << endl;

        cout << endl;
        cout << "Press 'q' to quit the game." << endl;
    }

    // Handles player inputs and paddle movement
    void entradas()
    {
        // Move the ball
        ball->Movimiento();

        // Check if any key is pressed
        if (_kbhit())
        {
            char current = _getch(); // Get the key pressed
            // Control player 1's paddle
            if (current == up1 && player1->getY() > 0)
                player1->moveUp();
            if (current == down1 && player1->getY() + 4 < height)
                player1->moveDown();
            // Control player 2's paddle
            if (current == up2 && player2->getY() > 0)
                player2->moveUp();
            if (current == down2 && player2->getY() + 4 < height)
                player2->moveDown();

            // Start the ball movement if it is stopped
            if (ball->getDireccion() == STOP)
                ball->randomDirection();

            // Quit the game if 'q' is pressed
            if (current == 'q')
                quit = true;
        }
    }

    // Contains the game's logic, including collisions and scoring
    void Logica()
    {
        // Get ball and paddle positions
        int ballx = ball->getX();
        int bally = ball->getY();
        int player1x = player1->getX();
        int player2x = player2->getX();
        int player1y = player1->getY();
        int player2y = player2->getY();

        // Check for ball collisions with Player 1's paddle
        if (ballx == player1x + 1 && bally >= player1y && bally < player1y + 4)
            ball->CambioDir((eDir)((rand() % 3) + 4));

        // Check for ball collisions with Player 2's paddle
        if (ballx == player2x - 1 && bally >= player2y && bally < player2y + 4)
            ball->CambioDir((eDir)((rand() % 3) + 1));

        // Check for ball collisions with the top wall
        if (bally == 0)
            ball->CambioDir(ball->getDireccion() == UPRIGHT ? DOWNRIGHT : DOWNLEFT);

        // Check for ball collisions with the bottom wall
        if (bally == height - 1)
            ball->CambioDir(ball->getDireccion() == DOWNRIGHT ? UPRIGHT : UPLEFT);

        // Check if Player 1 scores
        if (ballx == width - 1)
            puntajes(player1);

        // Check if Player 2 scores
        if (ballx == 0)
            puntajes(player2);
    }

    // Main game loop: runs the game until the user quits
    void Ejecucion()
    {
        while (!quit)
        {
            renderizado(); // Draw the game board
            entradas();    // Handle player input
            Logica();      // Handle ball movement and collisions
            Sleep(50);     // Delay to control frame rate and reduce flickering
        }
    }
};

// Main function to start the game
int main()
{
    // Set console size to 1280x720
    SetConsoleSize(1280, 720);
    
    cManejoJuego c(40, 20); // Create a game with a board size of 40x20
    c.Ejecucion(); // Start the game loop
    return 0;
}
