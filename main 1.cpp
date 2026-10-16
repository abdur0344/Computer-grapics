/*
 * ============================================================================
 * CAKELAB - Interactive 2D Cake Customization Game
 * Computer Graphics Project (AIUB)
 * All-in-one standalone file for Code::Blocks (FreeGLUT / OpenGL)
 *
 * ----------------------------------------------------------------------------
 * GROUP MEMBER CONTRIBUTIONS (ROLL-WISE):
 * ----------------------------------------------------------------------------
 * 1. Md. Minhaj Rowfun Rabbi Anik (23-54110-3):
 *    - Home / Welcome Screen (Stage 0)
 *    - Stage 1: Base Shape & Tier Customization
 *    - Global Quit Dialog Modal System
 *    - Main Event Loop & Window Management (PAGE 1, 6, 11, 12, 13)
 *
 * 2. Adiba Howlader (23-54309-3):
 *    - Stage 2: Flavor & Color Palette System
 *    - Physical Dimension Scaling Factors (Scale & Height)
 *    - 3D Extrusion Side Walls & Cream Filling Stripe (PAGE 2, 3, 10, 11)
 *
 * 3. Md. Tanvir Hasan (23-54796-3):
 *    - Stage 3: Toppings & Ingredients Placement
 *    - Top Surface UV Normalization & Multi-tier Surface Alignment
 *    - Double-Harmonic Sinusoidal Animations (Candle Flame & Gems) (PAGE 8, 9, 11)
 *
 * 4. Abdur Rahman (23-54954-3):
 *    - Stage 4: Full Cake Packaging & Enclosure Systems
 *    - Target Order Recipe Generator & 100-Point Scoring Algorithm
 *    - Stage 5: Finish Screen Summary & Evaluation (PAGE 4, 7, 10, 11)
 *
 * ----------------------------------------------------------------------------
 * PAGE INDEX FOR VIVA PRESENTATION:
 * ----------------------------------------------------------------------------
 * PAGE 1: Includes & Global Constants
 * PAGE 2: Data Structures (Color, Point, Ingredient, FlavorPalette, Design, TargetOrder)
 * PAGE 3: Global State & Flavor Palettes (8 Flavors with Color & Size Factors)
 * PAGE 4: Target Order Generator & Real-time Scoring System (100 Points Total)
 * PAGE 5: 2D Graphics Primitives (Lines, Triangles, Rectangles, Circles, Ellipses, Polygons)
 * PAGE 6: Text Rendering & UI Widgets (Buttons, Menu Items, Quit Dialog)
 * PAGE 7: Target Order Card UI Component (Real-time Checklist & Score Bar)
 * PAGE 8: Cake Outline Geometry Generation (Circle, Square, Triangle, Rectangle, Polygon)
 * PAGE 9: Topping Items Renderer (8 Animated Ingredients with Transformations)
 * PAGE 10: 3D Layered Cake & Packaging Renderer (Extrusion, Cream, Tier Stacking, Packaging)
 * PAGE 11: Stage Screens (Welcome, Shape, Flavor, Toppings, Packaging, Finish)
 * PAGE 12: GLUT Event Callbacks (Display, Reshape, Keyboard with Dual-Case, Timer)
 * PAGE 13: Main Entry Point (Window Setup, Projection, Loop Initialization)
 * ============================================================================
 */

// ============================================================================
// PAGE 1: INCLUDES & GLOBAL CONSTANTS
// Contribution: Md. Minhaj Rowfun Rabbi Anik (23-54110-3)
// ============================================================================
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cctype>

// Window and layout dimensions
const float WINDOW_WIDTH  = 1280.0f;
const float WINDOW_HEIGHT = 760.0f;
const float PANEL_WIDTH   = 320.0f;
const float HEADER_HEIGHT = 90.0f;  // Offsets title below navigation tabs
const float FOOTER_HEIGHT = 70.0f;
const float PI            = 3.14159265359f;

// ============================================================================
// PAGE 2: DATA STRUCTURES
// Contribution: Adiba Howlader (23-54309-3) & Abdur Rahman (23-54954-3)
// ============================================================================

// RGB Color structure [0.0 - 1.0]
struct Color {
    float r, g, b;
    Color(float r = 0, float g = 0, float b = 0) : r(r), g(g), b(b) {}
};

// 2D coordinate point
struct Point {
    float x, y;
    Point(float x = 0, float y = 0) : x(x), y(y) {}
};

// Placed topping on top cake surface (Normalized UV mapping)
struct Ingredient {
    int type;        // 1..8 (Berry, Biscuit, Peanut, Cherry, Fruit, Choc, Gem, Candle)
    float u, v;      // Normalized coordinates on top surface [-1..1]
    float scale;     // Random size variation
    float rotation;  // Random tilt angle in degrees
};

// Flavor configuration: 7 coordinated colors + physical dimension scaling
struct FlavorPalette {
    Color sponge;          // Outer sponge layer color
    Color darkSponge;      // Darker shading for side depth
    Color filling;         // Cream filling stripe color
    Color frosting;        // Top frosting color
    Color frostingShadow;  // Base shadow tone
    Color drip;            // Dripping cream accent
    Color accent;          // Outer frosting border
    float scaleFactor;     // Scale multiplier (changes cake physical width)
    float heightFactor;    // Height multiplier (changes cake tier thickness)
};

// Active user cake design state
struct Design {
    int shapeType;      // 0=Circle, 1=Square, 2=Triangle, 3=Rectangle, 4=Polygon
    int tierCount;      // 1..3 tiers stacked vertically
    int polygonSides;   // 5..8 sides for polygon base
    int flavorType;     // 0..7 flavor palette
    int packagingType;  // 0=None, 1=Rope Tie, 2=Gift Box, 3=Wrapping Sheet, 4=Ribbon & Tag
    std::vector<Ingredient> ingredients;

    Design() : shapeType(0), tierCount(1), polygonSides(6), flavorType(0), packagingType(0) {}
};

// Target recipe order generated randomly for the player
struct TargetOrder {
    int shapeType;
    int tierCount;
    int flavorType;
    int targetIngType1;
    int targetIngCount1;
    int targetIngType2;
    int targetIngCount2;
    int packagingType;
};

// ============================================================================
// PAGE 3: GLOBAL STATE & FLAVOR PALETTES
// Contribution: Adiba Howlader (23-54309-3)
// ============================================================================

int currentStage = 0;          // 0=Welcome, 1=Shape, 2=Flavor, 3=Toppings, 4=Packaging, 5=Finish
bool showQuitDialog = false;   // Modal quit confirmation dialog toggle (y/n)
Design design;
TargetOrder target;
float animClock = 0.0f;        // Continuous timer for flame flicker & bobbing

// 8 Flavor Palettes: Each with unique colors AND physical size factors
FlavorPalette flavors[8] = {
    // 0: Strawberry (Classic pink, standard size)
    {Color(0.98f, 0.72f, 0.76f), Color(0.88f, 0.52f, 0.58f), Color(0.95f, 0.35f, 0.45f),
     Color(1.00f, 0.85f, 0.88f), Color(0.85f, 0.60f, 0.65f), Color(0.92f, 0.25f, 0.38f),
     Color(1.00f, 0.95f, 0.96f), 1.00f, 1.00f},

    // 1: Green Tea / Matcha (Compact, delicate bakery size)
    {Color(0.68f, 0.82f, 0.62f), Color(0.48f, 0.65f, 0.42f), Color(0.35f, 0.55f, 0.30f),
     Color(0.82f, 0.94f, 0.78f), Color(0.55f, 0.72f, 0.50f), Color(0.38f, 0.60f, 0.32f),
     Color(0.92f, 0.98f, 0.90f), 0.94f, 0.92f},

    // 2: Black Forest (Tall layered sponge, larger physical width)
    {Color(0.38f, 0.24f, 0.16f), Color(0.24f, 0.14f, 0.08f), Color(0.88f, 0.15f, 0.25f),
     Color(0.96f, 0.94f, 0.92f), Color(0.75f, 0.70f, 0.68f), Color(0.25f, 0.12f, 0.08f),
     Color(0.92f, 0.20f, 0.30f), 1.08f, 1.10f},

    // 3: Red Velvet (Fluffy, slightly elevated tiers)
    {Color(0.72f, 0.18f, 0.22f), Color(0.52f, 0.10f, 0.14f), Color(0.96f, 0.94f, 0.90f),
     Color(0.98f, 0.96f, 0.92f), Color(0.80f, 0.75f, 0.70f), Color(0.68f, 0.12f, 0.16f),
     Color(0.90f, 0.20f, 0.25f), 1.04f, 1.05f},

    // 4: Mango (Wide, tropical fruit cake)
    {Color(1.00f, 0.78f, 0.28f), Color(0.88f, 0.62f, 0.15f), Color(1.00f, 0.55f, 0.10f),
     Color(1.00f, 0.92f, 0.60f), Color(0.85f, 0.70f, 0.35f), Color(1.00f, 0.60f, 0.05f),
     Color(1.00f, 0.98f, 0.85f), 1.06f, 0.98f},

    // 5: Rich Chocolate (Dense, largest & thickest cake)
    {Color(0.48f, 0.28f, 0.16f), Color(0.32f, 0.18f, 0.08f), Color(0.30f, 0.15f, 0.08f),
     Color(0.58f, 0.35f, 0.20f), Color(0.38f, 0.20f, 0.10f), Color(0.25f, 0.12f, 0.05f),
     Color(0.85f, 0.70f, 0.50f), 1.14f, 1.15f},

    // 6: Vanilla Bean (Neat, standard bakery profile)
    {Color(0.96f, 0.90f, 0.78f), Color(0.86f, 0.78f, 0.62f), Color(0.92f, 0.82f, 0.65f),
     Color(1.00f, 0.98f, 0.92f), Color(0.85f, 0.82f, 0.75f), Color(0.88f, 0.75f, 0.55f),
     Color(1.00f, 1.00f, 1.00f), 0.98f, 1.00f},

    // 7: Butterscotch (Caramel fudge, hearty medium-large cake)
    {Color(0.94f, 0.70f, 0.38f), Color(0.78f, 0.52f, 0.22f), Color(0.85f, 0.45f, 0.15f),
     Color(0.98f, 0.86f, 0.62f), Color(0.80f, 0.65f, 0.40f), Color(0.82f, 0.48f, 0.12f),
     Color(1.00f, 0.94f, 0.78f), 1.07f, 1.06f}
};

