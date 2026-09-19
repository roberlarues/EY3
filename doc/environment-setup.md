# Entorno de desarrollo: instalación

Guía para dejar listo un entorno capaz de compilar EY3 desde CLI, sin
Android Studio. Cubre dos escenarios que deben acabar en el mismo estado:

- **Desarrollo local**: Arch Linux.
- **CI**: Ubuntu (probablemente Ubuntu LTS en el runner).

La estrategia es instalar el SDK/NDK de Android vía `sdkmanager` (herramienta
oficial, multiplataforma) en vez de paquetes de la distro, para que la
versión instalada sea *idéntica* en ambos sistemas. Solo las herramientas de
base (Java, cmake, ninja, utilidades) se instalan con el gestor de paquetes
de cada distro.

## 1. Resumen de versiones

| Herramienta | Versión recomendada | Motivo |
|---|---|---|
| JDK | 17 | Exigido por las `cmdline-tools` recientes de Android; usado además para `keytool`/`jarsigner`. |
| cmake | ≥ 3.22 (el del sistema) | Generador de build, usado tanto para EY3 como para OpenCV. |
| ninja | ≥ 1.10 (el del sistema) | Backend de build, usado por todos los `add_custom_command` del proyecto. |
| Android cmdline-tools | última | Para instalar SDK/NDK/build-tools de forma reproducible vía `sdkmanager`. |
| Android build-tools | 34.0.0 (o la última estable) | `aapt`, `zipalign` (usados directamente en los `CMakeLists.txt` de `apps/`). |
| Android platform | android-24 | Ver nota más abajo — es el mínimo real que exige el código, no el que declaran los `AndroidManifest.xml` actuales (19). |
| Android NDK | r27 (LTS) | Toolchain de compilación C++ para Android. Evitar release candidates. |
| OpenCV | 4.x reciente (compilado desde fuente para Android) | Solo necesario si vas a activar `EY3_WITH_CV=ON`. |

> **Nota sobre `android-24`**: el código usa Camera2 NDK (`ACameraManager_create`,
> etc.) y `AImageReader`, ambas APIs disponibles desde API 24. Los
> `AndroidManifest.xml` de los ejemplos declaran `minSdkVersion="19"`, lo cual
> es inconsistente con esa dependencia real (un dispositivo API 19-23
> crashearía al usar la cámara). No lo he corregido todavía — lo dejo anotado
> aquí como pendiente, no como parte de esta guía de entorno.

## 2. Paquetes base de la distro

### Arch Linux (desarrollo local)

```bash
sudo pacman -S --needed jdk17-openjdk cmake ninja unzip wget git base-devel
```

### Ubuntu (pipeline de CI)

```bash
sudo apt-get update
sudo apt-get install -y openjdk-17-jdk cmake ninja-build unzip wget git build-essential
```

`build-essential`/`base-devel` hacen falta porque `cmake` ejecuta
comprobaciones de compilador nativo (`try_compile`) al configurar, aunque el
build final de EY3 se cross-compile con el NDK.

## 3. Android SDK (cmdline-tools)

Mismo procedimiento en Arch y Ubuntu, ya que se descarga el zip oficial de
Google en vez de usar el gestor de paquetes de la distro.

1. Descargar las *Command line tools only* desde la página oficial:
   https://developer.android.com/studio#command-tools
2. Descomprimir de forma que quede esta estructura (la herramienta es
   quisquillosa con la ruta `cmdline-tools/latest/`):

```bash
mkdir -p $HOME/android-sdk/cmdline-tools
unzip commandlinetools-linux-*.zip -d $HOME/android-sdk/cmdline-tools
mv $HOME/android-sdk/cmdline-tools/cmdline-tools $HOME/android-sdk/cmdline-tools/latest
```

3. Exportar variables de entorno (añadir a `~/.bashrc` / `~/.zshrc` o al
   script de CI):

```bash
export ANDROID_HOME="$HOME/android-sdk"
export JAVA_HOME="/usr/lib/jvm/java-17-openjdk"   # ajustar según distro
export PATH="$JAVA_HOME/bin:$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$PATH"
```

   - `$JAVA_HOME/bin` en el PATH es necesario porque los `CMakeLists.txt` de
     `apps/*` invocan `jarsigner` directamente, sin ruta absoluta.
   - En Ubuntu, `JAVA_HOME` suele ser `/usr/lib/jvm/java-17-openjdk-amd64`.

4. Aceptar licencias e instalar los paquetes necesarios:

```bash
yes | sdkmanager --sdk_root="$ANDROID_HOME" --licenses
sdkmanager --sdk_root="$ANDROID_HOME" \
  "platform-tools" \
  "platforms;android-34" \
  "platforms;android-24" \
  "build-tools;34.0.0"
```

