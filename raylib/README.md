# إضافة Raylib لغة دزاير (Raylib Extension for Djazair)

تعتبر هذه الإضافة الأقوى لتطوير الألعاب والرسومات والوسائط التفاعلية ثنائية الأبعاد (2D) داخل لغة دزاير. توفر واجهة برمجية كائنية التوجه (OOP) بالكامل تغلف وظائف مكتبة Raylib الشهيرة.

---

## كيفية الاستدعاء (Importing)
يتم استيراد الحزمة البرمجية بالكامل باستخدام الاسم المستعار `rl`:
```djazair
use raylib as rl
```

---

## الكلاسات الأساسية (Core Classes)

### 1. كلاس النافذة `Window`
لإدارة نافذة التطبيق ودورة الرسم ومعدل الإطارات:
*   `init(w, h, title)`: فتح نافذة جديدة بالأبعاد والعنوان المحدد.
*   `isOpen()`: التحقق مما إذا كانت النافذة لا تزال مفتوحة (ترجع `True` طالما لم يتم إغلاقها).
*   `begin()`: بدء دورة الرسم الحالية للشاشة.
*   `finish()`: إنهاء دورة الرسم وعرض المخرجات على الشاشة.
*   `clear(color)`: مسح الخلفية وتلوينها بلون معين.
*   `setFPS(fps)`: تحديد الحد الأقصى لمعدل الإطارات في الثانية (Target FPS).
*   `getFPS()`: جلب معدل الإطارات الحالي.
*   `setTitle(title)`: تغيير عنوان النافذة ديناميكياً.
*   `setSize(w, h)`: تغيير حجم النافذة.
*   `close()`: إغلاق النافذة وتحرير الموارد.

### 2. كلاس الكاميرا ثنائية الأبعاد `Camera2D`
للتحكم في الرؤية، التحجيم، والتدوير داخل فضاء العالم الافتراضي للعبة:
*   `init()`: إنشاء كائن كاميرا بالخصائص الافتراضية:
    *   `offset_x`, `offset_y`: نقطة ارتكاز الشاشة (أبعاد الكاميرا).
    *   `target_x`, `target_y`: النقطة التي تنظر إليها الكاميرا في العالم.
    *   `zoom`: معامل التقريب والتبعيد (الافتراضي `1.0`).
    *   `rotation`: زاوية دوران الكاميرا.
*   `begin()`: تفعيل وضع الكاميرا ثنائية الأبعاد للرسم.
*   `finish()`: إيقاف وضع الكاميرا والعودة لرسم الإحداثيات المباشرة للشاشة (HUD).
*   `toWorld(screenX, screenY)`: تحويل إحداثيات الشاشة (الفأرة مثلاً) إلى إحداثيات عالم اللعبة ثنائي الأبعاد. يُرجع مصفوفة ثنائية `[worldX, worldY]`.
*   `toScreen(worldX, worldY)`: تحويل إحداثيات عالم اللعبة إلى إحداثيات الشاشة ثنائية الأبعاد. يُرجع مصفوفة ثنائية `[screenX, screenY]`.

### 3. كلاس الصور `Texture`
تحميل ورسم الصور والـ Sprites:
*   `init(path)`: تحميل الصورة من المسار المحدد إلى ذاكرة الفيديو.
*   `draw(x, y)`: رسم الصورة عند الإحداثيات المحددة على الشاشة.
*   `drawRec(srcX, srcY, srcW, srcH, destX, destY, color)`: رسم جزء محدد من الصورة.
*   `drawEx(destX, destY, rotation, scale, color)`: رسم الصورة مع تدوير وتكبير/تصغير.
*   `unload()`: مسح الصورة من ذاكرة الشاشة لتحرير الموارد.

### 4. الأشكال الهندسية (`Circle`, `Rectangle`)
تسهل رسم والتحكم في الأشكال ثنائية الأبعاد:
*   `draw()`: رسم الشكل ممتلئاً باللون المحدد.
*   `drawLines()`: رسم الحدود الخارجية للشكل فقط.
*   `drawGradientV(color2)`: **(Rectangle فقط)** رسم مستطيل بتدرج لوني عمودي.
*   `checkCollision(other)`: التحقق من وجود تصادم مع شكل هندسي آخر (دائرة مع دائرة أو مستطيل مع مستطيل).