// UI Display Names
const char* shapeNames[5]      = {"Circle", "Square", "Triangle", "Rectangle", "Polygon"};
const char* flavorNames[8]     = {"Strawberry", "Green Tea", "Black Forest", "Red Velvet",
                                  "Mango", "Chocolate", "Vanilla", "Butterscotch"};
const char* packagingNames[5]  = {"None", "Rope Tie", "Gift Box", "Wrapping Sheet", "Ribbon & Tag"};
const char* ingredientNames[8] = {"Berry", "Biscuit", "Peanut", "Cherry",
                                  "Fruit Slice", "Chocolate", "Candy Gem", "Candle"};

// ============================================================================
// PAGE 4: TARGET ORDER GENERATOR & SCORING SYSTEM
// Contribution: Abdur Rahman (23-54954-3)
// ============================================================================

// Initialize a randomized goal recipe
void initTargetOrder() {
    target.shapeType       = rand() % 5;
    target.tierCount       = 1 + rand() % 3;
    target.flavorType      = rand() % 8;
    target.targetIngType1  = 1 + rand() % 8;
    target.targetIngCount1 = 2 + rand() % 4;
    target.targetIngType2  = 1 + rand() % 8;
    while (target.targetIngType2 == target.targetIngType1) {
        target.targetIngType2 = 1 + rand() % 8;
    }
    target.targetIngCount2 = 1 + rand() % 3;
    target.packagingType   = 1 + rand() % 4;
}

// Calculate total accuracy score out of 100 points
int calculateScore() {
    int score = 0;

    // 1. Shape Match: 20 points
    if (design.shapeType == target.shapeType) score += 20;

    // 2. Tier Count Match: 20 points
    if (design.tierCount == target.tierCount) score += 20;

    // 3. Flavor Palette Match: 20 points
    if (design.flavorType == target.flavorType) score += 20;

    // 4. Packaging Match: 20 points
    if (design.packagingType == target.packagingType) score += 20;

    // 5. Ingredient Match: Up to 20 points (10 pts per required topping type)
    int count1 = 0, count2 = 0;
    for (const auto& ing : design.ingredients) {
        if (ing.type == target.targetIngType1) count1++;
        if (ing.type == target.targetIngType2) count2++;
    }

    if (count1 >= target.targetIngCount1) score += 10;
    else if (count1 > 0) score += (10 * count1) / target.targetIngCount1;

    if (count2 >= target.targetIngCount2) score += 10;
    else if (count2 > 0) score += (10 * count2) / target.targetIngCount2;

    return score;
}

// ============================================================================
// PAGE 5: 2D GRAPHICS PRIMITIVES
// Contribution: All Group Members
// ============================================================================

void setColor(const Color& c, float alpha = 1.0f) {
    glColor3f(c.r, c.g, c.b);
}

// Line scan conversion using GL_LINES
void drawLine(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

// Triangle rendering (filled or wireframe)
void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, bool filled = true) {
    if (filled) glBegin(GL_TRIANGLES);
    else glBegin(GL_LINE_LOOP);

    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

// Midpoint circle approximation using triangle fans
void drawCircle(float cx, float cy, float radius, bool filled = true) {
    if (filled) glBegin(GL_TRIANGLE_FAN);
    else glBegin(GL_LINE_LOOP);

    if (filled) glVertex2f(cx, cy);
    for (int i = 0; i <= 360; i += 6) {
        float angle = i * PI / 180.0f;
        glVertex2f(cx + radius * cosf(angle), cy + radius * sinf(angle));
    }
    glEnd();
}

// Midpoint ellipse approximation for isometric top surfaces
void drawEllipse(float cx, float cy, float rx, float ry, bool filled = true) {
    if (filled) glBegin(GL_TRIANGLE_FAN);
    else glBegin(GL_LINE_LOOP);

    if (filled) glVertex2f(cx, cy);
    for (int i = 0; i <= 360; i += 6) {
        float angle = i * PI / 180.0f;
        glVertex2f(cx + rx * cosf(angle), cy + ry * sinf(angle));
    }
    glEnd();
}

// Quadrilateral rectangle rendering
void drawRect(float x1, float y1, float x2, float y2, bool filled = true) {
    if (filled) glBegin(GL_QUADS);
    else glBegin(GL_LINE_LOOP);

    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

// Regular polygon generation
void drawPolygon(float cx, float cy, float radius, int sides, bool filled = true) {
    if (filled) glBegin(GL_TRIANGLE_FAN);
    else glBegin(GL_LINE_LOOP);

    if (filled) glVertex2f(cx, cy);
    for (int i = 0; i <= sides; i++) {
        float angle = (i * 2.0f * PI) / sides;
        glVertex2f(cx + radius * cosf(angle), cy + radius * sinf(angle));
    }
    glEnd();
}

// ============================================================================
// PAGE 6: TEXT RENDERING & UI WIDGETS
// Contribution: Md. Minhaj Rowfun Rabbi Anik (23-54110-3)
// ============================================================================

void drawText(float x, float y, const char* text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (const char* p = text; *p; p++) {
        glutBitmapCharacter(font, *p);
    }
}

void drawSmallText(float x, float y, const char* text) {
    drawText(x, y, text, GLUT_BITMAP_HELVETICA_12);
}

void drawLargeText(float x, float y, const char* text) {
    drawText(x, y, text, GLUT_BITMAP_TIMES_ROMAN_24);
}

// Interactive button with labeled keybinding after the word
void drawButton(float x, float y, float w, float h, const char* label, bool active = false) {
    if (active) {
        setColor(Color(0.20f, 0.55f, 0.90f));
    } else {
        setColor(Color(0.86f, 0.88f, 0.92f));
    }
    drawRect(x, y, x + w, y + h, true);

    // Border stroke
    if (active) setColor(Color(0.10f, 0.35f, 0.70f));
    else setColor(Color(0.65f, 0.70f, 0.78f));
    glLineWidth(2.0f);
    drawRect(x, y, x + w, y + h, false);
    glLineWidth(1.0f);

    // Label text
    if (active) setColor(Color(1.0f, 1.0f, 1.0f));
    else setColor(Color(0.15f, 0.18f, 0.25f));

    int len = strlen(label);
    float textX = x + (w - len * 7.5f) / 2.0f;
    drawSmallText(textX, y + h / 2.0f - 4.0f, label);
}

// Sidebar list selection item
void drawMenuItem(float x, float y, float w, const char* label, bool selected = false) {
    if (selected) {
        setColor(Color(0.25f, 0.55f, 0.95f));
        drawRect(x - 5, y - 16, x + w, y + 8, true);
        setColor(Color(1.0f, 1.0f, 1.0f));
    } else {
        setColor(Color(0.15f, 0.15f, 0.20f));
    }
    drawSmallText(x, y - 4, label);
}

// Confirmation Dialog on pressing 'q'
void drawQuitDialog() {
    // Dim background overlay
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.08f, 0.10f, 0.18f, 0.75f);
    drawRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, true);
    glDisable(GL_BLEND);

    // Dialog Box Window
    float dw = 380.0f, dh = 180.0f;
    float dx = (WINDOW_WIDTH - dw) / 2.0f;
    float dy = (WINDOW_HEIGHT - dh) / 2.0f;

    setColor(Color(1.0f, 1.0f, 1.0f));
    drawRect(dx, dy, dx + dw, dy + dh, true);

    setColor(Color(0.85f, 0.25f, 0.25f));
    drawRect(dx, dy + dh - 36.0f, dx + dw, dy + dh, true);

    setColor(Color(1.0f, 1.0f, 1.0f));
    drawText(dx + 20.0f, dy + dh - 24.0f, "QUIT CONFIRMATION");

    setColor(Color(0.60f, 0.65f, 0.75f));
    glLineWidth(2.0f);
    drawRect(dx, dy, dx + dw, dy + dh, false);
    glLineWidth(1.0f);

    setColor(Color(0.20f, 0.25f, 0.35f));
    drawText(dx + 35.0f, dy + 95.0f, "Are you sure you want to quit?");

    // Buttons
    drawButton(dx + 50.0f, dy + 25.0f, 120.0f, 40.0f, "Yes (y)", true);
    drawButton(dx + 210.0f, dy + 25.0f, 120.0f, 40.0f, "No (n)", false);
}

