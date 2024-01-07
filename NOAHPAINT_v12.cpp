#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <iostream>
#include <string>

// Define Global Variables
enum Tool {
    BRUSH,
    ERASER,
    LINE,
    RECTANGLE,
    CIRCLE,
    FILL,
    EYEDROPPER, // not essential
};
Tool currentTool = BRUSH;

// TODO: Add a function to allow the user to paint on the screen
void paintBrush (int x1, int y1, int x2, int y2) {
    printf("%d %d %d %d\n", x1, y1, x2, y2);
}

// TODO: Add a function to allow the user to erase parts of the screen
void eraser () {

}

// TODO: Add a function to allow the user to draw a line
void line () {

}

// TODO: Add a function to allow the user to draw a rectangle
void rectangle () {

}

// TODO: Add a function to allow the user to draw a circle
void circle () {

}

void handleToolAction(int x1, int y1, int x2, int y2) {
    switch (currentTool) {
        case BRUSH:
            paintBrush(x1, y1, x2, y2);
            break;
        case !BRUSH:
            printf("Error: Tool not implemented yet\n");
            break;
    }
}

// --------------------------------MAIN FUNCTION--------------------------------

int main(int argc, char **argv)
{
    // Define local main function variables
    bool quit = false;
    int screenWidth = 1920/2, screenHeight = 1080/2;
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;

    // Init SDL with video subsystem
    SDL_Init(SDL_INIT_VIDEO);

    // Create window and renderer
    SDL_Window *window = SDL_CreateWindow("Main Window",
        SDL_WINDOWPOS_UNDEFINED, 30, screenWidth, screenHeight, SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *backgroundLayer = nullptr; // Declare the backgroundLayer variable

    // Prompt the user for an option
    std::cout << "Do you want to load an image? (y/n): ";
    char imageLoadOption;
    std::cin >> imageLoadOption;

    if (imageLoadOption == 'y' || imageLoadOption == 'Y') {
        bool imageLoaded = false;
        while (!imageLoaded) {
            // Prompt the user for an image to open in console
            std::cout << "Enter the name of the image you would like to open: ";
            std::string imageName;
            std::cin >> imageName;

            // Check if the file extension is .bmp
            if (imageName.substr(imageName.find_last_of(".") + 1) == "bmp") {
                SDL_Surface *image = SDL_LoadBMP(imageName.c_str());
                // Check if the file was loaded correctly
                if (image != nullptr) {
                    backgroundLayer = SDL_CreateTextureFromSurface(renderer, image);
                    SDL_FreeSurface(image);
                    printf("Image loaded\n");
                    imageLoaded = true;
                } else {
                    printf("Error: Could not read file correctly. ====> Please double check the file name and file location of the image you are trying to load and try again.\n Remember FILE NAMES ARE CASE SENSITIVE\n");
                    imageLoaded = false;
                }
            } else {
                printf("Error: Unsupported File Type. ====> The only supported image files that can be loaded are .bmp files.\n");
            }
        }
    // FIXME: This is where the program is supposed to create a blank white surface, its working until the user hovers over it with the mouse.
    } else {
        // Create texture from a blank surface
        SDL_Surface *blankSurface = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32, 255, 255, 255, 255);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        backgroundLayer = SDL_CreateTextureFromSurface(renderer, blankSurface);
        SDL_FreeSurface(blankSurface);
        printf("Blank surface created\n");
    }

    // Main Program loop
    SDL_Event event;
    while (!quit)
    {
        SDL_WaitEvent(&event);

        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:  // Check for a key press
                switch (event.key.keysym.sym)  // Check which key was pressed
                {
                case SDLK_b:
                    currentTool = BRUSH;
                    handleToolAction(x1, y1, x2, y2);
                    break;
                }
        break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                x1 = event.motion.x;
                y1 = event.motion.y;
                }
                break;
            case SDL_MOUSEMOTION:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                x2 = event.motion.x;
                y2 = event.motion.y;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                break;
        }

        SDL_RenderCopy(renderer, backgroundLayer, NULL, NULL); // SDL_RenderCopy copies the texture to the output device
        SDL_RenderPresent(renderer); // Add this line to update the window with the rendered texture
    }

    // Clean up, exit the program
    SDL_DestroyTexture(backgroundLayer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