### 5. كلاسات الصوت والوسائط (`Sound`, `Audio`, `Music`)
لتشغيل المؤثرات الصوتية والموسيقى:
*   `Audio()`: كائن مشغل لتفعيل نظام الصوت المدمج في الجهاز (`Audio.init()`).
    *   `close()`: إيقاف نظام الصوت وتحرير الموارد.
*   `Sound(path)`: تحميل الملف الصوتي القصير (المؤثرات) من المسار المحدد.
    *   `play()`: تشغيل الصوت.
    *   `unload()`: تفريغ الملف الصوتي من الذاكرة.
*   `Music(path)`: تحميل بث موسيقى طويل (Background Music) من المسار المحدد.
    *   `play()`: بدء تشغيل بث الموسيقى.
    *   `update()`: تحديث دفق البيانات للموسيقى (يجب استدعاؤه في حلقة اللعب الرئيسية لكل إطار).
    *   `stop()`: إيقاف تشغيل الموسيقى وإعادة مؤشر القراءة للبداية.
    *   `pause()`: إيقاف الموسيقى مؤقتاً.
    *   `resume()`: استئناف تشغيل الموسيقى بعد الإيقاف المؤقت.
    *   `isPlaying()`: التحقق مما إذا كانت الموسيقى تعمل حالياً (ترجع `True` أو `False`).
    *   `setVolume(volume)`: ضبط شدة الصوت (من `0.0` إلى `1.0`).
    *   `unload()`: تفريغ بث الموسيقى من الذاكرة كلياً.

### 6. كلاس النصوص `Text`
عرض نصوص على الشاشة:
*   `init(content, size, color)`: إنشاء كائن نص بالمحتوى والحجم واللون المحدد.
*   `draw(x, y)`: رسم النص عند الإحداثيات المحددة.

```djazair
let txt = rl.Text("Hello", 20, rl.WHITE)
txt.draw(100, 100)
```

### 7. كلاس الخطوط `Font`
تحميل خطوط مخصصة ورسم نصوص بها:
*   `init(path)`: تحميل ملف خط من المسار المحدد.
*   `draw(text, x, y, size, spacing, color)`: رسم نص بالخط المحمّل مع تباعد أحرف مخصص.
*   `unload()`: تفريغ الخط من الذاكرة.

### 8. كلاس المتجه `Vector2`
يمثل نقطة أو متجهاً ثنائي الأبعاد:
*   `init(x, y)`: إنشاء متجه جديد.
*   `add(other)`: جمع متجهين وإرجاع النتيجة.
*   `sub(other)`: طرح متجهين وإرجاع النتيجة.
*   `scale(factor)`: ضرب المتجه بعدد.

### 9. كلاس اللون `Color`
يمثل لوناً مكوناً من قنوات RGBA:
*   `init(r, g, b, a)`: إنشاء لون جديد.
*   `raw`: المصفوفة الخام `[r, g, b, a]` للاستخدام مع دوال C.

---

## الدوال البرمجية المباشرة (Standalone Functions)

### فحص التصادمات
*   `rl.checkCollisionCircles(c1x, c1y, c1r, c2x, c2y, c2r)`: تصادم دائرة مع دائرة.
*   `rl.checkCollisionRecs(r1x, r1y, r1w, r1h, r2x, r2y, r2w, r2h)`: تصادم مستطيل مع مستطيل.
*   `rl.checkCollisionCircleRec(cx, cy, r, rx, ry, rw, rh)`: تصادم دائرة مع مستطيل.
*   `rl.checkCollisionPointRec(px, py, rx, ry, rw, rh)`: التحقق مما إذا كانت نقطة تقع ضمن مستطيل.