// ============================================================================
// PAGE 7: TARGET ORDER CARD UI COMPONENT
// Contribution: Abdur Rahman (23-54954-3)
// ============================================================================

void drawTargetCard() {
    float cardX = WINDOW_WIDTH - 270.0f;
    float cardY = WINDOW_HEIGHT - HEADER_HEIGHT - 215.0f;
    float cardW = 250.0f;
    float cardH = 205.0f;

    // Card background panel
    setColor(Color(0.98f, 0.98f, 1.00f));
    drawRect(cardX, cardY, cardX + cardW, cardY + cardH, true);

    // Header banner with live score
    int score = calculateScore();
    if (score == 100) setColor(Color(0.15f, 0.65f, 0.25f));
    else setColor(Color(0.85f, 0.35f, 0.15f));
    drawRect(cardX, cardY + cardH - 32.0f, cardX + cardW, cardY + cardH, true);

    setColor(Color(1.0f, 1.0f, 1.0f));
    char scoreStr[60];
    sprintf(scoreStr, "TARGET ORDER [Score: %d]", score);
    drawSmallText(cardX + 12.0f, cardY + cardH - 22.0f, scoreStr);

    // Card border
    setColor(Color(0.70f, 0.75f, 0.85f));
    glLineWidth(2.0f);
    drawRect(cardX, cardY, cardX + cardW, cardY + cardH, false);
    glLineWidth(1.0f);

    float textY = cardY + cardH - 54.0f;
    char line[80];

    // 1. Shape checklist
    bool shapeMatch = (design.shapeType == target.shapeType);
    setColor(shapeMatch ? Color(0.1f, 0.6f, 0.2f) : Color(0.3f, 0.3f, 0.3f));
    sprintf(line, "[%c] Shape: %s", shapeMatch ? 'X' : ' ', shapeNames[target.shapeType]);
    drawSmallText(cardX + 15.0f, textY, line);
    textY -= 22.0f;

    // 2. Tiers checklist
    bool tierMatch = (design.tierCount == target.tierCount);
    setColor(tierMatch ? Color(0.1f, 0.6f, 0.2f) : Color(0.3f, 0.3f, 0.3f));
    sprintf(line, "[%c] Tiers: %d Tier%s", tierMatch ? 'X' : ' ', target.tierCount, target.tierCount > 1 ? "s" : "");
    drawSmallText(cardX + 15.0f, textY, line);
    textY -= 22.0f;

    // 3. Flavor checklist
    bool flavorMatch = (design.flavorType == target.flavorType);
    setColor(flavorMatch ? Color(0.1f, 0.6f, 0.2f) : Color(0.3f, 0.3f, 0.3f));
    sprintf(line, "[%c] Flavor: %s", flavorMatch ? 'X' : ' ', flavorNames[target.flavorType]);
    drawSmallText(cardX + 15.0f, textY, line);
    textY -= 22.0f;

    // 4. Toppings checklist
    int c1 = 0, c2 = 0;
    for (const auto& ing : design.ingredients) {
        if (ing.type == target.targetIngType1) c1++;
        if (ing.type == target.targetIngType2) c2++;
    }
    bool topMatch = (c1 >= target.targetIngCount1 && c2 >= target.targetIngCount2);
    setColor(topMatch ? Color(0.1f, 0.6f, 0.2f) : Color(0.3f, 0.3f, 0.3f));
    sprintf(line, "[%c] Tops: %dx %s, %dx %s",
            topMatch ? 'X' : ' ',
            target.targetIngCount1, ingredientNames[target.targetIngType1 - 1],
            target.targetIngCount2, ingredientNames[target.targetIngType2 - 1]);
    drawSmallText(cardX + 15.0f, textY, line);
    textY -= 22.0f;

    // 5. Packaging checklist
    bool packMatch = (design.packagingType == target.packagingType);
    setColor(packMatch ? Color(0.1f, 0.6f, 0.2f) : Color(0.3f, 0.3f, 0.3f));
    sprintf(line, "[%c] Pack: %s", packMatch ? 'X' : ' ', packagingNames[target.packagingType]);
    drawSmallText(cardX + 15.0f, textY, line);
}

// ============================================================================
// PAGE 8: CAKE OUTLINE GEOMETRY GENERATION
// Contribution: Md. Tanvir Hasan (23-54796-3)
// ============================================================================

// Generates 2D parametric boundary points squashed by isometric projection
std::vector<Point> generateCakeOutline(int shapeType, float scale, int polygonSides = 6) {
    std::vector<Point> outline;
    float squash = 0.38f; // Isometric tilt projection factor

    switch(shapeType) {
        case 0: { // Circle (360 degrees, step 6)
            for (int i = 0; i < 360; i += 6) {
                float angle = i * PI / 180.0f;
                outline.push_back(Point(cosf(angle) * scale, sinf(angle) * scale * squash));
            }
            break;
        }
        case 1: { // Square (Rotated 26 deg for 3D isometric view)
            float s = scale * 0.95f;
            float angle = 26.0f * PI / 180.0f;
            float corners[4][2] = {{-s, -s}, {s, -s}, {s, s}, {-s, s}};
            for (int i = 0; i < 4; i++) {
                float x = corners[i][0] * cosf(angle) - corners[i][1] * sinf(angle);
                float y = (corners[i][0] * sinf(angle) + corners[i][1] * cosf(angle)) * squash;
                outline.push_back(Point(x, y));
            }
            break;
        }
        case 2: { // Triangle
            float h = scale * 1.15f;
            outline.push_back(Point(0, h * 0.9f * squash));
            outline.push_back(Point(-h, -h * 0.55f * squash));
            outline.push_back(Point(h, -h * 0.55f * squash));
            break;
        }
        case 3: { // Rectangle (Rotated 16 deg)
            float w = scale * 1.35f, h = scale * 0.75f;
            float angle = 16.0f * PI / 180.0f;
            float corners[4][2] = {{-w, -h}, {w, -h}, {w, h}, {-w, h}};
            for (int i = 0; i < 4; i++) {
                float x = corners[i][0] * cosf(angle) - corners[i][1] * sinf(angle);
                float y = (corners[i][0] * sinf(angle) + corners[i][1] * cosf(angle)) * squash;
                outline.push_back(Point(x, y));
            }
            break;
        }
        case 4: { // Polygon (N-sides)
            for (int i = 0; i < polygonSides; i++) {
                float angle = (i * 2.0f * PI) / polygonSides;
                outline.push_back(Point(cosf(angle) * scale, sinf(angle) * scale * squash));
            }
            break;
        }
    }
    return outline;
}

// ============================================================================
// PAGE 9: TOPPING ITEMS RENDERER
// Contribution: Md. Tanvir Hasan (23-54796-3)
// ============================================================================

