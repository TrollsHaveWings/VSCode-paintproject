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

    // Create window, Create renderer and Initiliaze texture
    SDL_Window *window = SDL_CreateWindow("Main Window",
        SDL_WINDOWPOS_UNDEFINED, 30, screenWidth, screenHeight, SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *backgroundLayer = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, screenWidth, screenHeight);
    Uint32 *backgrounLayerPixels = new Uint32[screenWidth * screenHeight];
    SDL_UpdateTexture(backgroundLayer, NULL, backgrounLayerPixels, screenWidth * sizeof(Uint32));

    // Prompt the user with an option to load an image
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
                    SDL_Surface *imageSurface = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA8888, 0);
                    if (imageSurface != nullptr) {
                        // Delete the old array and create a new one
                        delete[] backgrounLayerPixels;
                        backgrounLayerPixels = new Uint32[imageSurface->w * imageSurface->h];
                        // Copy the pixel data
                        memcpy(backgrounLayerPixels, imageSurface->pixels, imageSurface->w * imageSurface->h * sizeof(Uint32));
                        // Update the texture
                        SDL_UpdateTexture(backgroundLayer, NULL, backgrounLayerPixels, imageSurface->w * sizeof(Uint32));
                        printf("Image loaded\n");
                        imageLoaded = true;
                    } else {
                        printf("Error: Could not convert image surface.\n");
                        imageLoaded = false;
                    }
                    // Free the image surface
                    SDL_FreeSurface(imageSurface);
                } else {
                    printf("Error: Could not read file correctly. ====> Please double check the file name and file location of the image you are trying to load and try again.\n Remember FILE NAMES ARE CASE SENSITIVE\n");
                    imageLoaded = false;
                }
            } else {
                printf("Error: Unsupported File Type. ====> The only supported image files that can be loaded are .bmp files.\n");
                imageLoaded = false;
            }
        }
    // If the user does not want to load an image, then make the background layer a white screen.
    } else {
        memset(backgrounLayerPixels, 255, screenWidth * screenHeight * sizeof(Uint32));
        SDL_UpdateTexture(backgroundLayer, NULL, backgrounLayerPixels, screenWidth * sizeof(Uint32));
        printf ("Blank Canvas Created\n");
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


