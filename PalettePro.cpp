#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string.h>
#include <cmath>
#include <vector>
#include <sstream>
#include <functional>
#include "tinyfiledialogs.h" //external library for native file dialogs


// Compile/Make Command: g++ -std=c++11 -Wall -Wextra -pedantic -o PalettePro PalettePro.cpp -lSDL2 -lSDL2main -lSDL2_image

/* --------------------------------DECLARE GLOBAL VARIABLES-------------------------------- */

// Window and Renderer Variables
int screenScale = 2;
int screenWidth = 1920/screenScale, screenHeight = 1080/screenScale;
int guiWidth = 200/screenScale;
Uint32 *backgrounLayerPixels = nullptr;
// Tool Variables
enum Tool {
    BRUSH,
    ERASER,
    LINE,
    RECTANGLE,
    CIRCLE,
    FILL,
};
Tool currentTool = BRUSH;
// Tool Setting Variables
int strokeWidth = 1;
int strokeRed = 0, strokeGreen = 0, strokeBlue = 0, strokeAlpha = 255;
int fillRed = -1, fillGreen = -1, fillBlue = -1, fillAlpha = 255;
int fillStartRed = 0, fillStartGreen = 0, fillStartBlue = 0, fillStartAlpha = 255;
int currentPixelRed = 0, currentPixelGreen = 0, currentPixelBlue = 0, currentPixelAlpha = 255;



/* --------------------------------FUNCTIONS-------------------------------- */

