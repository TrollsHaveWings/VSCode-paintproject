#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <iostream>
#include <string>

// Compile/Make Command for Mac: g++ -std=c++11 -Wall -Wextra -pedantic -o NOAHPAINT_v12 NOAHPAINT_v12.cpp -lSDL2 -lSDL2main -lSDL2_image
// Compile/Make Command for Windows: g++ -I src/include -L src/lib NOAHPAINT_v12.cpp -o NOAHPAINT -lSDL2main -lSDL2 -lSDL2_image

int screenWidth = 1920/2, screenHeight = 1080/2;
Uint32 *backgrounLayerPixels = nullptr;

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

// TODO: Make this function draw to backgroundLayerPixels array
void drawBrush (int x1, int y1, int x2, int y2, int mouseDown_Flag) {
    printf("PAINT BRUSH MOTION %d %d %d %d %d\n", x1, y1, x2, y2, mouseDown_Flag);
    if (mouseDown_Flag)
    {
        backgrounLayerPixels[x2 + y2 * screenWidth] = 0;
    }
}

// TODO: Add a function to allow the user to erase parts of the screen
void eraser () {

}

// TODO: Make this function draw to the BackgroundLayerPixels array
void drawLine (int x1, int y1, int x2, int y2, int mouseDown_Flag) {
    if (mouseDown_Flag)
    {
        printf("LINE MOTION %d %d %d %d %d\n", x1, y1, x2, y2, mouseDown_Flag);
        //TODO: Add a function to allow the user to draw a line using rubber banding when mouse button down
    }
    else
    {
        printf("LINE BUTTON UP %d %d %d %d %d\n", x1, y1, x2, y2, mouseDown_Flag);

        int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
        int dy = abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
        int err = (dx > dy ? dx : -dy) / 2, e2;
        while(1)
        {
            backgrounLayerPixels[x1 + y1 * screenWidth] = 0;
            if (x1 == x2 && y1 == y2) break;
            e2 = err;
            if (e2 > -dx) { err -= dy; x1 += sx; }
            if (e2 < dy) { err += dx; y1 += sy; }
        }
    }
}

// TODO: Make this function draw to the backgroundLayerPixels array
void drawRect (int x1, int y1, int x2, int y2, int mouseDown_Flag) {
    if (mouseDown_Flag)
    {
        printf("RECT MOTION %d %d %d %d %d\n", x1, y1, x2, y2, mouseDown_Flag);
        //TODO: Add a function to allow the user to draw a rectangle using rubber banding when mouse button down
    }
    else
    {
        printf("RECT BUTTON UP %d %d %d %d %d\n", x1, y1, x2, y2, mouseDown_Flag);

        for (int i = std::min(x1, x2) ; i <= std::max(x1, x2) ; i++)
        {
            backgrounLayerPixels[i + y1 * screenWidth] = 0;
            backgrounLayerPixels[i + y2 * screenWidth] = 0;
        }
        for (int j = std::min(y1, y2) ; j <= std::max(y1, y2) ; j++)
        {
            backgrounLayerPixels[x1 + j * screenWidth] = 0;
            backgrounLayerPixels[x2 + j * screenWidth] = 0;
        }
    }
}

// TODO: Add a function to allow the user to draw a circle
void drawCircle () {

}

// Function to handle which tool to pass the variables to
void handleToolAction(int x1, int y1, int x2, int y2, int mouseDown_Flag) {
    switch (currentTool) {
        case BRUSH:
            drawBrush(x1, y1, x2, y2, mouseDown_Flag);
            break;
        case LINE:
            drawLine(x1, y1, x2, y2, mouseDown_Flag);
            break;
        case RECTANGLE:
            drawRect(x1, y1, x2, y2, mouseDown_Flag);
            break;
        default:
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
                case SDLK_e:
                    currentTool = ERASER;
                    break;
                case SDLK_l:
                    currentTool = LINE;
                    break;
                case SDLK_r:
                    currentTool = RECTANGLE;
                    break;
                case SDLK_c:
                    currentTool = CIRCLE;
                    break;
                case SDLK_f:
                    currentTool = FILL;
                    break;
                case SDLK_i:
                    currentTool = EYEDROPPER;
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


