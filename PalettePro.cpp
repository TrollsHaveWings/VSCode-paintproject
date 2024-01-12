#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string.h>
#include <cmath>
#include <vector>
#include <sstream>



// Compile/Make Command for Mac: g++ -std=c++11 -Wall -Wextra -pedantic -o NOAHPAINT_v12 NOAHPAINT_v12.cpp -lSDL2 -lSDL2main -lSDL2_image
// Compile/Make Command for Windows: g++ -I src/include -L src/lib NOAHPAINT_v12.cpp -o NOAHPAINT -lSDL2main -lSDL2 -lSDL2_image

int screenScale = 2;
int screenWidth = 1920/screenScale, screenHeight = 1080/screenScale;
int guiWidth = 200/screenScale;
Uint32 *backgrounLayerPixels = nullptr;

// Define Global Variables
int strokeWidth = 1;
int strokeRed = 0, strokeGreen = 0, strokeBlue = 0, strokeAlpha = 255;
int fillRed = -1, fillGreen = -1, fillBlue = -1, fillAlpha = 255;
int fillStartRed = 0, fillStartGreen = 0, fillStartBlue = 0, fillStartAlpha = 255;
int currentPixelRed = 0, currentPixelGreen = 0, currentPixelBlue = 0, currentPixelAlpha = 255;

enum Tool {
    BRUSH,
    ERASER,
    LINE,
    RECTANGLE,
    CIRCLE,
    FILL,
};
Tool currentTool = BRUSH;

//Function for drawing a filled circle when strokeWidth is greater than 1 - called in drawLine, drawCircle, drawEllipse, drawBrush
void useStrokeWidth(int xCentre, int yCentre)
{
    uint32_t strokeColour32 = 0;
    strokeColour32 |= (strokeRed & 255) << 24;
    strokeColour32 |= (strokeGreen & 255) << 16;
    strokeColour32 |= (strokeBlue & 255) << 8;
    strokeColour32 |= (strokeAlpha & 255);

    int radius = (strokeWidth+1)/2;

    int xCurrent = radius;
    int yCurrent = 0;
    int decisionParameter = 1 - xCurrent;

    while (yCurrent <= xCurrent)
    {
        if (currentTool == ERASER)
        {
            for (int xLoop = xCentre - xCurrent + 1; xLoop <= xCentre + xCurrent - 1; xLoop++)
            {
                backgrounLayerPixels[xLoop + (yCentre + yCurrent) * screenWidth] = 0xFFFFFFFF;
                backgrounLayerPixels[xLoop + (yCentre - yCurrent) * screenWidth] = 0xFFFFFFFF;
            }
            for (int xLoop = xCentre - yCurrent + 1; xLoop <= xCentre + yCurrent - 1; xLoop++)
            {
                backgrounLayerPixels[xLoop + (yCentre + xCurrent) * screenWidth] = 0xFFFFFFFF;
                backgrounLayerPixels[xLoop + (yCentre - xCurrent) * screenWidth] = 0xFFFFFFFF;
            }
        }
        else if (strokeRed != -1)
        {
            for (int xLoop = xCentre - xCurrent + 1; xLoop <= xCentre + xCurrent - 1; xLoop++)
            {
                backgrounLayerPixels[xLoop + (yCentre + yCurrent) * screenWidth] = strokeColour32;
                backgrounLayerPixels[xLoop + (yCentre - yCurrent) * screenWidth] = strokeColour32;
            }
            for (int xLoop = xCentre - yCurrent + 1; xLoop <= xCentre + yCurrent - 1; xLoop++)
            {
                backgrounLayerPixels[xLoop + (yCentre + xCurrent) * screenWidth] = strokeColour32;
                backgrounLayerPixels[xLoop + (yCentre - xCurrent) * screenWidth] = strokeColour32;
            }
        }
        if (decisionParameter < 0)
        {
            decisionParameter += 2 * yCurrent + 3;
        }
        else
        {
            decisionParameter += 2 * (yCurrent - xCurrent) + 5;
            xCurrent--;
        }
        yCurrent++;
    }
}

