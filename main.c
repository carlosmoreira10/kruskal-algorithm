#include <stdio.h>
#include <stdlib.h>

struct grafo {
  int eh_ponderado;
  int nro_vertices;
  int grau_max;
  int **arestas;
  float **pesos;
  int *grau;
};

typedef struct grafo Grafo;

Grafo *cria_Grafo(int nro_vertices, int grau_max, int eh_ponderado) {
  Grafo *gr;
  gr = (Grafo *)malloc(sizeof(struct grafo));
  if (gr != NULL) {
    int i;
    gr->nro_vertices = nro_vertices;
    gr->grau_max = grau_max;
    gr->eh_ponderado = (eh_ponderado != 0) ? 1 : 0;
    gr->grau = (int *)calloc(nro_vertices, sizeof(int));

    gr->arestas = (int **)malloc(nro_vertices * sizeof(int *));
    for (i = 0; i < nro_vertices; i++)
      gr->arestas[i] = (int *)malloc(grau_max * sizeof(int));

    if (gr->eh_ponderado) {
      gr->pesos = (float **)malloc(nro_vertices * sizeof(float *));
      for (i = 0; i < nro_vertices; i++)
        gr->pesos[i] = (float *)malloc(grau_max * sizeof(float));
    }
  }
  return gr;
}

void libera_Grafo(Grafo *gr) {
  if (gr != NULL) {
    int i;
    for (i = 0; i < gr->nro_vertices; i++)
      free(gr->arestas[i]);
    free(gr->arestas);

    if (gr->eh_ponderado) {
      for (i = 0; i < gr->nro_vertices; i++)
        free(gr->pesos[i]);
      free(gr->pesos);
    }
    free(gr->grau);
    free(gr);
  }
}

int insereAresta(Grafo *gr, int orig, int dest, int eh_digrafo, float peso) {
  if (gr == NULL)
    return 0;
  if (orig < 0 || orig >= gr->nro_vertices)
    return 0;
  if (dest < 0 || dest >= gr->nro_vertices)
    return 0;

  gr->arestas[orig][gr->grau[orig]] = dest;
  if (gr->eh_ponderado)
    gr->pesos[orig][gr->grau[orig]] = peso;
  gr->grau[orig]++;

  if (eh_digrafo == 0)
    insereAresta(gr, dest, orig, 1, peso);
  return 1;
}

int removeAresta(Grafo *gr, int orig, int dest, int eh_digrafo) {
  if (gr == NULL)
    return 0;
  if (orig < 0 || orig >= gr->nro_vertices)
    return 0;
  if (dest < 0 || dest >= gr->nro_vertices)
    return 0;

  int i = 0;
  while (i < gr->grau[orig] && gr->arestas[orig][i] != dest)
    i++;
  if (i == gr->grau[orig]) // elemento nao encontrado
    return 0;
  gr->grau[orig]--;
  gr->arestas[orig][i] = gr->arestas[orig][gr->grau[orig]];
  if (gr->eh_ponderado)
    gr->pesos[orig][i] = gr->pesos[orig][gr->grau[orig]];
  if (eh_digrafo == 0)
    removeAresta(gr, dest, orig, 1);
  return 1;
}

void imprime_Grafo(Grafo *gr) {
  if (gr == NULL)
    return;

  int i, j;
  for (i = 0; i < gr->nro_vertices; i++) {
    printf("%d: ", i);
    for (j = 0; j < gr->grau[i]; j++) {
      if (gr->eh_ponderado)
        printf("%d(%.2f), ", gr->arestas[i][j], gr->pesos[i][j]);
      else
        printf("%d, ", gr->arestas[i][j]);
    }
    printf("\n");
  }
}

int procuraMenorDistancia(float *dist, int *visitado, int NV) {
  int i, menor = -1, primeiro = 1;
  for (i = 0; i < NV; i++) {
    if (dist[i] >= 0 && visitado[i] == 0) {
      if (primeiro) {
        menor = i;
        primeiro = 0;
      } else {
        if (dist[menor] > dist[i])
          menor = i;
      }
    }
  }
  return menor;
}

void menorCaminho_Grafo(Grafo *gr, int ini, int *ant, float *dist) {
  int i, cont, NV, ind, *visitado, vert;
  cont = NV = gr->nro_vertices;
  visitado = (int *)malloc(NV * sizeof(int));
  for (i = 0; i < NV; i++) {
    ant[i] = -1;
    dist[i] = -1;
    visitado[i] = 0;
  }
  dist[ini] = 0;
  while (cont > 0) {
    vert = procuraMenorDistancia(dist, visitado, NV);
    if (vert == -1)
      break;

    visitado[vert] = 1;
    cont--;
    for (i = 0; i < gr->grau[vert]; i++) {
      ind = gr->arestas[vert][i];
      if (dist[ind] < 0) {
        dist[ind] = dist[vert] + 1;
        ant[ind] = vert;
      } else {
        if (dist[ind] > dist[vert] + 1) {
          dist[ind] = dist[vert] + 1;
          ant[ind] = vert;
        }
      }
    }
  }

  free(visitado);
}