### دوال الرسم المباشرة
*   `rl.drawRectangle(x, y, w, h, color)`: رسم مستطيل ممتلئ.
*   `rl.drawRectangleLines(x, y, w, h, color)`: رسم حدود مستطيل.
*   `rl.drawRectangleGradientV(x, y, w, h, color1, color2)`: رسم مستطيل بتدرج عمودي.
*   `rl.drawRectangleRounded(x, y, w, h, roundness, segments, color)`: رسم مستطيل مدوّن الزوايا.
*   `rl.drawCircle(x, y, radius, color)`: رسم دائرة ممتلئة.
*   `rl.drawCircleLines(x, y, radius, color)`: رسم حدود دائرة.
*   `rl.drawEllipse(centerX, centerY, radiusH, radiusV, color)`: رسم بيضاوي.
*   `rl.drawLine(x1, y1, x2, y2, color)`: رسم خط.
*   `rl.drawLineEx(x1, y1, x2, y2, thickness, color)`: رسم خط بسمك محدد.
*   `rl.drawTriangle(v1x, v1y, v2x, v2y, v3x, v3y, color)`: رسم مثلث ممتلئ.
*   `rl.drawTriangleLines(v1x, v1y, v2x, v2y, v3x, v3y, color)`: رسم حدود مثلث.
*   `rl.drawPoly(x, y, sides, radius, rotation, color)`: رسم مضلع منتظم.
*   `rl.drawText(text, x, y, size, color)`: رسم نص على الشاشة.

### الإدخال (Keyboard & Mouse)
*   `rl.isKeyDown(key)`: هل المفتاح مضغوط حالياً؟
*   `rl.isKeyPressed(key)`: هل المفتاح ضُغط في هذا الإطار؟
*   `rl.isKeyReleased(key)`: هل المفتاح حُرّر في هذا الإطار؟
*   `rl.isKeyUp(key)`: هل المفتاح غير مضغوط؟
*   `rl.getKeyPressed()`: يرجع آخر مفتاح ضُغط (أو `0`).
*   `rl.getCharPressed()`: يرجع آخر حرف أدخله المستخدم.
*   `rl.isMouseButtonPressed(btn)`: هل زر الفأرة ضُغط؟
*   `rl.isMouseButtonDown(btn)`: هل زر الفأرة مضغوط حالياً؟
*   `rl.isMouseButtonReleased(btn)`: هل زر الفأرة حُرّر؟
*   `rl.isMouseButtonUp(btn)`: هل زر الفأرة غير مضغوط؟
*   `rl.getMouseX()`: موقع الفأرة على المحور X.
*   `rl.getMouseY()`: موقع الفأرة على المحور Y.
*   `rl.getMouseWheelMove()`: مقدار حركة عجلة الفأرة.

### النوافذ والنظام
*   `rl.toggleFullscreen()`: التبديل لوضع ملء الشاشة.
*   `rl.isWindowResized()`: ترجع `True` إذا قام المستخدم بتغيير حجم النافذة في الإطار الحالي.
*   `rl.getScreenWidth()`: عرض الشاشة الحالي بالبكسل.
*   `rl.getScreenHeight()`: ارتفاع الشاشة الحالي بالبكسل.
*   `rl.getTime()`: ترجع وقت التشغيل المنقضي بالثواني منذ بدء التطبيق.
*   `rl.getFrameTime()`: ترجع وقت آخر إطار بالثواني (لحساب الحركة).
*   `rl.getRandomValue(min, max)`: توليد رقم عشوائي صحيح بين الحدين.
*   `rl.hideCursor()`: إخفاء مؤشر الفأرة.
*   `rl.showCursor()`: إظهار مؤشر الفأرة.
*   `rl.measureText(text, size)`: قياس عرض نص بالبكسل.

---

## الثوابت (Constants)