//Function for drawing to a single pixel or circular group of pixels when the left mouse button is held down.
void drawBrush (int x1, int y1, int x2, int y2, bool isMouseDown) {
    uint32_t strokeColour32 = 0;
    strokeColour32 |= (strokeRed & 255) << 24;
    strokeColour32 |= (strokeGreen & 255) << 16;
    strokeColour32 |= (strokeBlue & 255) << 8;
    strokeColour32 |= (strokeAlpha & 255);

    if (isMouseDown) {
        if (strokeWidth == 1)
        {
            backgrounLayerPixels[x2 + y2 * screenWidth] = strokeColour32;
        }
        else
        {
            useStrokeWidth(x2, y2);
        }
    }
}

/*Function for erasing a single pixel or circular group of pixels when the left mouse button is held down - 
same as drawBrush but with white colour.*/
void eraser (int x1, int y1, int x2, int y2, int isMouseDown) {
    if (isMouseDown)
    {
        if (strokeWidth == 1)
        {
            backgrounLayerPixels[x2 + y2 * screenWidth] = 0xFFFFFFFF;
        }
        else
        {
            useStrokeWidth(x2, y2);
        }
    }
}

//Function for drawing a line using Eike's Bressingham's line algorithm.
void drawLine (int x1, int y1, int x2, int y2, bool isMouseDown) {
    uint32_t strokeColour32 = 0;
    strokeColour32 |= (strokeRed & 255) << 24;
    strokeColour32 |= (strokeGreen & 255) << 16;
    strokeColour32 |= (strokeBlue & 255) << 8;
    strokeColour32 |= (strokeAlpha & 255);

    if (isMouseDown)
    {
        //TODO: Add a function to allow the user to draw a line using rubber banding when mouse button down
    }
    else
    {
        int xDistance = abs(x2 - x1), xStep = x1 < x2 ? 1 : -1;
        int yDistance = abs(y2 - y1), yStep = y1 < y2 ? 1 : -1;
        int errorTerm = (xDistance > yDistance ? xDistance :  -yDistance) / 2, tempError;
        while(1)
        {
            if (strokeWidth == 1)
            {
                backgrounLayerPixels[x1 + y1 * screenWidth] = strokeColour32;
            }
            else
            {
                useStrokeWidth(x1, y1);
            }
            if (x1 == x2 && y1 == y2) break;
            tempError = errorTerm;
            if (tempError > -xDistance) { errorTerm -= yDistance; x1 += xStep; }
            if (tempError < yDistance) { errorTerm += xDistance; y1 += yStep; }
        }
    }
}

/*Function for drawing a rectangle - if the fillColour is not -1, the rectangle is filled with the fillColour first
(more efficient than floodFill) then the stroke is drawn with strokeColour and strokeWidth.*/

