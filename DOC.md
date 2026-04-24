# Documentación de Algoritmos de Pathfinding

Esta guía explica cómo implementar un nuevo algoritmo de búsqueda de caminos en este proyecto.

## Estructura de Entrada (Input)

Cualquier algoritmo debe implementar la interfaz `IPathfinder` y su método `solve(GraphData &graph)`. La estructura `GraphData` contiene:

- `nodes`: Un vector de estructuras `Node`. Cada nodo tiene un `id`, coordenadas `x, y` (espacio OpenGL [-1, 1]) y un `state`.
- `adj`: Una lista de adyacencia donde `adj[i]` contiene los vecinos del nodo `i` (índice del vecino y peso de la arista).
- `startId`: El índice del nodo de inicio.
- `endId`: El índice del nodo de destino.

## Estructura de Salida (Output)

Para que el visualizador muestre el progreso y el resultado, el algoritmo debe actualizar la propiedad `state` de los nodos en `graph.nodes`:

- **Nodos Visitados (Cerrados):** Cambiar `state` a `NodeState::CLOSED`. Se verán de color púrpura.
- **Nodos en la Frontera (Abiertos):** Cambiar `state` a `NodeState::OPEN`. Se verán de color azul claro.
- **Camino Final:** Una vez encontrado el destino, reconstruir el camino y cambiar el `state` de los nodos intermedios a `NodeState::PATH`. Se verán de color amarillo.

> **Nota:** No modifiques el `state` de los nodos que sean `NodeState::START` o `NodeState::END` para mantener la visibilidad de los puntos de origen y destino (color verde y rojo respectivamente).

## Ejemplo de Implementación

```cpp
class MyNewAlgo : public IPathfinder {
public:
    void solve(GraphData &graph) override {
        // 1. Acceder a graph.nodes[graph.startId]
        // 2. Usar graph.adj para explorar vecinos
        // 3. Marcar nodos como OPEN/CLOSED mientras buscas
        // 4. Marcar camino final como PATH
    }
};
```