// Scan converts 8 distinct toppings with local translation, rotation, and scale
void drawToppingItem(int type, float x, float y, float scale, float rot) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(rot, 0.0f, 0.0f, 1.0f);
    glScalef(scale, scale, 1.0f);

    switch(type) {
        case 1: { // Berry (Strawberry / Raspberry)
            setColor(Color(0.85f, 0.12f, 0.18f));
            drawCircle(0, 0, 10.0f);
            setColor(Color(1.0f, 0.5f, 0.5f));
            drawCircle(-3.0f, 3.0f, 3.0f);
            setColor(Color(0.2f, 0.6f, 0.1f));
            drawTriangle(0, 8.0f, -4.0f, 13.0f, 4.0f, 13.0f);
            break;
        }
        case 2: { // Biscuit (Cookie with pattern dots)
            setColor(Color(0.86f, 0.68f, 0.40f));
            drawCircle(0, 0, 12.0f);
            setColor(Color(0.65f, 0.45f, 0.22f));
            glLineWidth(2.0f);
            drawCircle(0, 0, 12.0f, false);
            glLineWidth(1.0f);
            for (int i = 0; i < 6; i++) {
                float a = i * 60.0f * PI / 180.0f;
                drawCircle(cosf(a) * 6.0f, sinf(a) * 6.0f, 1.5f);
            }
            break;
        }
        case 3: { // Peanut (Dual ellipse shape)
            setColor(Color(0.88f, 0.70f, 0.32f));
            drawEllipse(-4.0f, 0, 6.0f, 9.0f);
            drawEllipse(4.0f, 0, 6.0f, 9.0f);
            setColor(Color(0.70f, 0.50f, 0.18f));
            glLineWidth(1.5f);
            drawEllipse(-4.0f, 0, 6.0f, 9.0f, false);
            drawEllipse(4.0f, 0, 6.0f, 9.0f, false);
            glLineWidth(1.0f);
            break;
        }
        case 4: { // Cherry with curved stem
            setColor(Color(0.92f, 0.08f, 0.12f));
            drawCircle(0, 0, 9.0f);
            setColor(Color(1.0f, 0.6f, 0.6f));
            drawCircle(-3.0f, 3.0f, 2.5f);
            setColor(Color(0.25f, 0.50f, 0.15f));
            glLineWidth(2.5f);
            glBegin(GL_LINE_STRIP);
            glVertex2f(0, 8.0f);
            glVertex2f(4.0f, 16.0f);
            glVertex2f(10.0f, 20.0f);
            glEnd();
            glLineWidth(1.0f);
            break;
        }
        case 5: { // Fruit Slice (Citrus orange segment)
            setColor(Color(1.00f, 0.55f, 0.05f));
            drawCircle(0, 0, 12.0f);
            setColor(Color(1.00f, 0.95f, 0.70f));
            drawCircle(0, 0, 9.0f);
            setColor(Color(1.00f, 0.60f, 0.10f));
            for (int i = 0; i < 6; i++) {
                float a = i * 60.0f * PI / 180.0f;
                glBegin(GL_TRIANGLES);
                glVertex2f(0, 0);
                glVertex2f(cosf(a) * 8.5f, sinf(a) * 8.5f);
                glVertex2f(cosf(a + 50.0f * PI / 180.0f) * 8.5f, sinf(a + 50.0f * PI / 180.0f) * 8.5f);
                glEnd();
            }
            break;
        }
        case 6: { // Chocolate Chunk
            setColor(Color(0.38f, 0.20f, 0.10f));
            drawRect(-8.0f, -6.0f, 8.0f, 6.0f);
            setColor(Color(0.55f, 0.32f, 0.18f));
            drawRect(-6.0f, -4.0f, 6.0f, 4.0f);
            break;
        }
        case 7: { // Candy Gem (Animated sparkle)
            float sparkle = sinf(animClock * 4.0f) * 0.15f;
            setColor(Color(0.10f, 0.85f + sparkle, 0.95f));
            drawPolygon(0, 0, 10.0f, 6);
            setColor(Color(1.0f, 1.0f, 1.0f));
            drawPolygon(0, 2.0f, 4.0f, 6);
            break;
        }
        case 8: { // Lit Candle (Animated flickering flame)
            setColor(Color(0.95f, 0.92f, 0.85f));
            drawRect(-3.0f, 0, 3.0f, 22.0f);
            setColor(Color(0.90f, 0.25f, 0.25f));
            drawRect(-3.0f, 6.0f, 3.0f, 9.0f);
            drawRect(-3.0f, 14.0f, 3.0f, 17.0f);
            setColor(Color(0.2f, 0.2f, 0.2f));
            drawLine(0, 22.0f, 0, 25.0f);

            // Double sinusoidal harmonic flame flicker
            float flicker = sinf(animClock * 8.0f) * 2.0f + sinf(animClock * 19.0f) * 1.0f;
            setColor(Color(1.0f, 0.6f, 0.05f));
            drawEllipse(flicker * 0.5f, 31.0f + flicker * 0.3f, 5.0f, 8.0f);
            setColor(Color(1.0f, 0.95f, 0.3f));
            drawEllipse(flicker * 0.5f, 30.0f + flicker * 0.3f, 2.5f, 4.5f);
            break;
        }
    }
    glPopMatrix();
}

// ============================================================================
// PAGE 10: 3D LAYERED CAKE & PACKAGING RENDERER
// Contribution: Adiba Howlader (Extrusion) & Abdur Rahman (Packaging)
// ============================================================================

