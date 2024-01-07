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
void paintBrush (int x1, int y1, int x2, int y2, int mouseDown_Flag) {
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

// Function to handle which tool to pass the variables to
void handleToolAction(int x1, int y1, int x2, int y2, int mouseDown_Flag) {
    switch (currentTool) {
        case BRUSH:
            paintBrush(x1, y1, x2, y2, mouseDown_Flag);
            break;
        case !BRUSH:
            printf("Error: Tool not implemented yet\n");
            break;
    }
}

// Function to load an image
bool loadImage(std::string imageName, Uint32*& backgrounLayerPixels, SDL_Texture* backgroundLayer) {
    if (imageName.substr(imageName.find_last_of(".") + 1) != "bmp") {
        printf("Error: Unsupported File Type. ====> The only supported image files that can be loaded are .bmp files.\n");
        return false;
    }

    SDL_Surface *loadedImage = SDL_LoadBMP(imageName.c_str());
    if (loadedImage == nullptr) {
        printf("Error: Could not read file correctly. ====> Please double check the file name and file location of the image you are trying to load and try again.\n Remember FILE NAMES ARE CASE SENSITIVE\n");
        return false;
    }

    SDL_Surface *convertedImageSurface = SDL_ConvertSurfaceFormat(loadedImage, SDL_PIXELFORMAT_RGBA8888, 0);
    if (convertedImageSurface == nullptr) {
        printf("Error: Could not convert image surface.\n");
        return false;
    }

    backgrounLayerPixels = new Uint32[convertedImageSurface->w * convertedImageSurface->h];
    memcpy(backgrounLayerPixels, convertedImageSurface->pixels, convertedImageSurface->w * convertedImageSurface->h * sizeof(Uint32));
    SDL_UpdateTexture(backgroundLayer, NULL, backgrounLayerPixels, convertedImageSurface->w * sizeof(Uint32));

    SDL_FreeSurface(convertedImageSurface);
    printf("Image loaded\n");
    return true;
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

    // Init window, Init renderer, Init texture, Init pixel array
    SDL_Window *window = SDL_CreateWindow("Main Window",
        SDL_WINDOWPOS_UNDEFINED, 30, screenWidth, screenHeight, SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *backgroundLayer = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, screenWidth, screenHeight);
    Uint32 *backgrounLayerPixels = nullptr;

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
            imageLoaded = loadImage(imageName, backgrounLayerPixels, backgroundLayer);
        }
    // If the user does not want to load an image, then make the background layer a white screen.
    } else {
        backgrounLayerPixels = new Uint32[screenWidth * screenHeight];
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
                    break;
                }
        break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                x1 = event.motion.x;
                y1 = event.motion.y;
                x2 = event.motion.x;
                y2 = event.motion.y;
                }
                break;
            case SDL_MOUSEMOTION:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                x2 = event.motion.x;
                y2 = event.motion.y;
                handleToolAction(x1, y1, x2, y2, 1);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                handleToolAction(x1, y1, x2, y2, 0);
                }
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