void buscaProfundidade(Grafo *gr, int ini, int *visitado, int cont) {
  int i;
  visitado[ini] = cont;
  for (i = 0; i < gr->grau[ini]; i++) {
    if (!visitado[gr->arestas[ini][i]])
      buscaProfundidade(gr, gr->arestas[ini][i], visitado, cont + 1);
  }
}

void buscaProfundidade_Grafo(Grafo *gr, int ini, int *visitado) {
  int i, cont = 1;
  for (i = 0; i < gr->nro_vertices; i++)
    visitado[i] = 0;
  buscaProfundidade(gr, ini, visitado, cont);

  for (i = 0; i < gr->nro_vertices; i++)
    printf("%d -> %d\n", i, visitado[i]);
}

void buscaLargura_Grafo(Grafo *gr, int ini, int *visitado) {
  int i, vert, NV, cont = 1;
  int *fila, IF = 0, FF = 0;
  for (i = 0; i < gr->nro_vertices; i++)
    visitado[i] = 0;

  NV = gr->nro_vertices;
  fila = (int *)malloc(NV * sizeof(int));
  FF++;
  fila[FF] = ini;
  visitado[ini] = cont;

  while (IF != FF) {
    IF = (IF + 1) % NV;
    vert = fila[IF];
    cont++;
    for (i = 0; i < gr->grau[vert]; i++) {
      if (!visitado[gr->arestas[vert][i]]) {
        FF = (FF + 1) % NV;
        fila[FF] = gr->arestas[vert][i];
        visitado[gr->arestas[vert][i]] = cont;
      }
    }
  }
  free(fila);
  for (i = 0; i < gr->nro_vertices; i++)
    printf("%d -> %d\n", i, visitado[i]);
}

void algKruskal(Grafo *gr, int orig, int *pai) {
  int i, j, dest, primeiro, NV = gr->nro_vertices;
  double menorPeso;
  int *arv = (int *)malloc(NV * sizeof(int));
  for (i = 0; i < NV; i++) {
    arv[i] = i;
    pai[i] = -1; // sem pai
  }
  pai[orig] = orig;
  while (1) {
    primeiro = 1;
    for (i = 0; i < NV; i++) {            // percorre vértices
      for (j = 0; j < gr->grau[i]; j++) { // percorre arestas
        if (arv[i] != arv[gr->arestas[i][j]]) {
          if (primeiro) {
            menorPeso = gr->pesos[i][j];
            orig = i;
            dest = gr->arestas[i][j];
            primeiro = 0;
          } else {
            if (menorPeso > gr->pesos[i][j]) {
              menorPeso = gr->pesos[i][j];
              orig = i;
              dest = gr->arestas[i][j];
            }
          }
        }
      }
    }
      if (primeiro == 1)
        break;
      if (pai[orig] == -1)
        pai[orig] = dest;
      else
        pai[dest] = orig;

      for (i = 0; i < NV; i++) {
        if (arv[i] == arv[dest]) {
          arv[i] = arv[orig];
        }
      }
    }
}

int main() {
  int eh_digrafo = 0;
  Grafo *gr = cria_Grafo(6, 6, 1);

  insereAresta(gr, 0, 1, eh_digrafo, 6);
  insereAresta(gr, 0, 2, eh_digrafo, 1);
  insereAresta(gr, 0, 3, eh_digrafo, 5);
  insereAresta(gr, 1, 2, eh_digrafo, 2);
  insereAresta(gr, 1, 4, eh_digrafo, 5);
  insereAresta(gr, 2, 3, eh_digrafo, 2);
  insereAresta(gr, 2, 4, eh_digrafo, 6);
  insereAresta(gr, 2, 5, eh_digrafo, 4);
  insereAresta(gr, 3, 5, eh_digrafo, 4);
  insereAresta(gr, 4, 5, eh_digrafo, 3);

  int i, pai[6];

  algKruskal(gr, 0, pai);

  printf("arestas da árvore geradora mínima:\n");
  for (i = 1; i < gr->nro_vertices; i++) {
    printf("(%d, %d)\n", pai[i], i);
  }

  libera_Grafo(gr);

  return 0;
}