توفر الإضافة ثوابت جاهزة للألوان والمفاتيح البرمجية:
*   **الألوان**: `rl.WHITE`, `rl.BLACK`, `rl.RED`, `rl.GREEN`, `rl.BLUE`, `rl.YELLOW`, `rl.RAYWHITE`, `rl.SKYBLUE`, `rl.LIME`, `rl.GOLD`, `rl.PURPLE`, إلخ.
*   **مفاتيح لوحة المفاتيح الشائعة**:
    *   الاتجاهات: `rl.KEY_LEFT`, `rl.KEY_RIGHT`, `rl.KEY_UP`, `rl.KEY_DOWN`
    *   الحروف والأرقام: `rl.KEY_A` إلى `rl.KEY_Z`, و `rl.KEY_0` إلى `rl.KEY_9`
    *   مفاتيح الوظائف: `rl.KEY_F1` إلى `rl.KEY_F12`
    *   أزرار التحكم: `rl.KEY_SPACE`, `rl.KEY_ENTER`, `rl.KEY_ESCAPE`, `rl.KEY_BACKSPACE`, `rl.KEY_TAB`
    *   أزرار جانبية: `rl.KEY_RIGHT_SHIFT`, `rl.KEY_RIGHT_CONTROL`, `rl.KEY_RIGHT_ALT`
    *   التحرير: `rl.KEY_BACKSPACE`, `rl.KEY_DELETE`, `rl.KEY_INSERT`, `rl.KEY_HOME`, `rl.KEY_END`, `rl.KEY_PAGE_UP`, `rl.KEY_PAGE_DOWN`
    *   قفل: `rl.KEY_CAPS_LOCK`, `rl.KEY_NUM_LOCK`, `rl.KEY_PRINT_SCREEN`, `rl.KEY_PAUSE`
*   **أزرار الفأرة**: `rl.MOUSE_LEFT_BUTTON`, `rl.MOUSE_RIGHT_BUTTON`, `rl.MOUSE_MIDDLE_BUTTON`

---

## مثال تطبيقي متكامل (تحويل إحداثيات الكاميرا والتصادمات)

```djazair
use raylib as rl

# 1. تهيئة النافذة والاتصال بنظام الصوت
let win = rl.Window(800, 600, "Djazair Advanced Sandbox")
win.setFPS(60)

# 2. إعداد الكاميرا ثنائية الأبعاد
let camera = rl.Camera2D()
camera.offset_x = 400
camera.offset_y = 300
camera.zoom = 1.0

# 3. إعداد المستطيل والكرة
let targetRec = rl.Rectangle(-100, -50, 200, 100, rl.GREEN)
let ballPos = rl.Vector2(0, 150)
let ballRadius = 20

while win.isOpen():
    # التحكم في تدوير الكاميرا
    if rl.isKeyDown(rl.KEY_A): camera.target_x -= 5.0 end
    if rl.isKeyDown(rl.KEY_D): camera.target_x += 5.0 end
    
    # تتبع موقع الفأرة في العالم (World Space)
    let mX = rl.getMouseX()
    let mY = rl.getMouseY()
    let worldCoords = camera.toWorld(mX, mY)
    let worldX = worldCoords[0]
    let worldY = worldCoords[1]
    
    # فحص التصادم بين الكرة (دائرة) والمستطيل
    let isColliding = rl.checkCollisionCircleRec(ballPos.x, ballPos.y, ballRadius, targetRec.x, targetRec.y, targetRec.w, targetRec.h)
    if isColliding:
        targetRec.color = rl.RED
    else:
        targetRec.color = rl.GREEN
    end
    
    # الرسم
    win.begin()
    win.clear(rl.BLACK)
    
    # فضاء الكاميرا
    camera.begin()
    
    # رسم شبكة مرجعية بسيطة
    let gx = -400
    while gx <= 400:
        rl.drawLine(gx, -400, gx, 400, rl.DARKGRAY)
        rl.drawLine(-400, gx, 400, gx, rl.DARKGRAY)
        gx = gx + 100
    end
    
    targetRec.draw()
    rl.drawCircle(ballPos.x, ballPos.y, ballRadius, rl.PURPLE)
    
    camera.finish()
    
    # رسم معلومات الشاشة المباشرة (HUD)
    rl.drawText("Use A/D to pan camera", 10, 10, 16, rl.WHITE)
    rl.drawText("Mouse World Coords: [" + str(int(worldX)) + ", " + str(int(worldY)) + "]", 10, 30, 16, rl.SKYBLUE)
    
    win.finish()
end

win.close()
```
