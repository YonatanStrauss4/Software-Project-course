#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct cord             /* coordinates structure */
{
    double value;
    struct cord *next;
};
struct vector           /* vector structure */
{
    struct vector *next;
    struct cord *cords;
};

int count_it(struct cord *head)  /* a function to count the number of coordinates in a vector */
{
    int cnt = 0;
    for (;head !=NULL; head = head->next )
        ++cnt;
    return cnt;
}

double euclidean_distance(struct cord *v1, double *v2, int dim){  /* a function to calculate the euclidean distance between the vetor structure and an array of doubles */
    double dis = 0.0;
    int i;
    for(i=0; i<dim; i++){
        dis += (v1->value - v2[i])*(v1->value - v2[i]);
        v1 = v1->next;
    }
    return sqrt(dis);
}

int convergence(double *centroids_difference, int k, double EPS){  /* a function that checks if all the centroids converges */
    int i;
    for(i=0; i<k; i++){
        if(centroids_difference[i] >= EPS){
            return 0;
        }
    }
    return 1;
}

int index_of_min(double *arr, int k){  /* a function that returns the index of a minimum element in an array. if there are more then one minimum, it returns the index of the first minimum */
    int index = 0;
    double min = 1e30;
    int i;
    for(i=0; i<k; i++){
        if(arr[i] < min){
            min = arr[i];
            index = i;
        }
    }
    return index;
}

void sum_two_vectors(struct cord *v1, double *v2, int dim){  /* a function that returns a vector (an array of doubles) that is the sums of two vectors, element-wise (two arrays of doubles) */
    int i;
    for(i=0; i<dim; i++){
        v2[i] += v1->value;
        v1 = v1->next;
    }
}

double euclidean_distance_without_struct(double *v1, double *v2, int dim){  /* a function to calculate the euclidean distance between two arrays of doubles */
    double dis = 0;
    int i;
    for(i=0; i<dim; i++){
        dis += (v1[i] - v2[i])*(v1[i] - v2[i]);
    }
    return sqrt(dis);
}