void drawCake(const Design& d, float cx, float cy, float baseScale) {
    FlavorPalette& flavor = flavors[d.flavorType];

    // Apply flavor scaleFactor and heightFactor to physically resize cake
    float effectiveScale      = baseScale * flavor.scaleFactor;
    float effectiveTierHeight = 50.0f * flavor.heightFactor;
    float bodyDepth           = 42.0f * flavor.heightFactor;

    // 1. Draw tiers stacked from Bottom (tier 0) to Top (tier d.tierCount - 1)
    for (int tier = 0; tier < d.tierCount; tier++) {
        float scale = effectiveScale * (1.0f - tier * 0.22f);
        float tierY = cy + tier * effectiveTierHeight;

        auto outline = generateCakeOutline(d.shapeType, scale, d.polygonSides);
        int n = outline.size();

        // 1A. Draw Side Body (Extrusion downward for 3D depth)
        for (int i = 0; i < n; i++) {
            Point p1 = outline[i];
            Point p2 = outline[(i + 1) % n];

            // Only render front-facing silhouette faces
            if (p1.y <= 2.0f || p2.y <= 2.0f) {
                // Sponge side wall
                setColor(tier == 0 ? flavor.sponge : flavor.darkSponge);
                glBegin(GL_QUADS);
                glVertex2f(cx + p1.x, tierY + p1.y);
                glVertex2f(cx + p2.x, tierY + p2.y);
                glVertex2f(cx + p2.x, tierY + p2.y - bodyDepth);
                glVertex2f(cx + p1.x, tierY + p1.y - bodyDepth);
                glEnd();

                // Cream filling stripe in middle of tier
                setColor(flavor.filling);
                glBegin(GL_QUADS);
                glVertex2f(cx + p1.x, tierY + p1.y - bodyDepth * 0.45f);
                glVertex2f(cx + p2.x, tierY + p2.y - bodyDepth * 0.45f);
                glVertex2f(cx + p2.x, tierY + p2.y - bodyDepth * 0.65f);
                glVertex2f(cx + p1.x, tierY + p1.y - bodyDepth * 0.65f);
                glEnd();

                // Dripping glaze along top rim
                setColor(flavor.drip);
                glBegin(GL_QUADS);
                glVertex2f(cx + p1.x, tierY + p1.y);
                glVertex2f(cx + p2.x, tierY + p2.y);
                glVertex2f(cx + p2.x, tierY + p2.y - 12.0f);
                glVertex2f(cx + p1.x, tierY + p1.y - 12.0f);
                glEnd();

                // Base shadow crease line
                setColor(flavor.frostingShadow);
                glBegin(GL_LINES);
                glVertex2f(cx + p1.x, tierY + p1.y - bodyDepth);
                glVertex2f(cx + p2.x, tierY + p2.y - bodyDepth);
                glEnd();
            }
        }

        // 1B. Draw Frosted Top Surface of this tier
        setColor(flavor.frosting);
        glBegin(GL_POLYGON);
        for (const auto& p : outline) {
            glVertex2f(cx + p.x, tierY + p.y);
        }
        glEnd();

        // Accent rim outline
        setColor(flavor.accent);
        glLineWidth(2.5f);
        glBegin(GL_LINE_LOOP);
        for (const auto& p : outline) {
            glVertex2f(cx + p.x, tierY + p.y);
        }
        glEnd();
        glLineWidth(1.0f);
    }

    // 2. Draw TOPPINGS on the TOPMOST tier's surface (Works on 1st, 2nd, or 3rd tier)
    int topTierIdx     = d.tierCount - 1;
    float topTierScale = effectiveScale * (1.0f - topTierIdx * 0.22f);
    float topTierY     = cy + topTierIdx * effectiveTierHeight;
    float squashFactor = 0.38f;

    for (const auto& ing : d.ingredients) {
        if (ing.type >= 1 && ing.type <= 8) {
            // Map normalized UV coordinates onto the top tier isometric surface
            float ix = cx + ing.u * (topTierScale * 0.85f);
            float iy = topTierY + ing.v * (topTierScale * 0.85f * squashFactor);

            float sway = ing.rotation + sinf(animClock * 2.0f + ing.u * 5.0f) * 4.0f;
            drawToppingItem(ing.type, ix, iy, ing.scale, sway);
        }
    }

    // 3. Draw PACKAGING (Covers the entire cake from bottom platter to top lid)
    if (d.packagingType > 0) {
        float bottomY = cy - bodyDepth - 16.0f;
        float topY    = cy + (d.tierCount - 1) * effectiveTierHeight + 42.0f;
        float boxW    = effectiveScale * 1.30f;

        switch(d.packagingType) {
            case 1: { // 1. Rope Tie & Bakery Kraft Box (Encloses full cake)
                // Bottom cake board
                setColor(Color(0.70f, 0.52f, 0.32f));
                drawRect(cx - boxW - 12.0f, bottomY - 10.0f, cx + boxW + 12.0f, bottomY + 2.0f, true);
                setColor(Color(0.50f, 0.35f, 0.18f));
                drawRect(cx - boxW - 12.0f, bottomY - 10.0f, cx + boxW + 12.0f, bottomY + 2.0f, false);

                // Solid Bakery Kraft Box Enclosure
                setColor(Color(0.92f, 0.82f, 0.68f));
                drawRect(cx - boxW, bottomY, cx + boxW, topY, true);

                // Kraft box frame border
                setColor(Color(0.68f, 0.48f, 0.28f));
                glLineWidth(3.0f);
                drawRect(cx - boxW, bottomY, cx + boxW, topY, false);
                glLineWidth(1.0f);

                // Ropes tied vertically across whole box
                setColor(Color(0.52f, 0.34f, 0.16f));
                glLineWidth(3.0f);
                drawLine(cx - 30.0f, bottomY - 10.0f, cx - 30.0f, topY);
                drawLine(cx + 30.0f, bottomY - 10.0f, cx + 30.0f, topY);
                // Ropes tied horizontally
                drawLine(cx - boxW, (bottomY + topY) * 0.5f, cx + boxW, (bottomY + topY) * 0.5f);
                glLineWidth(1.0f);

                // Top Twine Knot & Loops on box top
                setColor(Color(0.48f, 0.30f, 0.12f));
                drawCircle(cx, topY, 6.0f, true);
                glLineWidth(2.5f);
                drawEllipse(cx - 14.0f, topY + 8.0f, 12.0f, 7.0f, false);
                drawEllipse(cx + 14.0f, topY + 8.0f, 12.0f, 7.0f, false);
                glLineWidth(1.0f);
                break;
            }
            case 2: { // 2. Deluxe Royal Gift Box (Encloses full cake)
                // Golden base platform
                setColor(Color(0.85f, 0.70f, 0.20f));
                drawRect(cx - boxW - 14.0f, bottomY - 12.0f, cx + boxW + 14.0f, bottomY, true);

                // Solid Royal Crimson Box body cover
                setColor(Color(0.85f, 0.15f, 0.22f));
                drawRect(cx - boxW, bottomY, cx + boxW, topY, true);

                // Outer gift box border
                setColor(Color(0.75f, 0.10f, 0.18f));
                glLineWidth(3.5f);
                drawRect(cx - boxW, bottomY, cx + boxW, topY, false);
                glLineWidth(1.0f);

                // Golden Satin Ribbon Bands spanning the full height & width
                setColor(Color(1.00f, 0.84f, 0.10f));
                drawRect(cx - 12.0f, bottomY - 12.0f, cx + 12.0f, topY, true);
                drawRect(cx - boxW, (bottomY + topY) * 0.48f - 10.0f, cx + boxW, (bottomY + topY) * 0.48f + 10.0f, true);

                // Ribbon borders
                setColor(Color(0.80f, 0.62f, 0.05f));
                glLineWidth(1.5f);
                drawRect(cx - 12.0f, bottomY - 12.0f, cx + 12.0f, topY, false);
                drawRect(cx - boxW, (bottomY + topY) * 0.48f - 10.0f, cx + boxW, (bottomY + topY) * 0.48f + 10.0f, false);
                glLineWidth(1.0f);

                // Grand Rosette Bow on top
                setColor(Color(1.00f, 0.88f, 0.20f));
                drawCircle(cx - 18.0f, topY + 14.0f, 15.0f, true);
                drawCircle(cx + 18.0f, topY + 14.0f, 15.0f, true);
                drawCircle(cx, topY + 22.0f, 12.0f, true);
                setColor(Color(0.88f, 0.70f, 0.10f));
                drawCircle(cx, topY + 10.0f, 8.0f, true);
                break;
            }
            case 3: { // 3. Solid Pastel Celebration Box (Encloses full cake)
                // Bottom base drum
                setColor(Color(0.85f, 0.85f, 0.90f));
                drawRect(cx - boxW - 10.0f, bottomY - 10.0f, cx + boxW + 10.0f, bottomY, true);
                setColor(Color(0.65f, 0.65f, 0.70f));
                drawRect(cx - boxW - 10.0f, bottomY - 10.0f, cx + boxW + 10.0f, bottomY, false);

                // Solid pastel celebration box body
                setColor(Color(0.82f, 0.75f, 0.90f));
                drawRect(cx - boxW, bottomY, cx + boxW, topY, true);

                // Festive lid border
                setColor(Color(0.68f, 0.60f, 0.80f));
                drawRect(cx - boxW - 2.0f, topY, cx + boxW + 2.0f, topY + 15.0f, true);

                // Box frame outline
                setColor(Color(0.55f, 0.45f, 0.70f));
                glLineWidth(2.5f);
                drawRect(cx - boxW, bottomY, cx + boxW, topY + 15.0f, false);
                glLineWidth(1.0f);

                // Confetti patterns on box
                setColor(Color(1.0f, 0.95f, 0.75f));
                for(int i=0; i<5; i++) drawCircle(cx - boxW + 25.0f + i*50.0f, bottomY + 30.0f + i*15.0f, 4.0f);
                setColor(Color(0.95f, 0.65f, 0.75f));
                for(int i=0; i<4; i++) drawCircle(cx - boxW + 40.0f + i*60.0f, bottomY + 60.0f + i*10.0f, 3.0f);
                break;
            }
            case 4: { // 4. Solid Boutique Gift Box with Ribbon & Tag (Encloses full cake)
                // Solid base tray
                setColor(Color(0.90f, 0.78f, 0.85f));
                drawRect(cx - boxW - 10.0f, bottomY - 10.0f, cx + boxW + 10.0f, bottomY, true);
                setColor(Color(0.70f, 0.58f, 0.65f));
                drawRect(cx - boxW - 10.0f, bottomY - 10.0f, cx + boxW + 10.0f, bottomY, false);

                // Solid Pink Boutique Box body
                setColor(Color(0.95f, 0.88f, 0.94f));
                drawRect(cx - boxW, bottomY, cx + boxW, topY, true);

                // Box frame outline
                setColor(Color(0.82f, 0.55f, 0.75f));
                glLineWidth(3.0f);
                drawRect(cx - boxW, bottomY, cx + boxW, topY, false);
                glLineWidth(1.0f);

                // Top Carrier Handle (U-shape)
                setColor(Color(0.75f, 0.45f, 0.68f));
                glLineWidth(5.0f);
                glBegin(GL_LINE_STRIP);
                glVertex2f(cx - 45.0f, topY);
                glVertex2f(cx - 45.0f, topY + 30.0f);
                glVertex2f(cx + 45.0f, topY + 30.0f);
                glVertex2f(cx + 45.0f, topY);
                glEnd();
                glLineWidth(1.0f);

                // Diagonal Pink Ribbon Sash across the full box
                setColor(Color(0.92f, 0.30f, 0.55f));
                glBegin(GL_QUADS);
                glVertex2f(cx - boxW, topY - 20.0f);
                glVertex2f(cx - boxW, topY);
                glVertex2f(cx + boxW, bottomY + 20.0f);
                glVertex2f(cx + boxW, bottomY);
                glEnd();

                // Hanging Gift Tag Card
                float tagX = cx + boxW * 0.35f;
                float tagY = (bottomY + topY) * 0.5f;
                setColor(Color(0.4f, 0.4f, 0.4f));
                drawLine(tagX + 25.0f, tagY + 20.0f, tagX + 25.0f, tagY);

                setColor(Color(1.00f, 0.98f, 0.92f));
                drawRect(tagX, tagY - 35.0f, tagX + 60.0f, tagY, true);
                setColor(Color(0.85f, 0.30f, 0.55f));
                glLineWidth(1.5f);
                drawRect(tagX, tagY - 35.0f, tagX + 60.0f, tagY, false);
                glLineWidth(1.0f);
                drawSmallText(tagX + 8.0f, tagY - 18.0f, "CAKE");
                drawSmallText(tagX + 8.0f, tagY - 30.0f, "GIFT");
                break;
            }
        }
    }
}

