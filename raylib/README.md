# Djazair Raylib Extension Manual

The **Raylib** extension is a powerful module for developing 2D games, animations, and interactive media in the Djazair programming language. It wraps the popular **Raylib** library, offering a clean Object-Oriented Programming (OOP) interface.

---

## 📋 Table of Contents
1. [Importing](#1-importing)
2. [Core Classes Reference](#2-core-classes-reference)
3. [Standalone Functions](#3-standalone-functions)
4. [Constants](#4-constants)
5. [Complete Game/Sandbox Example](#5-complete-gamesandbox-example)

---

## 1. Importing

To import the Raylib extension in your project, use:
```djazair
use raylib as rl
```

---

## 2. Core Classes Reference

### 1. `Window`
Manages application viewport, drawing loop, frame rate, and dimensions.
*   `init(w, h, title)`: Opens a new viewport window.
*   `isOpen()`: Returns `True` as long as the window has not been closed by the user or OS.
*   `begin()`: Starts drawing state for the current frame.
*   `finish()`: End drawing state, swap buffers, and poll events.
*   `clear(color)`: Wipes the screen background with a selected color.
*   `setFPS(fps)`: Limits maximum frames per second.
*   `getFPS()`: Retrieves current frames per second.
*   `setTitle(title)`: Dynamically changes window title.
*   `setSize(w, h)`: Dynamically resizes the window.
*   `close()`: Terminates window session and frees native resources.

### 2. `Camera2D`
Manages camera viewpoint offset, targets, rotation, and scale in 2D space.
*   `init()`: Instantiates camera. Properties:
    *   `offset_x`, `offset_y`: Screen coordinates anchor point.
    *   `target_x`, `target_y`: Target coordinates the camera centers on in world space.
    *   `zoom`: Zoom scale factor (Default: `1.0`).
    *   `rotation`: Viewport rotation angle in degrees.
*   `begin()`: Enables 2D camera viewport state.
*   `finish()`: Exits camera viewport state, returning coordinates back to standard screen coordinates (HUD layer).
*   `toWorld(screenX, screenY)`: Converts screen coordinates (e.g., mouse positions) into world space. Returns `[worldX, worldY]`.
*   `toScreen(worldX, worldY)`: Converts world space coordinates into screen space. Returns `[screenX, screenY]`.

### 3. `Texture`
Loads and draws image sprites to VRAM.
*   `init(path)`: Loads image file into VRAM.
*   `draw(x, y)`: Draws texture at coordinate.
*   `drawRec(srcX, srcY, srcW, srcH, destX, destY, color)`: Draws a cropped region of the texture.
*   `drawEx(destX, destY, rotation, scale, color)`: Draws texture with custom rotation, scale factor, and color tint.
*   `unload()`: Wipes texture from VRAM to release resources.

### 4. 2D Shapes (`Circle`, `Rectangle`)
Wrapper classes for 2D shape drawing and collision checks.
*   `draw()`: Draws shape filled with color.
*   `drawLines()`: Draws shape outline.
*   `drawGradientV(color2)`: *(Rectangle only)* Draws shape with vertical gradient.
*   `checkCollision(other)`: Collision checking between shapes (circle vs circle, or rect vs rect).

### 5. Media & Audio (`Audio`, `Sound`, `Music`)
Manages audio devices and sound effects/music streams.
*   `Audio()`: Initializes system sound device.
    *   `close()`: Deactivates sound device.
*   `Sound(path)`: Load short sound effects.
    *   `play()`: Plays sound effect.
    *   `unload()`: Unloads sound effect.
*   `Music(path)`: Load stream background music.
    *   `play()`: Starts streaming.
    *   `update()`: Polls stream packets. Must be invoked inside game loop once per frame.
    *   `stop()` / `pause()` / `resume()`: Playback state controls.
    *   `isPlaying()`: Returns playing state.
    *   `setVolume(volume)`: Sets output volume (`0.0` to `1.0`).
    *   `unload()`: Unloads stream.

### 6. Text drawing (`Text`, `Font`)
Draws text characters using default or custom TTF fonts.
*   `Text(content, size, color)`: Instantiates text object.
    *   `draw(x, y)`: Draws text at coordinates.
*   `Font(path)`: Loads TTF font file.
    *   `draw(text, x, y, size, spacing, color)`: Draws text with custom font and spacing.
    *   `unload()`: Unloads font data.

### 7. Vector2 & Color
Helper structures for points and colors.
*   `Vector2(x, y)`: Vector coordinates. Methods: `add(other)`, `sub(other)`, `scale(factor)`.
*   `Color(r, g, b, a)`: RGBA color. Exposes `.raw` array `[r, g, b, a]` for native call mapping.

---

## 3. Standalone Functions

### Collision Testing:
*   `rl.checkCollisionCircles(c1x, c1y, c1r, c2x, c2y, c2r)`: Circle vs Circle.
*   `rl.checkCollisionRecs(r1x, r1y, r1w, r1h, r2x, r2y, r2w, r2h)`: Rect vs Rect.
*   `rl.checkCollisionCircleRec(cx, cy, r, rx, ry, rw, rh)`: Circle vs Rect.
*   `rl.checkCollisionPointRec(px, py, rx, ry, rw, rh)`: Point vs Rect.

### Direct Shape Drawing:
*   `rl.drawRectangle(x, y, w, h, color)`
*   `rl.drawRectangleLines(x, y, w, h, color)`
*   `rl.drawRectangleGradientV(x, y, w, h, color1, color2)`
*   `rl.drawRectangleRounded(x, y, w, h, roundness, segments, color)`
*   `rl.drawCircle(x, y, radius, color)`
*   `rl.drawCircleLines(x, y, radius, color)`
*   `rl.drawEllipse(centerX, centerY, radiusH, radiusV, color)`
*   `rl.drawLine(x1, y1, x2, y2, color)`
*   `rl.drawLineEx(x1, y1, x2, y2, thickness, color)`
*   `rl.drawTriangle(v1x, v1y, v2x, v2y, v3x, v3y, color)`
*   `rl.drawTriangleLines(v1x, v1y, v2x, v2y, v3x, v3y, color)`
*   `rl.drawPoly(x, y, sides, radius, rotation, color)`
*   `rl.drawText(text, x, y, size, color)`

### Input polling (Keyboard & Mouse):
*   `rl.isKeyDown(key)` / `rl.isKeyPressed(key)` / `rl.isKeyReleased(key)` / `rl.isKeyUp(key)`
*   `rl.getKeyPressed()`: Returns keycode integer of last key pressed.
*   `rl.getCharPressed()`: Returns character unicode integer.
*   `rl.isMouseButtonPressed(btn)` / `rl.isMouseButtonDown(btn)` / `rl.isMouseButtonReleased(btn)` / `rl.isMouseButtonUp(btn)`
*   `rl.getMouseX()` / `rl.getMouseY()`
*   `rl.getMouseWheelMove()`

### Viewport and Utilities:
*   `rl.toggleFullscreen()`
*   `rl.isWindowResized()`
*   `rl.getScreenWidth()` / `rl.getScreenHeight()`
*   `rl.getTime()`: Time elapsed in seconds since start.
*   `rl.getFrameTime()`: Delat-time duration of last frame in seconds (useful for constant speeds).
*   `rl.getRandomValue(min, max)`
*   `rl.hideCursor()` / `rl.showCursor()`
*   `rl.measureText(text, size)`

---

## 4. Constants

*   **Colors**: `rl.WHITE`, `rl.BLACK`, `rl.RED`, `rl.GREEN`, `rl.BLUE`, `rl.YELLOW`, `rl.RAYWHITE`, `rl.SKYBLUE`, `rl.LIME`, `rl.GOLD`, `rl.PURPLE`, etc.
*   **Keyboard Keys**: `rl.KEY_LEFT`, `rl.KEY_RIGHT`, `rl.KEY_UP`, `rl.KEY_DOWN`, `rl.KEY_A` to `rl.KEY_Z`, `rl.KEY_0` to `rl.KEY_9`, `rl.KEY_F1` to `rl.KEY_F12`, `rl.KEY_SPACE`, `rl.KEY_ENTER`, `rl.KEY_ESCAPE`, `rl.KEY_BACKSPACE`, `rl.KEY_TAB`.
*   **Mouse Buttons**: `rl.MOUSE_LEFT_BUTTON`, `rl.MOUSE_RIGHT_BUTTON`, `rl.MOUSE_MIDDLE_BUTTON`.

---

## 5. Complete Game/Sandbox Example

```djazair
use raylib as rl

# 1. Open viewport window
let win = rl.Window(800, 600, "Djazair Advanced Sandbox")
win.setFPS(60)

# 2. Setup 2D Camera
let camera = rl.Camera2D()
camera.offset_x = 400
camera.offset_y = 300
camera.zoom = 1.0

# 3. Setup shapes
let targetRec = rl.Rectangle(-100, -50, 200, 100, rl.GREEN)
let ballPos = rl.Vector2(0, 150)
let ballRadius = 20

while win.isOpen():
    # Camera controls
    if rl.isKeyDown(rl.KEY_A): camera.target_x -= 5.0 end
    if rl.isKeyDown(rl.KEY_D): camera.target_x += 5.0 end
    
    # Translate mouse screen position to world space
    let mX = rl.getMouseX()
    let mY = rl.getMouseY()
    let worldCoords = camera.toWorld(mX, mY)
    let worldX = worldCoords[0]
    let worldY = worldCoords[1]
    
    # Check collision
    let isColliding = rl.checkCollisionCircleRec(ballPos.x, ballPos.y, ballRadius, targetRec.x, targetRec.y, targetRec.w, targetRec.h)
    if isColliding:
        targetRec.color = rl.RED
    else:
        targetRec.color = rl.GREEN
    end
    
    # Drawing Frame
    win.begin()
    win.clear(rl.BLACK)
    
    # Camera Viewport
    camera.begin()
    
    # Draw reference grid
    let gx = -400
    while gx <= 400:
        rl.drawLine(gx, -400, gx, 400, rl.DARKGRAY)
        rl.drawLine(-400, gx, 400, gx, rl.DARKGRAY)
        gx = gx + 100
    end
    
    targetRec.draw()
    rl.drawCircle(ballPos.x, ballPos.y, ballRadius, rl.PURPLE)
    
    camera.finish()
    
    # HUD overlay layer
    rl.drawText("Use A/D to pan camera", 10, 10, 16, rl.WHITE)
    rl.drawText("Mouse World Coords: [" + str(int(worldX)) + ", " + str(int(worldY)) + "]", 10, 30, 16, rl.SKYBLUE)
    
    win.finish()
end

win.close()
```
