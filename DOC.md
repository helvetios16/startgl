# Documentación de Algoritmos de Pathfinding

Esta guía explica cómo implementar un nuevo algoritmo de búsqueda de caminos en este proyecto.

## Estructura de Entrada (Input)

Cualquier algoritmo debe implementar la interfaz `IPathfinder` y su método `solve(GraphData &graph)`. La estructura `GraphData` contiene:

- `nodes`: Un vector de estructuras `Node`. Cada nodo tiene un `id`, coordenadas `x, y` (espacio OpenGL [-1, 1]), un `state` y un `parentId`.
- `adj`: Una lista de adyacencia donde `adj[i]` contiene los vecinos del nodo `i` (índice del vecino y peso de la arista).
- `startId`: El índice del nodo de inicio.
- `endId`: El índice del nodo de destino.

## Estructura de Salida (Output)

Para que el visualizador muestre el progreso y el resultado (incluyendo las conexiones pintadas), el algoritmo debe actualizar:

1.  **`node.state`**: Determina el color del punto (nodo).
    - **Nodos Visitados (Cerrados):** `NodeState::CLOSED` (Púrpura).
    - **Nodos en la Frontera (Abiertos):** `NodeState::OPEN` (Azul claro).
    - **Camino Final:** `NodeState::PATH` (Amarillo).

2.  **`node.parentId`**: Determina qué conexión se pinta y de qué color.
    - Al descubrir un nodo `v` desde un nodo `u`, debes asignar `graph.nodes[v].parentId = u;`.
    - El visualizador pintará la línea entre `u` y `v` usando el color del estado de `v`.
    - Si `parentId` es `-1`, la conexión se mantendrá en gris oscuro.

> **Nota:** No modifiques el `state` de los nodos que sean `NodeState::START` o `NodeState::END` para mantener la visibilidad de los puntos de origen y destino (color verde y rojo respectivamente).

## Ejemplo de Implementación

```cpp
class MyNewAlgo : public IPathfinder {
public:
    void solve(GraphData &graph) override {
        // 1. Al explorar:
        // graph.nodes[vecino].parentId = actual;
        // graph.nodes[vecino].state = NodeState::OPEN;

        // 2. Al procesar:
        // graph.nodes[actual].state = NodeState::CLOSED;

        // 3. Al encontrar el camino final:
        // Reconstruir usando parentId y marcar como NodeState::PATH
    }
};
```