// ============================================================================
// PAGE 11: STAGE SCREENS
// Contribution: Stage 0 & 1 (23-54110-3) | Stage 2 (23-54309-3)
//               Stage 3 (23-54796-3)     | Stage 4 & 5 (23-54954-3)
// ============================================================================

void drawHeader(const char* title) {
    // Top banner
    setColor(Color(0.12f, 0.15f, 0.28f));
    drawRect(0, WINDOW_HEIGHT - HEADER_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Breadcrumb progress tabs
    const char* stageNames[] = {"Welcome", "1. Shape", "2. Flavor", "3. Toppings", "4. Packaging", "5. Finish"};
    float startX = 40.0f;

    for (int i = 0; i < 6; i++) {
        float x = startX + i * 160.0f;
        bool active = (i == currentStage);

        if (active) {
            setColor(Color(0.25f, 0.55f, 0.95f));
            drawRect(x, WINDOW_HEIGHT - 48.0f, x + 140.0f, WINDOW_HEIGHT - 12.0f, true);
            setColor(Color(1.0f, 1.0f, 1.0f));
        } else if (i < currentStage) {
            setColor(Color(0.45f, 0.50f, 0.65f));
        } else {
            setColor(Color(0.25f, 0.28f, 0.40f));
        }
        drawSmallText(x + 12.0f, WINDOW_HEIGHT - 34.0f, stageNames[i]);
    }

    // Stage subtitle positioned cleanly below the tabs
    setColor(Color(1.0f, 1.0f, 1.0f));
    drawLargeText(WINDOW_WIDTH / 2.0f - strlen(title) * 6.5f, WINDOW_HEIGHT - 74.0f, title);
}

void drawFooter(const char* controls) {
    setColor(Color(0.12f, 0.15f, 0.28f));
    drawRect(0, 0, WINDOW_WIDTH, FOOTER_HEIGHT);
    setColor(Color(0.95f, 0.95f, 1.0f));
    drawSmallText(30.0f, 28.0f, controls);
}

// 0: Welcome Screen (Home page with Start and Quit options)
// Contribution: Md. Minhaj Rowfun Rabbi Anik (23-54110-3)
void drawWelcomeScreen() {
    glClear(GL_COLOR_BUFFER_BIT);

    setColor(Color(0.94f, 0.95f, 0.98f));
    drawRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Title banner
    setColor(Color(0.12f, 0.15f, 0.28f));
    drawRect(0, WINDOW_HEIGHT - 100.0f, WINDOW_WIDTH, WINDOW_HEIGHT);

    setColor(Color(1.0f, 1.0f, 1.0f));
    drawLargeText(WINDOW_WIDTH / 2.0f - 80.0f, WINDOW_HEIGHT - 60.0f, "CAKELAB");
    drawSmallText(WINDOW_WIDTH / 2.0f - 170.0f, WINDOW_HEIGHT - 85.0f,
                  "An Interactive 2D Cake Customization Game");

    // Project metadata card with Roll-wise contributions
    setColor(Color(1.0f, 1.0f, 1.0f));
    drawRect(WINDOW_WIDTH / 2.0f - 320.0f, WINDOW_HEIGHT - 390.0f,
             WINDOW_WIDTH / 2.0f + 320.0f, WINDOW_HEIGHT - 130.0f, true);
    setColor(Color(0.80f, 0.85f, 0.92f));
    drawRect(WINDOW_WIDTH / 2.0f - 320.0f, WINDOW_HEIGHT - 390.0f,
             WINDOW_WIDTH / 2.0f + 320.0f, WINDOW_HEIGHT - 130.0f, false);

    setColor(Color(0.15f, 0.20f, 0.35f));
    drawText(WINDOW_WIDTH / 2.0f - 290.0f, WINDOW_HEIGHT - 165.0f, "Group Members & Contributions:");
    setColor(Color(0.25f, 0.30f, 0.45f));
    drawSmallText(WINDOW_WIDTH / 2.0f - 260.0f, WINDOW_HEIGHT - 195.0f, "- Md. Minhaj Rowfun Rabbi Anik (23-54110-3) : Stage 1, Home, Quit, Loop");
    drawSmallText(WINDOW_WIDTH / 2.0f - 260.0f, WINDOW_HEIGHT - 220.0f, "- Adiba Howlader (23-54309-3)              : Stage 2 Flavor, Scaling, Shading");
    drawSmallText(WINDOW_WIDTH / 2.0f - 260.0f, WINDOW_HEIGHT - 245.0f, "- Md. Tanvir Hasan (23-54796-3)            : Stage 3 Toppings, UV Map, Animation");
    drawSmallText(WINDOW_WIDTH / 2.0f - 260.0f, WINDOW_HEIGHT - 270.0f, "- Abdur Rahman (23-54954-3)               : Stage 4 Packaging, Scoring, Finish");

    setColor(Color(0.15f, 0.20f, 0.35f));
    drawSmallText(WINDOW_WIDTH / 2.0f - 290.0f, WINDOW_HEIGHT - 310.0f, "Supervisor: Dr Mohammad Nazmul Alam");
    drawSmallText(WINDOW_WIDTH / 2.0f - 290.0f, WINDOW_HEIGHT - 340.0f, "Goal: Match the Target Order in the top right for a 100/100 score!");

    // Bobbing preview cake
    float bob = sinf(animClock * 2.0f) * 8.0f;
    drawCake(design, WINDOW_WIDTH / 2.0f, 210.0f + bob, 65.0f);

    // Primary action buttons with keybindings
    drawButton(WINDOW_WIDTH / 2.0f - 180.0f, 45.0f, 160.0f, 50.0f, "Start (s)", true);
    drawButton(WINDOW_WIDTH / 2.0f + 20.0f, 45.0f, 160.0f, 50.0f, "Quit (q)", false);

    if (showQuitDialog) drawQuitDialog();

    glutSwapBuffers();
}

// 1: Stage 1 - Base Shape & Tiers Selection
// Contribution: Md. Minhaj Rowfun Rabbi Anik (23-54110-3)
void drawShapeStage() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawHeader("Stage 1: Select Base Shape & Tiers");

    // Left sidebar panel
    setColor(Color(0.96f, 0.96f, 0.98f));
    drawRect(0, FOOTER_HEIGHT, PANEL_WIDTH, WINDOW_HEIGHT - HEADER_HEIGHT);

    setColor(Color(0.10f, 0.15f, 0.30f));
    drawText(25.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 35.0f, "SELECT SHAPE:");

    for (int i = 0; i < 5; i++) {
        char label[50];
        sprintf(label, "%d. %s", i + 1, shapeNames[i]);
        drawMenuItem(35.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 70.0f - i * 36.0f, 240.0f, label, i == design.shapeType);
    }

    setColor(Color(0.10f, 0.15f, 0.30f));
    drawText(25.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 275.0f, "TIER COUNT (+/-):");
    for (int i = 1; i <= 3; i++) {
        char label[30];
        sprintf(label, "%d Tier%s", i, i > 1 ? "s" : "");
        drawMenuItem(35.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 310.0f - (i - 1) * 32.0f, 150.0f, label, i == design.tierCount);
    }

    if (design.shapeType == 4) {
        setColor(Color(0.10f, 0.15f, 0.30f));
        drawText(25.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 430.0f, "POLYGON SIDES:");
        char label[30];
        sprintf(label, "%d Sides (Use +/-)", design.polygonSides);
        drawMenuItem(35.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 460.0f, 180.0f, label, true);
    }

    // Navigation buttons
    drawButton(25.0f, FOOTER_HEIGHT + 25.0f, 120.0f, 40.0f, "Previous (p)");
    drawButton(165.0f, FOOTER_HEIGHT + 25.0f, 120.0f, 40.0f, "Next (n)", true);

    // Canvas background
    setColor(Color(1.0f, 1.0f, 1.0f));
    drawRect(PANEL_WIDTH + 15.0f, FOOTER_HEIGHT + 15.0f, WINDOW_WIDTH - 295.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 15.0f);

    float cakeX = PANEL_WIDTH + (WINDOW_WIDTH - PANEL_WIDTH - 290.0f) / 2.0f;
    float cakeY = (WINDOW_HEIGHT / 2.0f) - 30.0f;
    drawCake(design, cakeX, cakeY, 95.0f);

    drawTargetCard();

    drawFooter("Keys: [1-5] Shape | [+/-] Tiers | [Next (n)] | [Previous (p)] | [Quit (q)]");

    if (showQuitDialog) drawQuitDialog();

    glutSwapBuffers();
}