void drawRect (int x1, int y1, int x2, int y2, bool isMouseDown)
{
    uint32_t strokeColour32 = 0;
    strokeColour32 |= (strokeRed & 255) << 24;
    strokeColour32 |= (strokeGreen & 255) << 16;
    strokeColour32 |= (strokeBlue & 255) << 8;
    strokeColour32 |= (strokeAlpha & 255);
    uint32_t fillColour32 = 0;
    fillColour32 |= (fillRed & 255) << 24;
    fillColour32 |= (fillGreen & 255) << 16;
    fillColour32 |= (fillBlue & 255) << 8;
    fillColour32 |= (fillAlpha & 255);

    if (isMouseDown)
    {
        //TODO: Add a function to allow the user to draw a rectangle using rubber banding when mouse button down
    }
    else
    {
        if (fillRed != -1)
        {
            for (int xLoop = std::min(x1, x2) ; xLoop <= std::max(x1, x2) ; xLoop++)
            {
                for (int yLoop = std::min(y1, y2) ; yLoop <= std::max(y1, y2) ; yLoop++)
                {
                    backgrounLayerPixels[xLoop + yLoop * screenWidth] = fillColour32;
                }
            }
        }

        if (strokeRed != -1)
        {
            for (int strokeLoop = 0 ; strokeLoop < strokeWidth ; strokeLoop++)
            {
                for (int xLoop = std::min(x1, x2)+strokeLoop ; xLoop <= std::max(x1, x2)-strokeLoop ; xLoop++)
                {
                    backgrounLayerPixels[xLoop + (std::min(y1, y2)+strokeLoop) * screenWidth] = strokeColour32;
                    backgrounLayerPixels[xLoop + (std::max(y1, y2)-strokeLoop) * screenWidth] = strokeColour32;
                }
                for (int yLoop = std::min(y1, y2)+strokeLoop ; yLoop <= std::max(y1, y2)-strokeLoop ; yLoop++)
                {
                    backgrounLayerPixels[(std::min(x1, x2)+strokeLoop) + yLoop * screenWidth] = strokeColour32;
                    backgrounLayerPixels[(std::max(x1, x2)-strokeLoop) + yLoop * screenWidth] = strokeColour32;
                }
            }
        }
    }
}