> Instala **ambos** platforms, no solo el 34: los `CMakeLists.txt` de `apps/*`
> reutilizan la misma variable `ANDROID_PLATFORM` tanto para el nivel de API
> del NDK (`-DANDROID_PLATFORM=android-24`, ver §6) como para el
> `android.jar` que usa `aapt` al empaquetar (`platforms/<ANDROID_PLATFORM>/android.jar`).
> Si solo instalas el 34, el empaquetado del APK falla con
> `Asset package include '.../platforms/android-24/android.jar' not found`
> aunque la compilación C++ haya ido bien.

   Comprueba versiones disponibles con `sdkmanager --sdk_root="$ANDROID_HOME" --list`
   si quieres una `build-tools`/`platform` más reciente que la de esta tabla.

## 4. Android NDK

```bash
sdkmanager --sdk_root="$ANDROID_HOME" --list | grep ndk
# elige la última versión r27.x (evita "rc" / release candidates)
sdkmanager --sdk_root="$ANDROID_HOME" "ndk;27.2.12479018"
```

```bash
export ANDROID_NDK="$ANDROID_HOME/ndk/27.2.12479018"   # ajusta al valor real instalado
```

(El número de patch exacto puede haber cambiado desde que se escribió esta
guía — usa el que te liste `sdkmanager --list`.)

## 5. OpenCV para Android (solo si vas a usar `EY3_WITH_CV=ON`)

EY3 espera los `.a` estáticos de OpenCV en una estructura concreta
(`staticlibs/<ABI>/`, `3rdparty/libs/<ABI>/`, `jni/include/`), que es la que
genera el propio build system de OpenCV al compilar para Android con
`CMAKE_INSTALL_PREFIX`. No hay un prebuilt oficial que encaje directamente —
hay que compilarlo desde fuente.

```bash
git clone --branch 4.10.0 --depth 1 https://github.com/opencv/opencv.git
export OPENCV_BUILD_OUTPUT="$HOME/opencv-android"
```

Repite este bloque **una vez por cada `ANDROID_ABI` que quieras soportar**
(normalmente basta con `arm64-v8a`; añade `x86_64` si vas a probar en
emulador), apuntando siempre al mismo `OPENCV_BUILD_OUTPUT`:

```bash
cmake opencv -B build-opencv-arm64 -G Ninja \
  -DBUILD_opencv_ittnotify=OFF -DBUILD_ITT=OFF -DCV_DISABLE_OPTIMIZATION=ON \
  -DWITH_TBB=ON -DANDROID_ARM_NEON=ON -DWITH_CUDA=OFF -DWITH_OPENCL=ON \
  -DWITH_OPENCLAMDFFT=OFF -DWITH_OPENCLAMDBLAS=OFF -DWITH_VA_INTEL=OFF \
  -DCPU_BASELINE_DISABLE=ON -DENABLE_SSE=OFF -DENABLE_SSE2=OFF \
  -DBUILD_TESTING=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_TESTS=OFF \
  -DCMAKE_BUILD_TYPE=RELEASE -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF \
  -DBUILD_opencv_apps=OFF -DBUILD_SHARED_LIBS=OFF -DOpenCV_STATIC=ON \
  -DWITH_1394=OFF -DWITH_ARITH_DEC=OFF -DWITH_ARITH_ENC=OFF -DWITH_CUBLAS=OFF \
  -DWITH_CUFFT=OFF -DWITH_FFMPEG=OFF -DWITH_GDAL=OFF -DWITH_GSTREAMER=OFF \
  -DWITH_GTK=OFF -DWITH_HALIDE=OFF -DWITH_JASPER=OFF -DWITH_NVCUVID=OFF \
  -DWITH_OPENEXR=OFF -DWITH_PROTOBUF=OFF -DWITH_PTHREADS_PF=OFF \
  -DWITH_QUIRC=OFF -DWITH_V4L=OFF -DWITH_WEBP=OFF \
  -DBUILD_LIST=core,features2d,flann,imgcodecs,imgproc,stitching \
  -DANDROID_NDK="$ANDROID_NDK" \
  -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" \
  -DANDROID_NATIVE_API_LEVEL=android-24 \
  -DBUILD_JAVA=OFF -DBUILD_ANDROID_EXAMPLES=OFF -DBUILD_ANDROID_PROJECTS=OFF \
  -DANDROID_STL=c++_shared \
  -DCMAKE_INSTALL_PREFIX:PATH="$OPENCV_BUILD_OUTPUT" \
  -DANDROID_ABI=arm64-v8a

ninja -C build-opencv-arm64
ninja -C build-opencv-arm64 install
```