// 2: Stage 2 - Flavor & Color Palette
// Contribution: Adiba Howlader (23-54309-3)
void drawFlavorStage() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawHeader("Stage 2: Select Flavor & Palette");

    setColor(Color(0.96f, 0.96f, 0.98f));
    drawRect(0, FOOTER_HEIGHT, PANEL_WIDTH, WINDOW_HEIGHT - HEADER_HEIGHT);

    setColor(Color(0.10f, 0.15f, 0.30f));
    drawText(25.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 35.0f, "SELECT FLAVOR:");

    for (int i = 0; i < 8; i++) {
        char label[50];
        sprintf(label, "%d. %s", i + 1, flavorNames[i]);
        drawMenuItem(35.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 70.0f - i * 35.0f, 240.0f, label, i == design.flavorType);
    }

    drawButton(25.0f, FOOTER_HEIGHT + 25.0f, 120.0f, 40.0f, "Previous (p)");
    drawButton(165.0f, FOOTER_HEIGHT + 25.0f, 120.0f, 40.0f, "Next (n)", true);

    setColor(Color(1.0f, 1.0f, 1.0f));
    drawRect(PANEL_WIDTH + 15.0f, FOOTER_HEIGHT + 15.0f, WINDOW_WIDTH - 295.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 15.0f);

    float cakeX = PANEL_WIDTH + (WINDOW_WIDTH - PANEL_WIDTH - 290.0f) / 2.0f;
    float cakeY = (WINDOW_HEIGHT / 2.0f) - 20.0f;
    drawCake(design, cakeX, cakeY, 95.0f);

    // Color Swatches
    FlavorPalette& f = flavors[design.flavorType];
    Color swatches[] = {f.sponge, f.filling, f.frosting, f.drip, f.accent};
    const char* names[] = {"Sponge", "Filling", "Frosting", "Drip", "Accent"};

    float swatchX = PANEL_WIDTH + 35.0f;
    float swatchY = FOOTER_HEIGHT + 35.0f;
    for (int i = 0; i < 5; i++) {
        setColor(swatches[i]);
        drawRect(swatchX + i * 115.0f, swatchY, swatchX + i * 115.0f + 95.0f, swatchY + 30.0f, true);
        setColor(Color(0.7f, 0.7f, 0.7f));
        drawRect(swatchX + i * 115.0f, swatchY, swatchX + i * 115.0f + 95.0f, swatchY + 30.0f, false);
        setColor(Color(0.2f, 0.2f, 0.3f));
        drawSmallText(swatchX + i * 115.0f + 8.0f, swatchY + 36.0f, names[i]);
    }

    drawTargetCard();

    drawFooter("Keys: [1-8] Flavor Palette | [Next (n)] | [Previous (p)] | [Quit (q)]");

    if (showQuitDialog) drawQuitDialog();

    glutSwapBuffers();
}

// 3: Stage 3 - Place Toppings
// Contribution: Md. Tanvir Hasan (23-54796-3)
void drawToppingsStage() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawHeader("Stage 3: Place Toppings on Top Surface");

    setColor(Color(0.96f, 0.96f, 0.98f));
    drawRect(0, FOOTER_HEIGHT, PANEL_WIDTH, WINDOW_HEIGHT - HEADER_HEIGHT);

    setColor(Color(0.10f, 0.15f, 0.30f));
    drawText(25.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 35.0f, "ADD INGREDIENTS:");

    for (int i = 0; i < 8; i++) {
        char label[50];
        sprintf(label, "%d. %s", i + 1, ingredientNames[i]);
        drawMenuItem(35.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 70.0f - i * 32.0f, 240.0f, label, false);
    }

    char countInfo[50];
    sprintf(countInfo, "Toppings Count: %zu / 80", design.ingredients.size());
    setColor(Color(0.20f, 0.25f, 0.40f));
    drawSmallText(35.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 340.0f, countInfo);

    drawButton(25.0f, FOOTER_HEIGHT + 85.0f, 120.0f, 38.0f, "Clear (c)");
    drawButton(165.0f, FOOTER_HEIGHT + 85.0f, 120.0f, 38.0f, "Random (r)");

    drawButton(25.0f, FOOTER_HEIGHT + 25.0f, 120.0f, 40.0f, "Previous (p)");
    drawButton(165.0f, FOOTER_HEIGHT + 25.0f, 120.0f, 40.0f, "Next (n)", true);

    setColor(Color(1.0f, 1.0f, 1.0f));
    drawRect(PANEL_WIDTH + 15.0f, FOOTER_HEIGHT + 15.0f, WINDOW_WIDTH - 295.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 15.0f);

    float cakeX = PANEL_WIDTH + (WINDOW_WIDTH - PANEL_WIDTH - 290.0f) / 2.0f;
    float cakeY = (WINDOW_HEIGHT / 2.0f) - 30.0f;
    drawCake(design, cakeX, cakeY, 95.0f);

    drawTargetCard();

    drawFooter("Keys: [1-8] Add Topping | [Clear (c)] | [Random (r)] | [Next (n)] | [Previous (p)]");

    if (showQuitDialog) drawQuitDialog();

    glutSwapBuffers();
}

// 4: Stage 4 - Select Packaging
// Contribution: Abdur Rahman (23-54954-3)
void drawPackagingStage() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawHeader("Stage 4: Select Packaging & Wrap");

    setColor(Color(0.96f, 0.96f, 0.98f));
    drawRect(0, FOOTER_HEIGHT, PANEL_WIDTH, WINDOW_HEIGHT - HEADER_HEIGHT);

    setColor(Color(0.10f, 0.15f, 0.30f));
    drawText(25.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 35.0f, "PACKAGING STYLE:");

    for (int i = 0; i < 5; i++) {
        char label[50];
        sprintf(label, "%d. %s", i, packagingNames[i]);
        drawMenuItem(35.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 70.0f - i * 36.0f, 240.0f, label, i == design.packagingType);
    }

    drawButton(25.0f, FOOTER_HEIGHT + 25.0f, 120.0f, 40.0f, "Previous (p)");
    drawButton(165.0f, FOOTER_HEIGHT + 25.0f, 120.0f, 40.0f, "Finish (f)", true);

    setColor(Color(1.0f, 1.0f, 1.0f));
    drawRect(PANEL_WIDTH + 15.0f, FOOTER_HEIGHT + 15.0f, WINDOW_WIDTH - 295.0f, WINDOW_HEIGHT - HEADER_HEIGHT - 15.0f);

    float cakeX = PANEL_WIDTH + (WINDOW_WIDTH - PANEL_WIDTH - 290.0f) / 2.0f;
    float cakeY = (WINDOW_HEIGHT / 2.0f) - 30.0f;
    drawCake(design, cakeX, cakeY, 95.0f);

    drawTargetCard();

    drawFooter("Keys: [0-4] Packaging | [Finish (f)] | [Previous (p)] | [Quit (q)]");

    if (showQuitDialog) drawQuitDialog();

    glutSwapBuffers();
}

