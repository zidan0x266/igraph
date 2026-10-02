/* Tests for the in-house spatial edge-betweenness extension. */
#include <igraph.h>
#include <math.h>
#include "test_utilities.h"

static igraph_error_t cancel_after_source(const char *message, igraph_real_t percent, void *data) {
    (void) message;
    (void) data;
    return percent > 0 ? IGRAPH_INTERRUPTED : IGRAPH_SUCCESS;
}

int main(void) {
    igraph_t graph;
    igraph_matrix_t coords;
    igraph_vector_t box, gebc, obc, dbc, stock;
    igraph_vector_init_real(&box, 3, 10.0, 8.0, 6.0);
    igraph_vector_init(&gebc, 0);
    igraph_vector_init(&obc, 0);
    igraph_vector_init(&dbc, 0);
    igraph_vector_init(&stock, 0);
    igraph_matrix_init(&coords, 4, 3);
    MATRIX(coords, 0, 0) = 9;
    MATRIX(coords, 1, 0) = 1;
    MATRIX(coords, 2, 0) = 3;
    /* Edge IDs deliberately differ from vertex order; vertex 3 is isolated. */
    igraph_small(&graph, 4, IGRAPH_UNDIRECTED, 2, 1, 1, 0, -1);
    IGRAPH_ASSERT(igraph_edge_betweenness_spatial(&graph, &coords, &box, 0, &gebc, &obc, &dbc) == IGRAPH_SUCCESS);
    igraph_edge_betweenness(&graph, NULL, &stock, igraph_ess_all(IGRAPH_EDGEORDER_ID), false, false);
    IGRAPH_ASSERT(igraph_vector_all_e(&gebc, &stock));
    IGRAPH_ASSERT(igraph_vector_all_e(&gebc, &obc));
    IGRAPH_ASSERT(VECTOR(gebc)[0] == 2 && VECTOR(gebc)[1] == 2);
    IGRAPH_ASSERT(fabs(VECTOR(dbc)[0] - 6.0) < 1e-14);
    IGRAPH_ASSERT(fabs(VECTOR(dbc)[1] - 6.0) < 1e-14);
    IGRAPH_ASSERT(igraph_edge_betweenness_spatial(&graph, &coords, &box, 1, &gebc, &obc, &dbc) == IGRAPH_SUCCESS);
    IGRAPH_ASSERT(igraph_vector_sum(&obc) == 0);
    IGRAPH_ASSERT(fabs(VECTOR(dbc)[0] - 6.0) < 1e-14);

    igraph_set_error_handler(igraph_error_handler_ignore);
    igraph_set_progress_handler(cancel_after_source);
    IGRAPH_ASSERT(igraph_edge_betweenness_spatial(&graph, &coords, &box, 0, &gebc, &obc, &dbc) == IGRAPH_INTERRUPTED);
    VERIFY_FINALLY_STACK();
    igraph_set_progress_handler(NULL);
    IGRAPH_ASSERT(igraph_edge_betweenness_spatial(&graph, &coords, &box, 0, &gebc, &obc, &dbc) == IGRAPH_SUCCESS);
    IGRAPH_ASSERT(igraph_edge_betweenness_spatial(&graph, &coords, &box, 3, &gebc, &obc, &dbc) == IGRAPH_EINVAL);
    IGRAPH_ASSERT(igraph_edge_betweenness_spatial(&graph, &coords, &box, 0, &gebc, &gebc, &dbc) == IGRAPH_EINVAL);
    VECTOR(box)[0] = 0;
    IGRAPH_ASSERT(igraph_edge_betweenness_spatial(&graph, &coords, &box, 0, &gebc, &obc, &dbc) == IGRAPH_EINVAL);
    VECTOR(box)[0] = 10;
    MATRIX(coords, 0, 0) = IGRAPH_NAN;
    IGRAPH_ASSERT(igraph_edge_betweenness_spatial(&graph, &coords, &box, 0, &gebc, &obc, &dbc) == IGRAPH_EINVAL);
    igraph_destroy(&graph);

    igraph_empty(&graph, 0, IGRAPH_UNDIRECTED);
    igraph_matrix_resize(&coords, 0, 3);
    IGRAPH_ASSERT(igraph_edge_betweenness_spatial(&graph, &coords, &box, 0, &gebc, &obc, &dbc) == IGRAPH_SUCCESS);
    IGRAPH_ASSERT(igraph_vector_size(&gebc) == 0);
    igraph_destroy(&graph);
    igraph_empty(&graph, 0, IGRAPH_DIRECTED);
    IGRAPH_ASSERT(igraph_edge_betweenness_spatial(&graph, &coords, &box, 0, &gebc, &obc, &dbc) == IGRAPH_EINVAL);
    igraph_destroy(&graph);
    igraph_matrix_destroy(&coords);
    igraph_vector_destroy(&box);
    igraph_vector_destroy(&gebc);
    igraph_vector_destroy(&obc);
    igraph_vector_destroy(&dbc);
    igraph_vector_destroy(&stock);
    VERIFY_FINALLY_STACK();
    return 0;
}