/* Function for drawing an ellipse - if the fillColour is not -1, the ellipse is filled with the fillColour first 
(more efficient than floodFill) then the stroke is drawn with strokeColour and strokeWidth. If either right or left shift
are held down, a circle is drawn.*/
void drawEllipse(int x1, int y1, int x2, int y2, bool isMouseDown) {
    uint32_t strokeColour32 = 0;
    strokeColour32 |= (strokeRed & 255) << 24;
    strokeColour32 |= (strokeGreen & 255) << 16;
    strokeColour32 |= (strokeBlue & 255) << 8;
    strokeColour32 |= (strokeAlpha & 255);
    uint32_t fillColour32 = 0;
    fillColour32 |= (fillRed & 255) << 24;
    fillColour32 |= (fillGreen & 255) << 16;
    fillColour32 |= (fillBlue & 255) << 8;
    fillColour32 |= (fillAlpha & 255);

    if (isMouseDown)
    {
        // TODO: Add a function to allow the user to draw a rectangle (or ellipse) using rubber banding when mouse button down
    }
    else
    {
        // Based upon midpoint ellipse algorithm description at https://www.javatpoint.com/computer-graphics-midpoint-ellipse-algorithm
        // Calculate centre of ellipse, xRadius and yRadius (for a circle these will each be the radius)
        int yCentre = (y1 + y2) / 2;
        int xCentre = (x1 + x2) / 2;
        int xRadius = abs(x2 - x1) / 2;
        int yRadius = abs(y2 - y1) / 2;
        //Radius is only used if plotting circles - ie with shift held down.
        int radius = std::min(xRadius, yRadius);

        SDL_Event ellipseEvent;
        SDL_WaitEvent(&ellipseEvent);
        switch (ellipseEvent.type)
        {
            case SDL_KEYDOWN:  // Check for a key press
                switch (ellipseEvent.key.keysym.sym) { // Check which key was pressed
                case SDLK_LSHIFT:
                    xRadius = radius;
                    yRadius = radius;
                    break;
                case SDLK_RSHIFT:
                    xRadius = radius;
                    yRadius = radius;
                    break;
                }
                break;
        }

        if (fillRed !=-1)
        {
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
                for (int xLoop = xCentre - xCurrent; xLoop <= xCentre + xCurrent; xLoop++)
                {
                    backgrounLayerPixels[xLoop + (yCentre + yCurrent) * screenWidth] = fillColour32;
                    backgrounLayerPixels[xLoop + (yCentre - yCurrent) * screenWidth] = fillColour32;
                }

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
                for (int xLoop = xCentre - xCurrent; xLoop <= xCentre + xCurrent; xLoop++)
                {
                    backgrounLayerPixels[xLoop + (yCentre + yCurrent) * screenWidth] = fillColour32;
                    backgrounLayerPixels[xLoop + (yCentre - yCurrent) * screenWidth] = fillColour32;
                }

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

        if (strokeRed !=-1)
        {
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
                if (strokeWidth == 1)
                {
                    backgrounLayerPixels[(xCentre + xCurrent) + (yCentre + yCurrent) * screenWidth] = strokeColour32;
                    backgrounLayerPixels[(xCentre - xCurrent) + (yCentre + yCurrent) * screenWidth] = strokeColour32;
                    backgrounLayerPixels[(xCentre + xCurrent) + (yCentre - yCurrent) * screenWidth] = strokeColour32;
                    backgrounLayerPixels[(xCentre - xCurrent) + (yCentre - yCurrent) * screenWidth] = strokeColour32;
                }
                else
                {
                    useStrokeWidth((xCentre + xCurrent), (yCentre + yCurrent));
                    useStrokeWidth((xCentre - xCurrent), (yCentre + yCurrent));
                    useStrokeWidth((xCentre + xCurrent), (yCentre - yCurrent));
                    useStrokeWidth((xCentre - xCurrent), (yCentre - yCurrent));
                }

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
                if (strokeWidth == 1)
                {
                    backgrounLayerPixels[(xCentre + xCurrent) + (yCentre + yCurrent) * screenWidth] = strokeColour32;
                    backgrounLayerPixels[(xCentre - xCurrent) + (yCentre + yCurrent) * screenWidth] = strokeColour32;
                    backgrounLayerPixels[(xCentre + xCurrent) + (yCentre - yCurrent) * screenWidth] = strokeColour32;
                    backgrounLayerPixels[(xCentre - xCurrent) + (yCentre - yCurrent) * screenWidth] = strokeColour32;
                }
                else
                {
                    useStrokeWidth((xCentre + xCurrent), (yCentre + yCurrent));
                    useStrokeWidth((xCentre - xCurrent), (yCentre + yCurrent));
                    useStrokeWidth((xCentre + xCurrent), (yCentre - yCurrent));
                    useStrokeWidth((xCentre - xCurrent), (yCentre - yCurrent));
                }

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
}

// Function for filling an area with the fillColour using 4-point approach outlined in Eike's lecture - may struggle with very large areas
void bucketFill (int xCurrent, int yCurrent, int isMouseDown) {
    uint32_t fillColour32 = 0;
    fillColour32 |= (fillRed & 255) << 24;
    fillColour32 |= (fillGreen & 255) << 16;
    fillColour32 |= (fillBlue & 255) << 8;
    fillColour32 |= (fillAlpha & 255);
    uint32_t fillStartColour32 = 0;
    fillStartColour32 |= (fillStartRed & 255) << 24;
    fillStartColour32 |= (fillStartGreen & 255) << 16;
    fillStartColour32 |= (fillStartBlue & 255) << 8;
    fillStartColour32 |= (fillStartAlpha & 255);

    if (!isMouseDown)
    {
        Uint32 pixelColour = backgrounLayerPixels[yCurrent * screenWidth + xCurrent];

        if (fillStartColour32 == fillColour32)
        {
            return;
        }
        if (pixelColour != fillStartColour32)
        {
            return;
        }
        else
        {
            backgrounLayerPixels[yCurrent * screenWidth + xCurrent] = fillColour32;
            bucketFill(xCurrent + 1, yCurrent, 0);
            bucketFill(xCurrent - 1, yCurrent, 0);
            bucketFill(xCurrent, yCurrent - 1, 0);
            bucketFill(xCurrent, yCurrent + 1, 0);
        }
    }
}

// Function to handle which tool to pass the variables to
void handleToolAction(int x1, int y1, int x2, int y2, bool isMouseDown, bool drawing) {
    switch (currentTool) {
        case BRUSH:
            drawBrush(x1, y1, x2, y2, isMouseDown);
            break;
        case ERASER:
            eraser(x1, y1, x2, y2, isMouseDown);
            break;
        case LINE:
            drawLine(x1, y1, x2, y2, isMouseDown);
            break;
        case RECTANGLE:
            drawRect(x1, y1, x2, y2, isMouseDown);
            break;
        case CIRCLE:
            drawEllipse(x1, y1, x2, y2, isMouseDown);
            break;
        case FILL:
            bucketFill(x2, y2, isMouseDown);
            break;
        default:
            printf("Error: Tool not implemented yet\n");
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
    SDL_Window *window = SDL_CreateWindow("BRUSH SELECTED, STROKE WIDTH 1, STROKE COLOUR (255,255,255), FILL COLOUR (-1, -1, -1)",
        SDL_WINDOWPOS_UNDEFINED, 30, screenWidth, screenHeight, SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *backgroundLayer = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, screenWidth, screenHeight);
    SDL_GetWindowPosition(window, &mainWindowX, &mainWindowY);


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

    SDL_Surface *gui_image = IMG_Load("GUI2.png");
    SDL_Texture *guiTexture = SDL_CreateTextureFromSurface(renderer, gui_image);
    SDL_FreeSurface(gui_image);
    SDL_Rect dstrect = { 0, 0, guiWidth, screenHeight };
    SDL_RenderCopy(renderer, guiTexture, NULL, &dstrect);
    //SDL_RenderPresent commits texture to video memory
    SDL_RenderPresent(renderer);


    bool quit = false;
    bool drawing = false;
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
                    currentTool = CIRCLE;
                    break;
                case SDLK_f:
                    currentTool = FILL;
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
                    if (y1 > 5 && y1 <= (screenHeight - 5) && x1 >= (guiWidth + 5) && x1 <= (screenWidth - 5))
                    {
                        drawing = true;
                    }
                    else
                    {
                        //If the user clicks on the GUI, check which tool they have selected - a little messy but it works.
                        if (y1 >=0 && y1 < 50)
                        {
                            if (x1 >= 0 && x1 < 50)
                            {
                                currentTool = BRUSH;
                            }
                            else if (x1 >= 50 && x1 < 100)
                            {
                                currentTool = ERASER;
                            }
                        }
                        else if (y1 >=50 && y1 < 100)
                        {
                            if (x1 >= 0 && x1 < 50)
                            {
                                currentTool = LINE;
                            }
                            else if (x1 >= 50 && x1 < 100)
                            {
                                currentTool = RECTANGLE;
                            }
                        }
                        else if (y1 >= 100 && y1 < 150)
                        {
                            if (x1 >= 0 && x1 < 50)
                            {
                                currentTool = CIRCLE;
                            }
                            else if (x1 >= 50 && x1 < 100)
                            {
                                currentTool = FILL;
                            }
                        }
                        else if (y1 >= 170 && y1 <= 210)
                        {
                            if (x1 >= 0 && x1 < 50)
                            {
                                //Image load call goes here
                            }
                            if (x1 >= 50 && x1 < 100)
                            {
                                //Image save call goes here
                            }                         }
                        else if (y1 >= 230 && y1 < 250)
                        {
                            if (x1 >= 0 && x1 < 20)
                            {
                                strokeWidth = 1;
                            }
                            else if (x1 >= 20 && x1 < 40)
                            {
                                strokeWidth = 3;
                            }
                            else if (x1 >= 40 && x1 < 60)
                            {
                                strokeWidth = 5;
                            }
                            else if (x1 >= 60 && x1 < 80)
                            {
                                strokeWidth = 7;
                            }
                            else if (x1 >= 80 && x1 < 100)
                            {
                                strokeWidth = 9;
                            }
                        }
                        else if (y1 >= 285 && y1 <= 305)
                        {
                            if (x1 >= 0 && x1 < 25)
                            {
                                strokeRed = -1; strokeGreen = -1; strokeBlue = -1;
                            }
                            else if (x1 >= 25 && x1 < 50)
                            {
                                strokeRed = 255; strokeGreen = 255; strokeBlue = 255;
                            }
                            else if (x1 >= 50 && x1 < 75)
                            {
                                strokeRed = 0; strokeGreen = 0; strokeBlue = 0;
                            }
                            else if (x1 >= 75 && x1 < 100)
                            {
                                strokeRed = 255; strokeGreen = 0; strokeBlue = 0;
                            }
                        }
                        else if (y1 >= 310 && y1 <= 330)
                        {
                            if (x1 >= 0 && x1 < 25)
                            {
                                strokeRed = 255; strokeGreen = 255; strokeBlue = 0;
                            }
                            else if (x1 >= 25 && x1 < 50)
                            {
                                strokeRed = 0; strokeGreen = 255; strokeBlue = 0;
                            }
                            else if (x1 >= 50 && x1 < 75)
                            {
                                strokeRed = 0; strokeGreen = 158; strokeBlue = 226;
                            }
                            else if (x1 >= 75 && x1 < 100)
                            {
                                strokeRed = 49; strokeGreen = 39; strokeBlue = 130;
                            }
                        }
                        else if (y1 >= 335 && y1 <= 355)
                        {
                            if (x1 >= 0 && x1 < 25)
                            {
                                strokeRed = 229; strokeGreen = 0; strokeBlue = 126;
                            }
                            else if (x1 >= 25 && x1 < 50)
                            {
                                strokeRed = 189; strokeGreen = 22; strokeBlue = 34;
                            }
                            else if (x1 >= 50 && x1 < 75)
                            {
                                strokeRed = 242; strokeGreen = 145; strokeBlue = 0;
                            }
                            else if (x1 >= 75 && x1 < 100)
                            {
                                strokeRed = 147; strokeGreen = 192; strokeBlue = 31;
                            }
                        }
                        else if (y1 >= 360 && y1 <= 380)
                        {
                            if (x1 >= 0 && x1 < 25)
                            {
                                strokeRed = 0; strokeGreen = 102; strokeBlue = 51;
                            }
                            else if (x1 >= 25 && x1 < 50)
                            {
                                strokeRed = 102; strokeGreen = 36; strokeBlue = 130;
                            }
                            else if (x1 >= 50 && x1 < 75)
                            {
                                strokeRed = 201; strokeGreen = 157; strokeBlue = 102;
                            }
                            else if (x1 >= 75 && x1 < 100)
                            {
                                strokeRed = 104; strokeGreen = 59; strokeBlue = 17;
                            }
                        }
                        else if (y1 >= 420 && y1 <= 440)
                        {
                            if (x1 >= 0 && x1 < 25)
                            {
                                fillRed = -1; fillGreen = -1; fillBlue = -1;
                            }
                            else if (x1 >= 25 && x1 < 50)
                            {
                                fillRed = 255; fillGreen = 255; fillBlue = 255;
                            }
                            else if (x1 >= 50 && x1 < 75)
                            {
                                fillRed = 0; fillGreen = 0; fillBlue = 0;
                            }
                            else if (x1 >= 75 && x1 < 100)
                            {
                                fillRed = 255; fillGreen = 0; fillBlue = 0;
                            }
                        }
                        else if (y1 >= 445 && y1 <= 465)
                        {
                            if (x1 >= 0 && x1 < 25)
                            {
                                fillRed = 255; fillGreen = 255; fillBlue = 0;
                            }
                            else if (x1 >= 25 && x1 < 50)
                            {
                                fillRed = 0; fillGreen = 255; fillBlue = 0;
                            }
                            else if (x1 >= 50 && x1 < 75)
                            {
                                fillRed = 0; fillGreen = 158; fillBlue = 226;
                            }
                            else if (x1 >= 75 && x1 < 100)
                            {
                                fillRed = 49; fillGreen = 39; fillBlue = 130;
                            }
                        }
                        else if (y1 >= 470 && y1 <= 490)
                        {
                            if (x1 >= 0 && x1 < 25)
                            {
                                fillRed = 229; fillGreen = 0; fillBlue = 126;
                            }
                            else if (x1 >= 25 && x1 < 50)
                            {
                                fillRed = 189; fillGreen = 22; fillBlue = 34;
                            }
                            else if (x1 >= 50 && x1 < 75)
                            {
                                fillRed = 242; fillGreen = 145; fillBlue = 0;
                            }
                            else if (x1 >= 75 && x1 < 100)
                            {
                                fillRed = 147; fillGreen = 192; fillBlue = 31;
                            }
                        }
                        else if (y1 >= 495 && y1 <= 515)
                        {
                            if (x1 >= 0 && x1 < 25)
                            {
                                fillRed = 0; fillGreen = 102; fillBlue = 51;
                            }
                            else if (x1 >= 25 && x1 < 50)
                            {
                                fillRed = 102; fillGreen = 36; fillBlue = 130;
                            }
                            else if (x1 >= 50 && x1 < 75)
                            {
                                fillRed = 201; fillGreen = 157; fillBlue = 102;
                            }
                            else if (x1 >= 75 && x1 < 100)
                            {
                                fillRed = 104; fillGreen = 59; fillBlue = 17;
                            }
                        }
                        // Update the window title to show the current tool and colour settings and stroke width to user
                        std::stringstream ss;
                        std::string toolName;
                        switch (currentTool) {
                            case 0:
                                toolName = "BRUSH";
                                break;
                            case 1:
                                toolName = "ERASER";
                                break;
                            case 2:
                                toolName = "LINE";
                                break;
                            case 3:
                                toolName = "RECTANGLE";
                                break;
                            case 4:
                                toolName = "CIRCLE";
                                break;
                            case 5:
                                toolName = "FILL";
                                break;
                            }
                            ss << toolName << " SELECTED, STROKE WIDTH " << strokeWidth << ", STROKE COLOUR (" << strokeRed << "," << strokeGreen << "," << strokeBlue << "), FILL COLOUR (" << fillRed << "," << fillGreen << "," << fillBlue << ")"; 
                            SDL_SetWindowTitle(window, ss.str().c_str());
                        }
                }
                break;
            case SDL_MOUSEMOTION:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    x2 = event.motion.x;
                    y2 = event.motion.y;
                    if (y2 > 5 && y2 <= (screenHeight - 5) && x2 >= (guiWidth + 5) && x2 <= (screenWidth - 5))
                    {
                        drawing = true;
                        handleToolAction(x1, y1, x2, y2, 1, drawing);
                    }
                    else
                    {
                        drawing = false;
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    if (drawing) {
                    drawing = false;

                    if (currentTool == FILL)
                    {
                        Uint32 fillStartColour = backgrounLayerPixels[y2 * screenWidth + x2];
                        fillStartRed = fillStartColour >> 24;
                        fillStartGreen = (fillStartColour >> 16) & 255;
                        fillStartBlue = (fillStartColour >> 8) & 255;
                        fillStartAlpha = fillStartColour & 255;
                    }
                    handleToolAction(x1, y1, x2, y2, 0, drawing);
                    }
                }
                break;
        }

        SDL_RenderCopy(renderer, backgroundLayer, NULL, NULL); // SDL_RenderCopy copies the texture to the output device
        SDL_RenderCopy(renderer, guiTexture, NULL, &dstrect);
        SDL_RenderPresent(renderer); // Add this line to update the window with the rendered texture
    }

    // Clean up, exit the program
    SDL_DestroyTexture(backgroundLayer);
    SDL_DestroyTexture(guiTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);


    SDL_Quit();
    return 0;
}