double** kmeans(int k, int iter, double eps, int n, int dim, double **vectors, double **centroids)  /* main function */
{
    int i,j; 
    const double LARGE_VALUE = 1e30;
    int min_cluster;
    struct vector *head_vec;           /* declaring everything we need for reading the vectors from the file, to create 24 linked list of vectors */
    struct vector *curr_vec;
    struct cord *head_cord;
    struct cord *curr_cord;
    double *cord_before_centroid;
    double **before_centroids;
    double *cord_clusters_sum;
    double **clusters_sum;
    double *centroids_difference;
    double *vector_clusters_d;
    double *centroids_updates;
    struct cord *next_cord;
    struct vector *next_vec;

    head_cord = malloc(sizeof(struct cord));
    if(head_cord == NULL){
        printf("An Error Has Occurred\n");
        exit(1);
    }
    curr_cord = head_cord;
    curr_cord->next = NULL;

    head_vec = malloc(sizeof(struct vector));
    if(head_vec == NULL){
        free(head_cord);
        printf("An Error Has Occurred\n");
        exit(1);
    }
    curr_vec = head_vec;
    curr_vec->next = NULL;

    for(i=0; i<n; i++)                          /* reading the vectors and creating  the 2d linked list of vectors */
    {
        curr_vec->cords = curr_cord;
        for(j=0; j<dim; j++)                    /* reading the coordinates of a single vector */
        {
            curr_cord->value = vectors[i][j];
            if(j != dim-1){
                curr_cord->next = malloc(sizeof(struct cord));
                if(curr_cord->next == NULL){                 /* freeing all the memory that was allocated for the vectors, if we needed to exit from the program */
                    curr_vec = head_vec;                
                    while(curr_vec != NULL){
                        curr_cord = curr_vec->cords;
                        while(curr_cord != NULL){
                            next_cord = curr_cord->next;
                            free(curr_cord);
                        curr_cord = next_cord;
                        }
                        next_vec = curr_vec->next;
                        free(curr_vec);
                        curr_vec = next_vec;
                    }       
                    PyErr_SetString(PyExc_MemoryError, "");
                    return NULL;
                }
                curr_cord = curr_cord->next;
            }
            else{
                curr_cord->next = NULL;
            }

        }
        if(i != n-1){
            curr_vec->next = malloc(sizeof(struct vector));
            if(curr_vec->next == NULL)
            {                                 /* freeing all the memory that was allocated for the vectors, if we needed to exit from the program */
                curr_vec = head_vec;                
                while(curr_vec != NULL){
                    curr_cord = curr_vec->cords;
                    while(curr_cord != NULL){
                        next_cord = curr_cord->next;
                        free(curr_cord);
                        curr_cord = next_cord;
                    }
                    next_vec = curr_vec->next;
                    free(curr_vec);
                    curr_vec = next_vec;
                }
                PyErr_SetString(PyExc_MemoryError, "");
                return NULL;
            } 
            curr_vec = curr_vec->next;
            head_cord = malloc(sizeof(struct cord));
            if(head_cord == NULL){                       /* freeing all the memory that was allocated for the vectors, if we needed to exit from the program */
                curr_vec = head_vec;                
                while(curr_vec != NULL){
                    curr_cord = curr_vec->cords;
                    while(curr_cord != NULL){
                        next_cord = curr_cord->next;
                        free(curr_cord);
                    curr_cord = next_cord;
                    }
                    next_vec = curr_vec->next;
                    free(curr_vec);
                    curr_vec = next_vec;
                }
                PyErr_SetString(PyExc_MemoryError, "");
                return NULL;
            }
            curr_cord = head_cord;
            curr_vec->cords = curr_cord;
            curr_vec->next = NULL;
        }
        else{
            curr_vec->next = NULL;
        }

    }
    
    cord_before_centroid = calloc(k*dim, sizeof(double));
    if(cord_before_centroid == NULL){
        curr_vec = head_vec;               /* freeing all the memory that was allocated for the vectors, if we needed to exit from the program */ 
        while(curr_vec != NULL){
            curr_cord = curr_vec->cords;
            while(curr_cord != NULL){
                next_cord = curr_cord->next;
                free(curr_cord);
            curr_cord = next_cord;
            }
            next_vec = curr_vec->next;
            free(curr_vec);
            curr_vec = next_vec;
        }       
        PyErr_SetString(PyExc_MemoryError, "");
        return NULL;
    }
    before_centroids = calloc(k, sizeof(double *));
    if(before_centroids == NULL){
        curr_vec = head_vec;           /* freeing all the memory that was allocated for the vectors, if we needed to exit from the program */     
        while(curr_vec != NULL){
            curr_cord = curr_vec->cords;
            while(curr_cord != NULL){
                next_cord = curr_cord->next;
                free(curr_cord);
            curr_cord = next_cord;
            }
            next_vec = curr_vec->next;
            free(curr_vec);
            curr_vec = next_vec;
        }       
        free(cord_before_centroid);
        PyErr_SetString(PyExc_MemoryError, "");
        return NULL;
    }
    for(i=0; i<k; i++){
        before_centroids[i] = cord_before_centroid + i*dim;
    }

    cord_clusters_sum = calloc(k*dim, sizeof(double));
    if(cord_clusters_sum == NULL){                       
        curr_vec = head_vec;                /* freeing all the memory that was allocated for the vectors, if we needed to exit from the program */
        while(curr_vec != NULL){
            curr_cord = curr_vec->cords;
            while(curr_cord != NULL){
                next_cord = curr_cord->next;
                free(curr_cord);
            curr_cord = next_cord;
            }
            next_vec = curr_vec->next;
            free(curr_vec);
            curr_vec = next_vec;
        }       
        free(cord_before_centroid);
        free(before_centroids);
        PyErr_SetString(PyExc_MemoryError, "");
        return NULL;
    }
    
    clusters_sum = calloc(k, sizeof(double *));
    if(clusters_sum == NULL){                     
        curr_vec = head_vec;                /* freeing all the memory that was allocated for the vectors, if we needed to exit from the program */
        while(curr_vec != NULL){
            curr_cord = curr_vec->cords;
            while(curr_cord != NULL){
                next_cord = curr_cord->next;
                free(curr_cord);
            curr_cord = next_cord;
            }
            next_vec = curr_vec->next;
            free(curr_vec);
            curr_vec = next_vec;
        }      
        free(cord_before_centroid);
        free(before_centroids);
        free(cord_clusters_sum);
        PyErr_SetString(PyExc_MemoryError, "");
        return NULL;
    }
    for(i=0; i<k; i++){
        clusters_sum[i] = cord_clusters_sum + i*dim;
    }


    centroids_difference = (double*) malloc(k * sizeof(double));
    if(centroids_difference == NULL){
        curr_vec = head_vec;              /* freeing all the memory that was allocated for the vectors, if we needed to exit from the program */  
        while(curr_vec != NULL){
            curr_cord = curr_vec->cords;
            while(curr_cord != NULL){
                next_cord = curr_cord->next;
                free(curr_cord);
            curr_cord = next_cord;
            }
            next_vec = curr_vec->next;
            free(curr_vec);
            curr_vec = next_vec;
        }      
        free(cord_before_centroid);
        free(before_centroids);
        free(cord_clusters_sum);
        free(clusters_sum);
        PyErr_SetString(PyExc_MemoryError, "");
        return NULL;
    }
    for(i=0; i<k; i++){
        centroids_difference[i] = LARGE_VALUE;  /* we start with a very large value to make sure we meet the conditions of the main while loop */
    }
    
    vector_clusters_d = (double*) malloc(k * sizeof(double));
    if(vector_clusters_d == NULL){
        curr_vec = head_vec;              /* freeing all the memory that was allocated for the vectors, if we needed to exit from the program */  
        while(curr_vec != NULL){
            curr_cord = curr_vec->cords;
            while(curr_cord != NULL){
                next_cord = curr_cord->next;
                free(curr_cord);
            curr_cord = next_cord;
            }
            next_vec = curr_vec->next;
            free(curr_vec);
            curr_vec = next_vec;
        }       
        free(cord_before_centroid);
        free(before_centroids);
        free(cord_clusters_sum);
        free(clusters_sum);
        free(centroids_difference);
        PyErr_SetString(PyExc_MemoryError, "");
        return NULL;
    }
    centroids_updates = (double*) calloc(k, sizeof(double));
    if(centroids_updates == NULL){ 
        curr_vec = head_vec;              /* freeing all the memory that was allocated for the vectors, if we needed to exit from the program */  
        while(curr_vec != NULL){
            curr_cord = curr_vec->cords;
            while(curr_cord != NULL){
                next_cord = curr_cord->next;
                free(curr_cord);
            curr_cord = next_cord;
            }
            next_vec = curr_vec->next;
            free(curr_vec);
            curr_vec = next_vec;
        }       
        free(cord_before_centroid);
        free(before_centroids);
        free(cord_clusters_sum);
        free(clusters_sum);
        free(centroids_difference);
        free(vector_clusters_d);
        PyErr_SetString(PyExc_MemoryError, "");
        return NULL;
    }

    while(iter != 0 && convergence(centroids_difference, k, eps) == 0){   /* main while loop for calculating the centroids */
        curr_vec = head_vec;
        while(curr_vec != NULL){       /* while loop for going over all the vectors */
            for(i=0; i<k; i++){    /*for each vector we calculate its eucllidean distance from every centroids */
                vector_clusters_d[i] = euclidean_distance(curr_vec->cords, centroids[i], dim);
                
            }
            min_cluster = index_of_min(vector_clusters_d, k);   /* the minimum cluster is the closest cluster to the current vector */
            centroids_updates[min_cluster] += 1;                /* we added the vector to the cluster so we add 1 to the number of vectors in the relevant cluster */
            sum_two_vectors(curr_vec->cords, clusters_sum[min_cluster], dim);     /* adding the vector to the cluster by summming it element-wise eith the othe vectors */
            curr_vec = curr_vec->next;   /* do it again for the next vector */
        }
        

        for(i=0; i<k; i++){                     /* building a copy of the current centroids for further calculations */
            for(j=0; j<dim; j++){
                before_centroids[i][j] = centroids[i][j];
            }
        }
        for(i=0; i<k; i++){                     /* calculation the new centroids by dividing the current sum of each cluster with the number of vectors in the cluster */
            for(j=0; j<dim; j++){
                if(centroids_updates[i] == 0.0){    /* preventing division by zero */
                    break;
                }
                centroids[i][j] = clusters_sum[i][j]/centroids_updates[i];
            }
        }
    
        for(i=0; i<k; i++){                     /* calculating the difference (euclidean distance) between the old centroids and the new centroids, for the convergence check */
            centroids_difference[i] = euclidean_distance_without_struct(centroids[i], before_centroids[i], dim);
        }
        
        for(i=0; i<k; i++){            /* initializing the centroids update array for the next iterations */
            centroids_updates[i] = 0.0;
        }
        
        for(i=0; i<k; i++){            /* initializing the clusters sum array for the next iterations */
            for(j=0; j<dim; j++){
                clusters_sum[i][j] = 0.0;
            }
        }
    
        iter -= 1;  
    }

    curr_vec = head_vec;                /* freeing all the memory that was allocated for the vectors */
    while(curr_vec != NULL){
        curr_cord = curr_vec->cords;
        while(curr_cord != NULL){
            next_cord = curr_cord->next;
            free(curr_cord);
            curr_cord = next_cord;
        }
        next_vec = curr_vec->next;
        free(curr_vec);
        curr_vec = next_vec;
    }

    free(before_centroids);     /* setting unused memory free */
    free(cord_before_centroid);
    free(clusters_sum);
    free(cord_clusters_sum);
    free(centroids_difference);
    free(vector_clusters_d);
    free(centroids_updates);

    return centroids;
}


