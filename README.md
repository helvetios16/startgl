# StartGL - Pathfinding Visualizer

Un visualizador de algoritmos de búsqueda de caminos desarrollado en C++ utilizando OpenGL y GLFW.

## Tecnologías Utilizadas

- **Lenguaje:** C++11
- **Gráficos:** [OpenGL](https://www.opengl.org/) para el renderizado de la interfaz y el grafo.
- **Ventaneo y Eventos:** [GLFW](https://www.glfw.org/) para la gestión de la ventana y entradas del usuario.
- **Sistema de Construcción:** [GNU Make](https://www.gnu.org/software/make/) para automatizar la compilación.

## Algoritmos Implementados

- A\* (A-Star)
- Dijkstra
- BFS (Breadth-First Search)
- DFS (Depth-First Search)

## Requisitos Previos

Librerías de desarrollo de OpenGL y GLFW:

### Fedora

```bash
sudo dnf install glfw-devel mesa-libGL-devel
```

### Linux (Debian/Ubuntu)

```bash
sudo apt-get install libglfw3-dev libgl1-mesa-dev
```

## Uso del Makefile

El proyecto incluye un `makefile` para facilitar la gestión del ciclo de vida del software:

- **Compilar el proyecto:**

  ```bash
  make
  ```

  Esto generará el ejecutable llamado `pathnode`.

- **Ejecutar la aplicación:**

  ```bash
  make run
  ```

  Compila (si es necesario) e inicia el visualizador automáticamente.

- **Limpiar archivos generados:**
  ```bash
  make clean
  ```
  Elimina el ejecutable `pathnode` y otros archivos temporales de compilación.