// 5: Stage 5 - Final Completed Screen
// Contribution: Abdur Rahman (23-54954-3)
void drawFinishScreen() {
    glClear(GL_COLOR_BUFFER_BIT);

    setColor(Color(0.94f, 0.95f, 0.98f));
    drawRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    setColor(Color(0.12f, 0.15f, 0.28f));
    drawRect(0, WINDOW_HEIGHT - 80.0f, WINDOW_WIDTH, WINDOW_HEIGHT);

    setColor(Color(1.0f, 1.0f, 1.0f));
    drawLargeText(WINDOW_WIDTH / 2.0f - 140.0f, WINDOW_HEIGHT - 52.0f, "YOUR CAKE IS COMPLETE!");

    int finalScore = calculateScore();

    // Summary Card on Left
    setColor(Color(1.0f, 1.0f, 1.0f));
    drawRect(40.0f, FOOTER_HEIGHT + 30.0f, 380.0f, WINDOW_HEIGHT - 110.0f, true);
    setColor(Color(0.80f, 0.85f, 0.92f));
    drawRect(40.0f, FOOTER_HEIGHT + 30.0f, 380.0f, WINDOW_HEIGHT - 110.0f, false);

    // Score Banner
    if (finalScore >= 90) setColor(Color(0.15f, 0.65f, 0.25f));
    else if (finalScore >= 60) setColor(Color(0.90f, 0.55f, 0.10f));
    else setColor(Color(0.85f, 0.25f, 0.20f));
    drawRect(60.0f, WINDOW_HEIGHT - 175.0f, 360.0f, WINDOW_HEIGHT - 125.0f, true);

    setColor(Color(1.0f, 1.0f, 1.0f));
    char scoreText[50];
    sprintf(scoreText, "FINAL SCORE: %d / 100", finalScore);
    drawText(85.0f, WINDOW_HEIGHT - 155.0f, scoreText);

    // Breakdown Details
    setColor(Color(0.15f, 0.20f, 0.35f));
    drawText(60.0f, WINDOW_HEIGHT - 205.0f, "DESIGN SUMMARY:");

    char buf[100];
    setColor(Color(0.25f, 0.30f, 0.45f));
    sprintf(buf, "Shape: %s", shapeNames[design.shapeType]);
    drawSmallText(60.0f, WINDOW_HEIGHT - 235.0f, buf);

    sprintf(buf, "Tiers: %d Tier%s", design.tierCount, design.tierCount > 1 ? "s" : "");
    drawSmallText(60.0f, WINDOW_HEIGHT - 260.0f, buf);

    sprintf(buf, "Flavor: %s", flavorNames[design.flavorType]);
    drawSmallText(60.0f, WINDOW_HEIGHT - 285.0f, buf);

    sprintf(buf, "Packaging: %s", packagingNames[design.packagingType]);
    drawSmallText(60.0f, WINDOW_HEIGHT - 310.0f, buf);

    sprintf(buf, "Total Toppings: %zu", design.ingredients.size());
    drawSmallText(60.0f, WINDOW_HEIGHT - 335.0f, buf);

    int counts[9] = {0};
    for (const auto& ing : design.ingredients) {
        if (ing.type >= 1 && ing.type <= 8) counts[ing.type]++;
    }

    drawSmallText(60.0f, WINDOW_HEIGHT - 370.0f, "Ingredients Breakdown:");
    int ty = WINDOW_HEIGHT - 395.0f;
    for (int i = 1; i <= 8; i++) {
        if (counts[i] > 0) {
            sprintf(buf, "  - %s: %d", ingredientNames[i - 1], counts[i]);
            drawSmallText(60.0f, ty, buf);
            ty -= 20.0f;
        }
    }

    // Only 'Again (a)' button shown after completion
    drawButton(140.0f, FOOTER_HEIGHT + 50.0f, 160.0f, 42.0f, "Again (a)", true);

    // Large Cake Showcase
    float cakeCenterX = 420.0f + (WINDOW_WIDTH - 420.0f) / 2.0f;
    float cakeCenterY = (WINDOW_HEIGHT / 2.0f) - 30.0f;
    drawCake(design, cakeCenterX, cakeCenterY, 115.0f);

    drawFooter("Keys: [Again (a)] | [Quit (q)]");

    if (showQuitDialog) drawQuitDialog();

    glutSwapBuffers();
}

// ============================================================================
// PAGE 12: GLUT EVENT CALLBACKS
// Contribution: Md. Minhaj Rowfun Rabbi Anik (23-54110-3)
// ============================================================================

void display() {
    switch(currentStage) {
        case 0: drawWelcomeScreen(); break;
        case 1: drawShapeStage(); break;
        case 2: drawFlavorStage(); break;
        case 3: drawToppingsStage(); break;
        case 4: drawPackagingStage(); break;
        case 5: drawFinishScreen(); break;
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Full keyboard handling with dual-case support (e.g., 's' and 'S')
void keyboard(unsigned char key, int x, int y) {
    char k = tolower(key);

    // Handle Quit Confirmation Dialog if active
    if (showQuitDialog) {
        if (k == 'y') {
            exit(0);
        } else if (k == 'n') {
            showQuitDialog = false;
        }
        glutPostRedisplay();
        return;
    }

    // Trigger Quit Dialog from any stage
    if (k == 'q') {
        showQuitDialog = true;
        glutPostRedisplay();
        return;
    }

    switch(currentStage) {
        case 0: // Welcome Screen: Start (s) starts customization
            if (k == 's' || k == 'n' || k == 'w') {
                currentStage = 1;
            }
            break;

        case 1: // Stage 1: Shape & Tiers
            if (k >= '1' && k <= '5') {
                design.shapeType = k - '1';
            }
            if (k == '+' || k == '=') {
                if (design.shapeType == 4 && design.polygonSides < 8) {
                    design.polygonSides++;
                } else if (design.tierCount < 3) {
                    design.tierCount++;
                }
            }
            if (k == '-' || k == '_') {
                if (design.shapeType == 4 && design.polygonSides > 5) {
                    design.polygonSides--;
                } else if (design.tierCount > 1) {
                    design.tierCount--;
                }
            }
            if (k == 'n') currentStage = 2;
            if (k == 'p') currentStage = 0;
            break;

        case 2: // Stage 2: Flavor Palette
            if (k >= '1' && k <= '8') {
                design.flavorType = k - '1';
            }
            if (k == 'n') currentStage = 3;
            if (k == 'p') currentStage = 1;
            break;

        case 3: // Stage 3: Toppings
            if (k >= '1' && k <= '8') {
                if (design.ingredients.size() < 80) {
                    Ingredient ing;
                    ing.type = k - '0';

                    // Random placement on the top surface
                    float angle = (rand() % 360) * PI / 180.0f;
                    float r = ((rand() % 80) + 10) / 100.0f;
                    ing.u = cosf(angle) * r;
                    ing.v = sinf(angle) * r;
                    ing.scale = 0.85f + (rand() % 30) / 100.0f;
                    ing.rotation = (rand() % 60) - 30.0f;

                    design.ingredients.push_back(ing);
                }
            }
            if (k == 'r') { // Random scatter 6 toppings
                for (int i = 0; i < 6 && design.ingredients.size() < 80; i++) {
                    Ingredient ing;
                    ing.type = 1 + rand() % 8;
                    float angle = (rand() % 360) * PI / 180.0f;
                    float r = ((rand() % 80) + 10) / 100.0f;
                    ing.u = cosf(angle) * r;
                    ing.v = sinf(angle) * r;
                    ing.scale = 0.85f + (rand() % 30) / 100.0f;
                    ing.rotation = (rand() % 60) - 30.0f;
                    design.ingredients.push_back(ing);
                }
            }
            if (k == 'c') {
                design.ingredients.clear();
            }
            if (k == 'n') currentStage = 4;
            if (k == 'p') currentStage = 2;
            break;

        case 4: // Stage 4: Packaging
            if (k >= '0' && k <= '4') {
                design.packagingType = k - '0';
            }
            if (k == 'f') currentStage = 5;
            if (k == 'p') currentStage = 3;
            break;

        case 5: // Stage 5: Finish
            if (k == 'a') { // Play Again: reset state and generate new target order
                design = Design();
                initTargetOrder();
                currentStage = 0;
            }
            break;
    }

    glutPostRedisplay();
}

// 60 FPS animation timer for candle flame & gem glint
void timer(int value) {
    animClock += 0.02f;
    if (animClock > 2.0f * PI * 100.0f) animClock = 0.0f;
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// ============================================================================
// PAGE 13: MAIN ENTRY POINT
// Contribution: Md. Minhaj Rowfun Rabbi Anik (23-54110-3)
// ============================================================================

int main(int argc, char** argv) {
    srand((unsigned int)time(NULL));
    initTargetOrder();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(80, 40);
    glutCreateWindow("CakeLab - Interactive 2D Cake Customization Game");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);

    printf("\n=======================================================\n");
    printf("        CAKELAB - Interactive Cake Design Game         \n");
    printf("=======================================================\n");
    printf(" Group Members:\n");
    printf("   1. Md. Minhaj Rowfun Rabbi Anik (23-54110-3) - Stage 1, Home, Quit\n");
    printf("   2. Adiba Howlader (23-54309-3)              - Stage 2 Flavor\n");
    printf("   3. Md. Tanvir Hasan (23-54796-3)            - Stage 3 Toppings\n");
    printf("   4. Abdur Rahman (23-54954-3)               - Stage 4 Packaging\n");
    printf("=======================================================\n");
    printf(" Controls:\n");
    printf("   Welcome Screen : Start (s), Quit (q)\n");
    printf("   Stage 1 Shape  : [1-5] Shape, [+/-] Tiers, Next (n), Previous (p)\n");
    printf("   Stage 2 Flavor : [1-8] Flavor, Next (n), Previous (p)\n");
    printf("   Stage 3 Topping: [1-8] Add Toppings, Clear (c), Random (r), Next (n), Previous (p)\n");
    printf("   Stage 4 Package: [0-4] Packaging Style, Finish (f), Previous (p)\n");
    printf("   Finish Screen  : Again (a), Quit (q)\n");
    printf("   Global Quit    : Quit (q) -> Confirmation Dialog (y/n)\n");
    printf("=======================================================\n\n");

    glutMainLoop();
    return 0;
}