static PyObject* k_means(PyObject *self, PyObject *args){
    int k;
    int iter;
    double eps;
    int numVectors;
    int dim;
    PyObject *outerLst;
    PyObject *innerList;
    PyObject *item;
    PyObject *centroids;
    PyObject *cordCentroids;
    double *coordinates;                  
    double **vectors;
    double **initialCentroids;
    double *initialCordsCentroids;
    double **finalCentroids;

    if(!PyArg_ParseTuple(args, "iidiiOO", &k, &iter, &eps, &numVectors, &dim, &outerLst, &centroids)) {
        PyErr_SetString(PyExc_MemoryError, "");
        return NULL;
    }


    coordinates = calloc(numVectors*dim , sizeof(double)); 
    if(coordinates == NULL){
        PyErr_SetString(PyExc_MemoryError, "");
        return NULL;
    }
    vectors = calloc(numVectors, sizeof(double *));
    if(vectors == NULL){
        free(coordinates);
        PyErr_SetString(PyExc_MemoryError, "");
        return NULL;
    }
    
    for(int i=0; i<numVectors; i++){
        vectors[i] = coordinates + i * dim;
    }

    
    for (int i = 0; i < numVectors; i++) {          /*setting up vectors to be a double** containing the vectors that were passed from kmeans_pp (from both input files after inner join)*/
        innerList = PyList_GetItem(outerLst, i); 
        for (int j = 0; j < dim; j++) {
            item = PyList_GetItem(innerList, j);
            vectors[i][j] = PyFloat_AsDouble(item);
        }
    }

    initialCordsCentroids = calloc(k*dim , sizeof(double));   
    if(initialCordsCentroids == NULL){
        free(coordinates);
        free(vectors);
        PyErr_SetString(PyExc_MemoryError, "");
        return NULL;
    }

    initialCentroids = calloc(k, sizeof(double *));
    if(initialCentroids == NULL){
        free(coordinates);
        free(vectors);
        free(initialCordsCentroids);
        PyErr_SetString(PyExc_MemoryError, "");
        return NULL;
    }


    for(int i=0; i<k; i++){
        initialCentroids[i] = initialCordsCentroids + i * dim;
    }

    for(int i=0; i<k; i++){             /*setting up initialCentroids to be a double** containing the initial centroids chosen randomly that were passed from kmeans_pp*/
        cordCentroids = PyList_GetItem(centroids,i);
        for(int j=0; j<dim; j++){
            item = PyList_GetItem(cordCentroids,j);
            initialCentroids[i][j] = PyFloat_AsDouble(item);
        }
    }

    finalCentroids = kmeans(k, iter, eps, numVectors, dim, vectors, initialCentroids);      /*calling the kmeans algo from HW1 modified according to the instructions*/

    PyObject* ret = PyList_New(k);      /*converting double **finalCentroids to a 2D python list containing the final centroids to be passed back to kmeans_pp*/
    for (int i = 0; i < k; i++) {
        PyObject* ret_inner = PyList_New(dim); 
        for (int j = 0; j < dim; j++) {
            
            item = Py_BuildValue("d", finalCentroids[i][j]); 
            PyList_SET_ITEM(ret_inner, j, item); 
        }
        PyList_SET_ITEM(ret, i, ret_inner); 
    }
    
    free(coordinates);      /*freeing memory that we are done using*/
    free(vectors);
    free(initialCordsCentroids);
    free(initialCentroids);

    return ret;
   
}

static PyMethodDef kmeansMethods[] = {
    {"fit",                   
      (PyCFunction) k_means,
      METH_VARARGS,         
      PyDoc_STR("fit function excpects as input: 1<k<number of vectors, 1<number of iterations<1000, epsilon>=0, number of vectors, vectors (python list), selected centroids (python list)")},
    {NULL, NULL, 0, NULL}     
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp", 
    NULL, 
    -1,  
    kmeansMethods 
};

PyMODINIT_FUNC PyInit_mykmeanssp(void)
{
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (!m) {
        return NULL;
    }
    return m;
}
