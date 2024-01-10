#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string.h>
#include <cmath>
#include <vector>


// Compile/Make Command for Mac: g++ -std=c++11 -Wall -Wextra -pedantic -o NOAHPAINT_v12 NOAHPAINT_v12.cpp -lSDL2 -lSDL2main -lSDL2_image
// Compile/Make Command for Windows: g++ -I src/include -L src/lib NOAHPAINT_v12.cpp -o NOAHPAINT -lSDL2main -lSDL2 -lSDL2_image

int screenScale = 2;
int screenWidth = 1920/screenScale, screenHeight = 1080/screenScale;
Uint32 *backgrounLayerPixels = nullptr;

// Define Global Variables
enum Tool {
    BRUSH,
    ERASER,
    LINE,
    RECTANGLE,
    ELLIPSE,
    FILL,
    EYEDROPPER, // not essential
};
Tool currentTool = BRUSH;

// TODO: Make this function draw to backgroundLayerPixels array
void drawBrush (int x1, int y1, int x2, int y2, bool isMouseDown) {
    printf("PAINT BRUSH MOTION %d %d %d %d %d\n", x1, y1, x2, y2, isMouseDown
    );
    if (isMouseDown
    )
    {
        backgrounLayerPixels[x2 + y2 * screenWidth] = 0;
    }
}

// TODO: Add a function to allow the user to erase parts of the screen
void eraser () {

}

// TODO: Make this function draw to the BackgroundLayerPixels array
void drawLine (int x1, int y1, int x2, int y2, bool isMouseDown)
{
    if (isMouseDown
    )
    {
        printf("LINE MOTION %d %d %d %d %d\n", x1, y1, x2, y2, isMouseDown
        );
        //TODO: Add a function to allow the user to draw a line using rubber banding when mouse button down
    }
    else
    {
        printf("LINE BUTTON UP %d %d %d %d %d\n", x1, y1, x2, y2, isMouseDown
        );

        // Bresenham's line algorithm
        int xDistance = abs(x2 - x1), xStep = x1 < x2 ? 1 : -1;
        int yDistance = abs(y2 - y1), yStep = y1 < y2 ? 1 : -1;
        int errorTerm = (xDistance > yDistance ? xDistance :  yDistance) / 2, tempError;
        while(1)
        {
            backgrounLayerPixels[x1 + y1 * screenWidth] = 0;
            if (x1 == x2 && y1 == y2) break;
            tempError = errorTerm;
            if (tempError > -xDistance) { errorTerm -= yDistance; x1 += xStep; }
            if (tempError < yDistance) { errorTerm += xDistance; y1 += yStep; }
        }
    }
}

// TODO: Make this function draw to the backgroundLayerPixels array
void drawRect (int x1, int y1, int x2, int y2, bool isMouseDown)
{
    if (isMouseDown
    )
    {
        printf("RECT MOTION %d %d %d %d %d\n", x1, y1, x2, y2, isMouseDown
        );
        //TODO: Add a function to allow the user to draw a rectangle using rubber banding when mouse button down
    }
    else
    {
        printf("RECT BUTTON UP %d %d %d %d %d\n", x1, y1, x2, y2, isMouseDown
        );

        for (int yLoop = std::min(x1, x2) ; yLoop <= std::max(x1, x2) ; yLoop++)
        {
            backgrounLayerPixels[yLoop + y1 * screenWidth] = 0;
            backgrounLayerPixels[yLoop + y2 * screenWidth] = 0;
        }
        for (int xLoop = std::min(y1, y2) ; xLoop <= std::max(y1, y2) ; xLoop++)
        {
            backgrounLayerPixels[x1 + xLoop * screenWidth] = 0;
            backgrounLayerPixels[x2 + xLoop * screenWidth] = 0;
        }
    }
}


