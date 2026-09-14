# OpenGL minimal pour macOS

C++17, CMake, GLFW et OpenGL 3.3 core : une fenêtre avec un triangle orange.
Échap ferme la fenêtre. Les shaders sont inclus dans main.cpp.

## Prérequis

Installer les outils Apple si nécessaire : `xcode-select --install`.
Avec Homebrew installé :

```sh
brew install cmake glfw
```

## Compiler et lancer

Depuis ce dossier :

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH="$(brew --prefix)"
cmake --build build
./build/opengl_mac
```

Ce projet utilise directement les en-têtes OpenGL de macOS, sans GLAD ni GLEW.
Utiliser un Homebrew de la même architecture que le compilateur (Apple Silicon
ou Intel). Le viewport utilise les dimensions du framebuffer pour Retina.

Référence sur les contextes macOS : https://www.glfw.org/docs/latest/compat_guide.html