Al terminar, `$OPENCV_BUILD_OUTPUT/sdk/native` es el valor que se pasa como
`-DOPENCV_DIR` al configurar EY3.

> Compilar OpenCV es, con diferencia, el paso más lento de todo el setup
> (puede tardar bastante más que instalar NDK/SDK). Tenlo en cuenta al
> planificar el tiempo de la primera ejecución del pipeline de CI — conviene
> cachear `$OPENCV_BUILD_OUTPUT` entre builds.

## 6. Compilar EY3 (validación en dos etapas)

### Etapa 1 — núcleo sin OpenCV (`EY3_WITH_CV=OFF`)

Valida toolchain + `Engine`/`Renderer` sin necesidad de haber compilado
OpenCV todavía:

```bash
cmake -B build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" \
  -DSDK_VERSION=34.0.0 \
  -DANDROID_PLATFORM=android-24 \
  -DANDROID_ABI=arm64-v8a \
  -DEY3_WITH_CV=OFF

ninja -C build ey3-triangle-unsigned
```

Si esto compila, el core (motor, render, EGL/GLES3, delta-time) es sano en
el entorno nuevo, sin que OpenCV interfiera en el diagnóstico.

### Etapa 2 — con visión por computador (`EY3_WITH_CV=ON`)

```bash
cmake -B build-cv -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" \
  -DSDK_VERSION=34.0.0 \
  -DANDROID_PLATFORM=android-24 \
  -DANDROID_ABI=arm64-v8a \
  -DOPENCV_DIR="$OPENCV_BUILD_OUTPUT/sdk/native" \
  -DEY3_WITH_CV=ON

ninja -C build-cv ey3-triangle-unsigned ey3-background-img-unsigned ey3-orb-demo-unsigned
```

Los targets `-unsigned` no requieren keystore y son suficientes para
validar que compila y empaqueta. Los targets `-apk` (firmados) y `-run`
(instalación vía `adb`) solo tienen sentido con un keystore configurado y,
para `-run`, un dispositivo/emulador conectado — algo que normalmente no
está disponible en un runner de CI sin hardware o emulador dedicado.

> Compilar y empaquetar no es lo mismo que validar en tiempo de ejecución.
> El fix del bug de `camera.cpp` y la cámara en general solo se pueden
> comprobar de verdad en un dispositivo real con cámara — el CI puede
> garantizar que el proyecto compila en cada cambio, pero no sustituye una
> prueba manual en dispositivo antes de dar por buena una release.

## 7. Variables de entorno — resumen para el pipeline

```bash
export JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64   # ruta Ubuntu
export ANDROID_HOME=$HOME/android-sdk
export ANDROID_NDK=$ANDROID_HOME/ndk/27.2.12479018
export OPENCV_BUILD_OUTPUT=$HOME/opencv-android        # solo si EY3_WITH_CV=ON
export PATH="$JAVA_HOME/bin:$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$PATH"
```

### 7.1 Trampa: las variables se congelan al configurar, no al compilar

Los `CMakeLists.txt` usan `$ENV{ANDROID_HOME}` y `$ENV{JAVA_HOME}`, que CMake lee
**en el momento de configurar** y escribe tal cual dentro de las reglas de
compilación. Si configuras desde una shell que no las tiene exportadas, no
falla nada en ese momento: las reglas quedan con rutas rotas y el error
aparece mucho después, al compilar.

Síntomas concretos vistos:

```
/bin/sh: línea 1: /build-tools/34.0.0/aapt: No existe el fichero o el directorio
cd .../build && /bin/keytool -genkeypair ...
```

Es decir, `${BUILD_TOOLS}` quedó como `/build-tools/34.0.0` (con `ANDROID_HOME`
vacío) y `keytool` como `/bin/keytool` (con `JAVA_HOME` vacío).

Dos consecuencias prácticas:

1. **Exportar las variables después no arregla nada.** Hay que volver a
   configurar desde una shell que ya las tenga:
   `ANDROID_HOME=... JAVA_HOME=... cmake -S . -B build`.
2. **Reconfigurar con valores distintos rompe la regla del keystore.** Al
   cambiar la línea de comandos, ninja considera que `release.keystore` está
   desactualizado y reintenta `keytool -genkeypair`, que falla con *"No se ha
   generado el par de claves, el alias \<alias\> ya existe"* y detiene la
   compilación. Se sale de ahí borrando el keystore (ojo: cambia la clave de
   firma) o, mejor, configurando siempre con las mismas variables.

Pendiente de revisar: resolver `aapt`/`zipalign`/`keytool` con `find_program()`
y guardarlos en la caché, y hacer que la regla del keystore no se reintente si
el fichero ya existe. Eso quitaría la dependencia del entorno en el momento de
configurar y ambos síntomas de arriba.
