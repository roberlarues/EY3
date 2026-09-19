# Tareas pendientes

Cosas detectadas y aparcadas a propósito, con el contexto necesario para
retomarlas sin volver a investigarlas. Cada una indica si está **decidida** o
es una **propuesta** por valorar.

---

## 1. Renderizado sin pantalla (para trabajar en el VPS) — propuesta

**Qué pasa.** Compilar y firmar APKs en un servidor sin entorno gráfico
funciona sin más. Lo que no funciona es *ejecutar* el simulador de escritorio:
`WindowDesktop` crea una ventana con GLFW y pide un contexto EGL/GLES real, y
ambas cosas necesitan un display.

**Por qué importa.** Es la única forma de comprobar que un cambio se ve bien.
En desarrollo local esto se ha estado validando lanzando la app y capturando la
pantalla; en remoto hay que sustituirlo por algo equivalente.

**Opciones, de menos a más invasiva.**

1. **`Xvfb` delante.** `xvfb-run -s "-screen 0 1080x2400x24" ./app`. Cero
   cambios en el código. Requiere Mesa por software (llvmpipe) si el servidor
   no tiene GPU: `LIBGL_ALWAYS_SOFTWARE=1`. Lo más rápido de probar.
2. **EGL *surfaceless* + FBO.** Un modo sin ventana en `WindowDesktop` (o una
   `WindowHeadless` hermana) que cree el contexto con
   `EGL_PLATFORM_SURFACELESS_MESA` y renderice a un framebuffer propio. Sin
   dependencia de X ni Wayland.
3. **Volcado de fotogramas.** Junto a cualquiera de las dos anteriores: un
   `glReadPixels` + guardar PNG (con OpenCV, que ya está) cada N fotogramas o
   bajo una variable de entorno. Esto es lo que de verdad hace falta para
   verificar en remoto, y de paso sirve para capturas automáticas en CI.

**Cómo comprobarlo.** Lanzar `ey3-maze-desktop` en el servidor y obtener un PNG
con el laberinto dibujado, sin que haya ningún display conectado.

---

## 2. Dejar de depender del entorno al configurar — propuesta

Ver `environment-setup.md`, sección 7.1, donde está el diagnóstico completo.
Resumen: `$ENV{ANDROID_HOME}` y `$ENV{JAVA_HOME}` se leen al configurar y se
escriben dentro de las reglas, así que configurar sin ellas genera rutas rotas
(`/build-tools/34.0.0/aapt`, `/bin/keytool`) y el fallo aparece al compilar.

**Arreglo propuesto:** resolver `aapt`, `zipalign` y `keytool` con
`find_program()` y guardarlos en la caché de CMake, y hacer que la regla del
keystore no se reintente si el fichero ya existe (hoy, reconfigurar cambia la
línea de comandos, ninja la reintenta y `keytool` falla porque el alias ya
existe, deteniendo la compilación).

---

## 3. Quitar `EY3_WITH_CV=OFF` — decidido

OpenCV pasa a ser dependencia incondicional: ya está integrado, y mantener el
camino sin él añade complejidad para ahorrar ~1,5 MB, que no compensa.

**Qué se toca:** las guardas `if(EY3_WITH_CV)` del `CMakeLists.txt` raíz y de
`desktop/CMakeLists.txt`, los `#ifdef EY3_WITH_CV` de `include/ey3.h` y
`include/system/asset_loader.h`, la opción en ambos `CMakeLists.txt`, el
`templates/ey3-starter/android/CMakeLists.txt` (lo replica) y la sección 6 de
`environment-setup.md`, que describe una validación en dos etapas que dejará de
tener sentido.

---

## 4. Texturas con alfa — propuesta

`Texture` está fijada a `GL_RGB` y `AssetLoader::loadImageAsset` lee con
`IMREAD_COLOR`, así que el canal alfa se pierde dos veces. Por eso los sprites
de `apps/ey3-maze` usan *color key* magenta, que el fragment shader descarta.

**No requiere ninguna dependencia nueva:** OpenCV ya decodifica el alfa con
`IMREAD_UNCHANGED`. Haría falta un parámetro de formato en `Texture::generate`
(o detectarlo por el número de canales del `cv::Mat`) y una variante de
`loadImageAsset` que no descarte el alfa. Con eso, el *color key* y su
`discard` en el shader desaparecen.

---

## 5. Texto — propuesta (planificado)

Con `cv::putText` sobre un `cv::Mat` que se sube como textura, sin dependencias
nuevas. Limitaciones a tener en cuenta: fuentes Hershey (feas), sin kerning ni
Unicode, y cada cambio de texto implica volver a subir la textura. Suficiente
para marcadores, HUD y depuración; corto para una interfaz con mucho texto.

---

## 6. Sonido — propuesta (planificado)

Sin decidir nada todavía. Nota para cuando toque: es el único bloque grande que
no puede apoyarse en OpenCV, así que aquí sí habrá que elegir dependencia
(OpenSL ES / AAudio en Android, y algo equivalente en escritorio).

---

## 7. Avisar de caracteres desconocidos en los niveles — propuesta

`MazeGame::buildLevel` (`apps/ey3-maze/src/game/maze_game.cpp`) ignora en
silencio cualquier carácter que no esté en la leyenda. Como los niveles de
`assets/levels/` se editan a mano, un `LOGW` con fila, columna y carácter
ahorraría tiempo cuando un nivel no se vea como se esperaba.