void drawEllipse(int x1, int y1, int x2, int y2, bool isMouseDown)
{
    if (isMouseDown)
    {
        printf("ELLIPSE MOTION %d %d %d %d %d\n", x1, y1, x2, y2, isMouseDown);
        // TODO: Add a function to allow the user to draw a rectangle (or ellipse) using rubber banding when mouse button down
    }
    else
    {
        printf("ELLIPSE BUTTON UP %d %d %d %d %d\n", x1, y1, x2, y2, isMouseDown);
        // Based upon midpoint ellipse algorithm description at https://www.javatpoint.com/computer-graphics-midpoint-ellipse-algorithm
        // Calculate centre of ellipse, xRadius and yRadius (for a circle these will each be the radius)

        int yCenter = (y1 + y2) / 2;
        int xCenter = (x1 + x2) / 2;
        int xRadius = abs(x2 - x1) / 2;
        int yRadius = abs(y2 - y1) / 2;

        int xCurrent = 0;
        int yCurrent = yRadius;
        int aSquared = xRadius * xRadius;
        int bSquared = yRadius * yRadius;
        int twoASquared = 2 * aSquared;
        int twoBSquared = 2 * bSquared;
        int fourASquared = 4 * aSquared;
        int fourBSquared = 4 * bSquared;
        int xEnd = round(aSquared / sqrt(aSquared + bSquared));

        int xDistance = 0;
        int yDistance = fourASquared * yCurrent;

        // Region 1 - gradient less than 1
        int decisionParameter = round(bSquared - (aSquared * yRadius) + (0.25 * aSquared));
        while (xDistance <= yDistance)
        {
            // Commented out lines fill out the ellipse in red
            /*for (int i = xCenter - xCurrent; i <= xCenter + xCurrent; i++)
            {
                backgrounLayerPixels[i + (yCenter + yCurrent) * screenWidth] = 0xFF0000FF;
                backgrounLayerPixels[i + (yCenter - yCurrent) * screenWidth] = 0xFF0000FF;
            }*/
            backgrounLayerPixels[xCenter + xCurrent + (yCenter + yCurrent) * screenWidth] = 0;
            backgrounLayerPixels[xCenter - xCurrent + (yCenter + yCurrent) * screenWidth] = 0;
            backgrounLayerPixels[xCenter + xCurrent + (yCenter - yCurrent) * screenWidth] = 0;
            backgrounLayerPixels[xCenter - xCurrent + (yCenter - yCurrent) * screenWidth] = 0;

            xCurrent++;
            xDistance += fourBSquared;

            if (decisionParameter < 0)
            {
                decisionParameter += xDistance + twoBSquared;
            }
            else
            {
                yCurrent--;
                yDistance -= fourASquared;
                decisionParameter += xDistance - yDistance + twoBSquared;
            }
        }

        // Region 2 - gradient greater than 1
        decisionParameter = round(bSquared * (xCurrent + 0.5) * (xCurrent + 0.5) + aSquared * (yCurrent - 1) * (yCurrent - 1) - aSquared * bSquared);
        while (yCurrent >= 0)
        {
            // Commented out lines fill out the ellipse in red
            /*for (int i = xCenter - xCurrent; i <= xCenter + xCurrent; i++)
            {
                backgrounLayerPixels[i + (yCenter + yCurrent) * screenWidth] = 0xFF0000FF;
                backgrounLayerPixels[i + (yCenter - yCurrent) * screenWidth] = 0xFF0000FF;
            }*/
            backgrounLayerPixels[xCenter + xCurrent + (yCenter + yCurrent) * screenWidth] = 0;
            backgrounLayerPixels[xCenter - xCurrent + (yCenter + yCurrent) * screenWidth] = 0;
            backgrounLayerPixels[xCenter + xCurrent + (yCenter - yCurrent) * screenWidth] = 0;
            backgrounLayerPixels[xCenter - xCurrent + (yCenter - yCurrent) * screenWidth] = 0;

            yCurrent--;
            yDistance -= fourASquared;

            if (decisionParameter > 0)
            {
                decisionParameter += aSquared - yDistance;
            }
            else
            {
                xCurrent++;
                xDistance += fourBSquared;
                decisionParameter += aSquared - yDistance + xDistance;
            }
        }
    }
}

// Function to handle which tool to pass the variables to
void handleToolAction(int x1, int y1, int x2, int y2, bool isMouseDown
) {
    switch (currentTool) {
        case BRUSH:
            drawBrush(x1, y1, x2, y2, isMouseDown
            );
            break;
        case LINE:
            drawLine(x1, y1, x2, y2, isMouseDown
            );
            break;
        case RECTANGLE:
            drawRect(x1, y1, x2, y2, isMouseDown
            );
            break;
        case ELLIPSE:
            drawEllipse(x1, y1, x2, y2, isMouseDown
            );
            break;
        default:
            printf("errorTermor: Tool not implemented yet\n");
            break;
    }
}