void useStrokeWidth(int xCentre, int yCentre)
//Function for drawing a filled circle when strokeWidth is greater than 1 - called in drawLine, drawCircle, drawEllipse, drawBrush
{
    // Convert stroke and fill colors to 32-bit format
    uint32_t strokeColour32 = (strokeRed & 255) << 24 | (strokeGreen & 255) << 16 | (strokeBlue & 255) << 8 | (strokeAlpha & 255);

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


void drawBrush (int x1, int y1, int x2, int y2, bool isMouseDown)
//Function for drawing a single pixel or calling the useStrokeWidth function when the left mouse button is held down.
{
    // Convert stroke and fill colors to 32-bit format
    uint32_t strokeColour32 = (strokeRed & 255) << 24 | (strokeGreen & 255) << 16 | (strokeBlue & 255) << 8 | (strokeAlpha & 255);

    if (isMouseDown) {
        if (strokeWidth == 1)
        {
            backgrounLayerPixels[x2 + y2 * screenWidth] = strokeColour32;
        }
        else
        {
            //if the stroke width is more than a single pixel draw a circle with the stroke width function
            useStrokeWidth(x2, y2);
        }
    }
}


void eraser (int x1, int y1, int x2, int y2, int isMouseDown)
//Function for erasing a single pixel or calling the useStrokeWidth function when the left mouse button is held down.
{
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


void drawLine (int x1, int y1, int x2, int y2, bool isMouseDown)
//Function for drawing a line using Eike's Bressingham's line algorithm. -- NOT MY OWN ALGO --
{
    // Convert stroke and fill colors to 32-bit format
    uint32_t strokeColour32 = (strokeRed & 255) << 24 | (strokeGreen & 255) << 16 | (strokeBlue & 255) << 8 | (strokeAlpha & 255);

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


void drawRect (int x1, int y1, int x2, int y2, bool isMouseDown)
// Function for drawing a rectangle - if the fillColour is not -1, the rectangle is filled with the fillColour first
{
    // Convert stroke and fill colors to 32-bit format
    uint32_t strokeColour32 = (strokeRed & 255) << 24 | (strokeGreen & 255) << 16 | (strokeBlue & 255) << 8 | (strokeAlpha & 255);
    uint32_t fillColour32 = (fillRed & 255) << 24 | (fillGreen & 255) << 16 | (fillBlue & 255) << 8 | (fillAlpha & 255);
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


void drawEllipse(int x1, int y1, int x2, int y2, bool isMouseDown)
/* Function for drawing an ellipse if the fillColour is not -1, the ellipse is filled with the fillColour first
If either right or left shift are held down when drawing the ellipse, a perfect circle is drawn.

Based upon midpoint ellipse algorithm from https://www.javatpoint.com/computer-graphics-midpoint-ellipse-algorithm */
{
    // Convert stroke and fill colors to 32-bit format
    uint32_t strokeColour32 = (strokeRed & 255) << 24 | (strokeGreen & 255) << 16 | (strokeBlue & 255) << 8 | (strokeAlpha & 255);
    uint32_t fillColour32 = (fillRed & 255) << 24 | (fillGreen & 255) << 16 | (fillBlue & 255) << 8 | (fillAlpha & 255);

    if (isMouseDown) {
        // TODO: Add a function to allow the user to draw a rectangle (or ellipse) using rubber banding when mouse button down
    } else {
        // Calculate ellipse parameters
        int yCentre = (y1 + y2) / 2;
        int xCentre = (x1 + x2) / 2;
        int xRadius = abs(x2 - x1) / 2;
        int yRadius = abs(y2 - y1) / 2;
        int radius = std::min(xRadius, yRadius);

        SDL_Event ellipseEvent;
        SDL_WaitEvent(&ellipseEvent);
        switch (ellipseEvent.type) {
            case SDL_KEYDOWN:
                switch (ellipseEvent.key.keysym.sym) {
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT:
                        xRadius = radius;
                        yRadius = radius;
                        break;
                }
                break;
        }

        // Fill the ellipse if fill color is specified
        if (fillRed != -1) {
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

            int decisionParameter = round(bSquared - (aSquared * yRadius) + (0.25 * aSquared));
            while (xDistance <= yDistance) {
                for (int xLoop = xCentre - xCurrent; xLoop <= xCentre + xCurrent; xLoop++) {
                    backgrounLayerPixels[xLoop + (yCentre + yCurrent) * screenWidth] = fillColour32;
                    backgrounLayerPixels[xLoop + (yCentre - yCurrent) * screenWidth] = fillColour32;
                }

                xCurrent++;
                xDistance += fourBSquared;

                if (decisionParameter < 0) {
                    decisionParameter += xDistance + twoBSquared;
                } else {
                    yCurrent--;
                    yDistance -= fourASquared;
                    decisionParameter += xDistance - yDistance + twoBSquared;
                }
            }

            decisionParameter = round(bSquared * (xCurrent + 0.5) * (xCurrent + 0.5) + aSquared * (yCurrent - 1) * (yCurrent - 1) - aSquared * bSquared);
            while (yCurrent >= 0) {
                for (int xLoop = xCentre - xCurrent; xLoop <= xCentre + xCurrent; xLoop++) {
                    backgrounLayerPixels[xLoop + (yCentre + yCurrent) * screenWidth] = fillColour32;
                    backgrounLayerPixels[xLoop + (yCentre - yCurrent) * screenWidth] = fillColour32;
                }

                yCurrent--;
                yDistance -= fourASquared;

                if (decisionParameter > 0) {
                    decisionParameter += aSquared - yDistance;
                } else {
                    xCurrent++;
                    xDistance += fourBSquared;
                    decisionParameter += aSquared - yDistance + xDistance;
                }
            }
        }

        // Draw the stroke of the ellipse if stroke color is specified
        if (strokeRed != -1) {
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

            int decisionParameter = round(bSquared - (aSquared * yRadius) + (0.25 * aSquared));
            while (xDistance <= yDistance) {
                if (strokeWidth == 1) {
                    backgrounLayerPixels[(xCentre + xCurrent) + (yCentre + yCurrent) * screenWidth] = strokeColour32;
                    backgrounLayerPixels[(xCentre - xCurrent) + (yCentre + yCurrent) * screenWidth] = strokeColour32;
                    backgrounLayerPixels[(xCentre + xCurrent) + (yCentre - yCurrent) * screenWidth] = strokeColour32;
                    backgrounLayerPixels[(xCentre - xCurrent) + (yCentre - yCurrent) * screenWidth] = strokeColour32;
                } else {
                    useStrokeWidth((xCentre + xCurrent), (yCentre + yCurrent));
                    useStrokeWidth((xCentre - xCurrent), (yCentre + yCurrent));
                    useStrokeWidth((xCentre + xCurrent), (yCentre - yCurrent));
                    useStrokeWidth((xCentre - xCurrent), (yCentre - yCurrent));
                }

                xCurrent++;
                xDistance += fourBSquared;

                if (decisionParameter < 0) {
                    decisionParameter += xDistance + twoBSquared;
                } else {
                    yCurrent--;
                    yDistance -= fourASquared;
                    decisionParameter += xDistance - yDistance + twoBSquared;
                }
            }

            decisionParameter = round(bSquared * (xCurrent + 0.5) * (xCurrent + 0.5) + aSquared * (yCurrent - 1) * (yCurrent - 1) - aSquared * bSquared);
            while (yCurrent >= 0) {
                if (strokeWidth == 1) {
                    backgrounLayerPixels[(xCentre + xCurrent) + (yCentre + yCurrent) * screenWidth] = strokeColour32;
                    backgrounLayerPixels[(xCentre - xCurrent) + (yCentre + yCurrent) * screenWidth] = strokeColour32;
                    backgrounLayerPixels[(xCentre + xCurrent) + (yCentre - yCurrent) * screenWidth] = strokeColour32;
                    backgrounLayerPixels[(xCentre - xCurrent) + (yCentre - yCurrent) * screenWidth] = strokeColour32;
                } else {
                    useStrokeWidth((xCentre + xCurrent), (yCentre + yCurrent));
                    useStrokeWidth((xCentre - xCurrent), (yCentre + yCurrent));
                    useStrokeWidth((xCentre + xCurrent), (yCentre - yCurrent));
                    useStrokeWidth((xCentre - xCurrent), (yCentre - yCurrent));
                }

                yCurrent--;
                yDistance -= fourASquared;

                if (decisionParameter > 0) {
                    decisionParameter += aSquared - yDistance;
                } else {
                    xCurrent++;
                    xDistance += fourBSquared;
                    decisionParameter += aSquared - yDistance + xDistance;
                }
            }
        }
    }
}


void bucketFill (int xCurrent, int yCurrent, int isMouseDown)
// Function for filling an area with the fillColour using 4-point approach outlined in Eike's lecture - may struggle with very large areas
{
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

bool loadImage(Uint32*& backgrounLayerPixels, SDL_Texture* backgroundLayer)
// Function opens a file dialog for the user to load an image
{
    const char * filters[5] = { "*.bmp", "*.png", "*.jpg", "*.jpeg", "*.gif" };
    char const * imageName = tinyfd_openFileDialog(
        "Open Image",  // Dialog title
        "",            // Default file path
        5,             // Number of filters
        filters,       // Filters
        NULL,          // Description for filters
        0              // Allow multiple selection
    );

    if (!imageName) {
        printf("No file was selected.\n");
        return false;
    }


    SDL_Surface* loadedImage = IMG_Load(imageName);
    if (loadedImage == nullptr) {
        printf("error: Could not read file correctly. ====> Please double check the file name and file location of the image you are trying to load and try again.\n Remember FILE NAMES ARE CASE SENSITIVE\n");
        return false;
    }

    SDL_Surface* convertedImageSurface = SDL_ConvertSurfaceFormat(loadedImage, SDL_PIXELFORMAT_RGBA8888, 0);
    if (convertedImageSurface == nullptr) {
        printf("error: Could not convert image surface.\n");
        return false;
    }

    backgrounLayerPixels = new Uint32[convertedImageSurface->w * convertedImageSurface->h];
    memcpy(backgrounLayerPixels, convertedImageSurface->pixels, convertedImageSurface->w * convertedImageSurface->h * sizeof(Uint32));
    SDL_UpdateTexture(backgroundLayer, NULL, backgrounLayerPixels, convertedImageSurface->w * sizeof(Uint32));

    SDL_FreeSurface(convertedImageSurface);
    printf("Image loaded\n");
    return true;
}

void handleGUIButtons(int x1, int y1)
// Function to handle the buttons on the GUI
{
    struct Button {
        int x1, y1, x2, y2;
        std::function<void()> onClick;

        bool contains(int x, int y) const {return x >= x1 && x < x2 && y >= y1 && y < y2;}
    };
    std::vector<Button> buttons = {
        // Tool selection buttons
        {0, 0, 50, 50, [](){ currentTool = BRUSH; }},
        {50, 0, 100, 50, [](){ currentTool = ERASER; }},
        {0, 50, 50, 100, [](){ currentTool = LINE; }},
        {50, 50, 100, 100, [](){ currentTool = RECTANGLE; }},
        {0, 100, 50, 150, [](){ currentTool = CIRCLE; }},
        {50, 100, 100, 150, [](){ currentTool = FILL; }},

        // Image load and save buttons
        {0, 170, 50, 210, [](){ /* Image load call goes here */ }},
        {50, 170, 100, 210, [](){ /* Image save call goes here */ }},

        // Stroke width buttons Y = 230 - 250
        {0, 230, 20, 250, [](){ strokeWidth = 1; }},
        {20, 230, 40, 250, [](){ strokeWidth = 3; }},
        {40, 230, 60, 250, [](){ strokeWidth = 5; }},
        {60, 230, 80, 250, [](){ strokeWidth = 7; }},
        {80, 230, 100, 250, [](){ strokeWidth = 9; }},

        // --- Stroke colour buttons ---
        // Stroke color buttons Y= 285 - 305
        {0, 285, 25, 305, [](){ strokeRed = -1; strokeGreen = -1; strokeBlue = -1; }},
        {25, 285, 50, 305, [](){ strokeRed = 255; strokeGreen = 255; strokeBlue = 255; }},
        {50, 285, 75, 305, [](){ strokeRed = 0; strokeGreen = 0; strokeBlue = 0; }},
        {75, 285, 100, 305, [](){ strokeRed = 255; strokeGreen = 0; strokeBlue = 0; }},
        // Stroke color buttons Y = 310 - 330
        {0, 310, 25, 330, [](){ strokeRed = 0; strokeGreen = 255; strokeBlue = 0; }},
        {25, 310, 50, 330, [](){ strokeRed = 0; strokeGreen = 0; strokeBlue = 255; }},
        {50, 310, 75, 330, [](){ strokeRed = 255; strokeGreen = 255; strokeBlue = 0; }},
        {75, 310, 100, 330, [](){ strokeRed = 255; strokeGreen = 0; strokeBlue = 255; }},
        // Stroke color buttons Y = 335 - 355
        {0, 335, 25, 355, [](){ strokeRed = 0; strokeGreen = 255; strokeBlue = 255; }},
        {25, 335, 50, 355, [](){ strokeRed = 255; strokeGreen = 255; strokeBlue = 255; }},
        {50, 335, 75, 355, [](){ strokeRed = 128; strokeGreen = 128; strokeBlue = 128; }},
        {75, 335, 100, 355, [](){ strokeRed = 128; strokeGreen = 0; strokeBlue = 0; }},
        // Stroke color buttons Y = 360 - 380
        {0, 360, 25, 380, [](){ strokeRed = 128; strokeGreen = 128; strokeBlue = 0; }},
        {25, 360, 50, 380, [](){ strokeRed = 0; strokeGreen = 128; strokeBlue = 0; }},
        {50, 360, 75, 380, [](){ strokeRed = 128; strokeGreen = 0; strokeBlue = 128; }},
        {75, 360, 100, 380, [](){ strokeRed = 0; strokeGreen = 128; strokeBlue = 128; }},

        // --- Fill colour buttons ---
        // Fill color buttons Y= 420 - 440
        {0, 420, 25, 440, [](){ fillRed = -1; fillGreen = -1; fillBlue = -1; }},
        {25, 420, 50, 440, [](){ fillRed = 255; fillGreen = 255; fillBlue = 255; }},
        {50, 420, 75, 440, [](){ fillRed = 0; fillGreen = 0; fillBlue = 0; }},
        {75, 420, 100, 440, [](){ fillRed = 255; fillGreen = 0; fillBlue = 0; }},
        // Fill color buttons Y = 445 - 465
        {0, 445, 25, 465, [](){ fillRed = 0; fillGreen = 255; fillBlue = 0; }},
        {25, 445, 50, 465, [](){ fillRed = 0; fillGreen = 0; fillBlue = 255; }},
        {50, 445, 75, 465, [](){ fillRed = 255; fillGreen = 255; fillBlue = 0; }},
        {75, 445, 100, 465, [](){ fillRed = 255; fillGreen = 0; fillBlue = 255; }},
        // Fill color buttons Y = 470 - 490
        {0, 470, 25, 490, [](){ fillRed = 0; fillGreen = 255; fillBlue = 255; }},
        {25, 470, 50, 490, [](){ fillRed = 255; fillGreen = 255; fillBlue = 255; }},
        {50, 470, 75, 490, [](){ fillRed = 128; fillGreen = 128; fillBlue = 128; }},
        {75, 470, 100, 490, [](){ fillRed = 128; fillGreen = 0; fillBlue = 0; }},
        // Fill color buttons Y = 495 - 515
        {0, 495, 25, 515, [](){ fillRed = 128; fillGreen = 128; fillBlue = 0; }},
        {25, 495, 50, 515, [](){ fillRed = 0; fillGreen = 128; fillBlue = 0; }},
        {50, 495, 75, 515, [](){ fillRed = 128; fillGreen = 0; fillBlue = 128; }},
        {75, 495, 100, 515, [](){ fillRed = 0; fillGreen = 128; fillBlue = 128; }},
    };

    for (const Button& button : buttons) {
        if (button.contains(x1, y1)) {
            button.onClick();
            break;
        }
    }
}


void handleToolAction(int x1, int y1, int x2, int y2, bool isMouseDown, bool drawing)
// Function to handle which tool to pass the variables based on the current tool selected
{
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


/* --------------------------------MAIN FUNCTION-------------------------------- */

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

    // Creating Blank Canvas
    backgrounLayerPixels = new Uint32[screenWidth * screenHeight];
    memset(backgrounLayerPixels, 255, screenWidth * screenHeight * sizeof(Uint32));
    SDL_UpdateTexture(backgroundLayer, NULL, backgrounLayerPixels, screenWidth * sizeof(Uint32));
    printf ("Blank Canvas Created\n");

    // Load GUI
    SDL_Surface *gui_image = IMG_Load("GUI.png");
    SDL_Texture *guiTexture = SDL_CreateTextureFromSurface(renderer, gui_image);
    SDL_FreeSurface(gui_image);
    SDL_Rect dstrect = { 0, 0, guiWidth, screenHeight };
    SDL_RenderCopy(renderer, guiTexture, NULL, &dstrect);
    //SDL_RenderPresent commits texture to video memory
    SDL_RenderPresent(renderer);



    /* --------------------------------MAIN PROGRAM LOOP-------------------------------- */
    SDL_Event event;
    bool quit = false;
    bool drawing = false;
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;

    while (!quit)
    {
        SDL_UpdateTexture(backgroundLayer, NULL, backgrounLayerPixels, screenWidth * sizeof(Uint32));
        SDL_WaitEvent(&event);

        /* --------------------------------EVENT HANDLER-------------------------------- */
        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:  // Check for a key press (hotkey functionality)
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
                        handleGUIButtons(x1, y1);
                    };
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