// Function to load an image
bool loadImage(std::string imageName, Uint32*& backgrounLayerPixels, SDL_Texture* backgroundLayer) {
    std::vector<std::string> supportedExtensions = { "bmp", "png", "jpg", "jpeg", "gif" };
    std::string fileExtension = imageName.substr(imageName.find_last_of(".") + 1);
    bool isSupported = false;

    for (const std::string& extension : supportedExtensions) {
        if (fileExtension == extension) {
            isSupported = true;
            break;
        }
    }

    if (!isSupported) {
        printf("errorTermor: Unsupported File Type. ====> The only supported image files that can be loaded are .bmp, .png, .jpg, .jpeg, and .gif files.\n");
        return false;
    }

    SDL_Surface* loadedImage = IMG_Load(imageName.c_str());
    if (loadedImage == nullptr) {
        printf("errorTermor: Could not read file correctly. ====> Please double check the file name and file location of the image you are trying to load and try again.\n Remember FILE NAMES ARE CASE SENSITIVE\n");
        return false;
    }

    SDL_Surface* convertedImageSurface = SDL_ConvertSurfaceFormat(loadedImage, SDL_PIXELFORMAT_RGBA8888, 0);
    if (convertedImageSurface == nullptr) {
        printf("errorTermor: Could not convert image surface.\n");
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
    // Init SDL with video subyStepstem
    SDL_Init(SDL_INIT_VIDEO);

    int mainWindowX, mainWindowY;

    // Creating Main Window
    SDL_Window *window = SDL_CreateWindow("Main Window",
        SDL_WINDOWPOS_UNDEFINED, 30, screenWidth, screenHeight, SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *backgroundLayer = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, screenWidth, screenHeight);
    SDL_GetWindowPosition(window, &mainWindowX, &mainWindowY);

    // Creating GUI Window
    SDL_Window *GUI = SDL_CreateWindow(NULL,
        SDL_WINDOWPOS_UNDEFINED, 30, 200/screenScale, 1080/screenScale, SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer *GUIrenderer = SDL_CreateRenderer(GUI, -1, 0);
    SDL_Surface *GUIsurface = IMG_Load("GUI.png");
    SDL_Texture *GUItexture = SDL_CreateTextureFromSurface(GUIrenderer, GUIsurface);
    SDL_FreeSurface(GUIsurface);
    SDL_RenderCopy(GUIrenderer, GUItexture, NULL, NULL);
    SDL_RenderPresent(GUIrenderer);
    SDL_SetWindowPosition(GUI, mainWindowX - 200/screenScale, mainWindowY);

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

    bool quit = false;
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
    SDL_Event event;

    // Main Program loop
    while (!quit)
    {
        SDL_UpdateTexture(backgroundLayer, NULL, backgrounLayerPixels, screenWidth * sizeof(Uint32));

        SDL_WaitEvent(&event);

        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_CLOSE:
                        if (event.window.windowID == SDL_GetWindowID(window)) {
                            quit = true;
                        }
                        if (event.window.windowID == SDL_GetWindowID(GUI)) {
                            quit = true;
                        }
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        if (event.window.windowID == SDL_GetWindowID(window)) {
                            // The main window gained focus, raise the GUI window
                            SDL_RaiseWindow(GUI);
                        } else if (event.window.windowID == SDL_GetWindowID(GUI)) {
                            // The GUI window gained focus, raise the main window
                            SDL_RaiseWindow(window);
                        }
                        break;
                    case SDL_WINDOWEVENT_MINIMIZED:
                        if (event.window.windowID == SDL_GetWindowID(window)) {
                            // The main window was minimized, minimize the GUI window
                            SDL_MinimizeWindow(GUI);
                        } else if (event.window.windowID == SDL_GetWindowID(GUI)) {
                            // The GUI window was minimized, minimize the main window
                            SDL_MinimizeWindow(window);
                        }
                        break;
                    case SDL_WINDOWEVENT_MAXIMIZED:
                        if (event.window.windowID == SDL_GetWindowID(window)) {
                            // The main window was maximized, maximize the GUI window
                            SDL_MaximizeWindow(GUI);
                        } else if (event.window.windowID == SDL_GetWindowID(GUI)) {
                            // The GUI window was maximized, maximize the main window
                            SDL_MaximizeWindow(window);
                        }
                        break;
                    case SDL_WINDOWEVENT_RESTORED:
                    if (event.window.windowID == SDL_GetWindowID(window)) {
                            // The main window was restored, restore the GUI window
                            SDL_RestoreWindow(GUI);
                        } else if (event.window.windowID == SDL_GetWindowID(GUI)) {
                            // The GUI window was restored, restore the main window
                            SDL_RestoreWindow(window);
                        }
                        break;
                    default:
                        if (event.window.windowID == SDL_GetWindowID(window)) {
                            // The main window was moved
                            int x, y;
                            SDL_GetWindowPosition(window, &x, &y);
                            SDL_SetWindowPosition(GUI, x - (200/screenScale), y);  // Assuming the width of the GUI window is 200
                        }
                        break;
                }
                break;
            case SDL_KEYDOWN:  // Check for a key press
                switch (event.key.keysym.sym) { // Check which key was pressed
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
                    currentTool = ELLIPSE;
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
    SDL_DestroyTexture(GUItexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyRenderer(GUIrenderer);

    SDL_DestroyWindow(window);
    SDL_DestroyWindow(GUI);

    SDL_Quit();
    return 0;
}
