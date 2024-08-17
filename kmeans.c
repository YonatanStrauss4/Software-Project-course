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



int main(int argc, char **argv)  /* main function */
{
    int i,j;
    double k;
    double iterations;
    int  n = 0;                 /* number of vectors */
    int dimension = 0;          /* dimension of vectors */ 
    const double EPS = 0.001;
    const double LARGE_VALUE = 1e30;
    int min_cluster;
    struct vector *head_vec;           /* declaring everything we need for reading the vectors from the file, to create 24 linked list of vectors */
    struct vector *curr_vec;
    struct cord *head_cord;
    struct cord *curr_cord;
    double element;
    char c;
    double *cord_centroid;                  /* declaring all the arrays we need for further calculations, initialization will happen next */
    double **centroids;
    double *cord_before_centroid;
    double **before_centroids;
    double *cord_clusters_sum;
    double **clusters_sum;
    double *centroids_difference;
    double *vector_clusters_d;
    double *centroids_updates;
    struct cord *next_cord;
    struct vector *next_vec;


    if(argc == 3){              /* checking if we got iter as an argument */
        sscanf(argv[1], "%lf", &k);
        sscanf(argv[2], "%lf", &iterations);
    }
    else{                      /* in this case, we didnt get iter as arguments */
        sscanf(argv[1], "%lf", &k);
        iterations = 200;
    }
    
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


    while (scanf("%lf%c", &element, &c) == 2)  /* reading the vectors and creating  the 24 linked list of vectors */
    {
        if(c == '\n')  /* finished reacing a single vector. we creat new vector and connecting all the relevant pointers of the 2d linked list */
        {
            curr_cord->value = element;
            curr_vec->cords = head_cord;
            curr_vec->next = malloc(sizeof(struct vector));
            if(curr_vec->next == NULL){
                free(head_cord);                       /* freeing all the memory that was allocated for the vectors, if we needed to exit from the program */
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
                printf("An Error Has Occurred\n");
                exit(1);
            }
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            head_cord = malloc(sizeof(struct cord));
            if(head_cord == NULL){
                free(head_cord);                       /* freeing all the memory that was allocated for the vectors, if we needed to exit from the program */
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
                printf("An Error Has Occurred\n");
                exit(1);
            }
            curr_cord = head_cord;
            curr_cord->next = NULL;
            n += 1;
            continue;
        }

        curr_cord->value = element;                    /* we are still reading coordinates from the same vector and connecting all the relevant pointers */
        curr_cord->next = malloc(sizeof(struct cord));
        if(curr_cord->next == NULL){
            free(head_cord);                       /* freeing all the memory that was allocated for the vectors, if we needed to exit from the program */
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
            printf("An Error Has Occurred\n");
            exit(1);
        }
        curr_cord = curr_cord->next;
        curr_cord->next = NULL;
    }

    free(head_cord);                  /* freeing the head_cord */



    curr_vec = head_vec;           /* this section is to get rid of the blank line that we counted as another vector */
    while(curr_vec->next->next != NULL){
        curr_vec = curr_vec->next;
    }
    next_vec = curr_vec->next;
    curr_vec->next = NULL;
    free(next_vec);
    
    if(fmod(k, 1.0) != 0.0 || k <=1 || k >= n){  /* checking conditions on k and iter */
        curr_vec = head_vec;                     /* freeing all the memory that was allocated for the vectors, if we needed to exit from the program */ 
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
        printf("Invalid number of clusters!\n");
        exit(1);
    }
    if(fmod(iterations, 1.0) != 0.0 || iterations <=1 || iterations >= 1000){
        curr_vec = head_vec;                     /* freeing all the memory that was allocated for the vectors, if we needed to exit from the program */ 
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
        printf("Invalid maximum iteration!\n");
        exit(1);
    }
    
    k = (int)k;                         /* changing k and iter to be integers for convenience */
    iterations = (int)iterations;
    dimension = count_it(head_vec->cords);  /* dimension of vectors */
    cord_centroid = calloc(k*dimension, sizeof(double));   /* allocating space for all the arrays we initialized  */
    if(cord_centroid == NULL){
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
        printf("An Error Has Occurred\n");
        exit(1);
    }
    centroids = calloc(k, sizeof(double *));
    if(centroids == NULL){                  
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
        free(cord_centroid);
        printf("An Error Has Occurred\n");
        exit(1);
    }
    for(i=0; i<k; i++){
        centroids[i] = cord_centroid + i*dimension;
    }

    cord_before_centroid = calloc(k*dimension, sizeof(double));
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
        free(cord_centroid);
        free(centroids);
        printf("An Error Has Occurred\n");
        exit(1);
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
        free(cord_centroid);
        free(centroids);
        free(cord_before_centroid);
        printf("An Error Has Occurred\n");
        exit(1);
    }
    for(i=0; i<k; i++){
        before_centroids[i] = cord_before_centroid + i*dimension;
    }

    cord_clusters_sum = calloc(k*dimension, sizeof(double));
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
        free(cord_centroid);
        free(centroids);
        free(cord_before_centroid);
        free(before_centroids);
        printf("An Error Has Occurred\n");
        exit(1);
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
        free(cord_centroid);
        free(centroids);
        free(cord_before_centroid);
        free(before_centroids);
        free(cord_clusters_sum);
        printf("An Error Has Occurred\n");
        exit(1);
    }
    for(i=0; i<k; i++){
        clusters_sum[i] = cord_clusters_sum + i*dimension;
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
        free(cord_centroid);
        free(centroids);
        free(cord_before_centroid);
        free(before_centroids);
        free(cord_clusters_sum);
        free(clusters_sum);
        printf("An Error Has Occurred\n");
        exit(1);
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
        free(cord_centroid);
        free(centroids);
        free(cord_before_centroid);
        free(before_centroids);
        free(cord_clusters_sum);
        free(clusters_sum);
        free(centroids_difference);
        printf("An Error Has Occurred\n");
        exit(1);
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
        free(cord_centroid);
        free(centroids);
        free(cord_before_centroid);
        free(before_centroids);
        free(cord_clusters_sum);
        free(clusters_sum);
        free(centroids_difference);
        free(vector_clusters_d);
        printf("An Error Has Occurred\n");
        exit(1);
    }

    curr_vec = head_vec;                     /* initializing the centroids to be the first k vectors */
    if(curr_vec == NULL){
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
        free(cord_centroid);
        free(centroids);
        free(cord_before_centroid);
        free(before_centroids);
        free(cord_clusters_sum);
        free(clusters_sum);
        free(centroids_difference);
        free(vector_clusters_d);
        free(centroids_updates);
        printf("An Error Has Occurred\n");
        exit(1);
    }

    
    curr_cord = head_vec->cords;
    if(curr_cord == NULL){
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
        free(cord_centroid);
        free(centroids);
        free(cord_before_centroid);
        free(before_centroids);
        free(cord_clusters_sum);
        free(clusters_sum);
        free(centroids_difference);
        free(vector_clusters_d);
        free(centroids_updates);
        printf("An Error Has Occurred\n");
        exit(1);
    }
    
    for(i=0; i<dimension; i++){
            centroids[0][i] = curr_cord->value;
            curr_cord = curr_cord->next;
        }
    
    for(i=1; i<k; i++){
        curr_vec = curr_vec->next;
        curr_cord = curr_vec->cords;
        for(j=0; j<dimension; j++){
            centroids[i][j] = curr_cord->value;
            curr_cord = curr_cord->next;
        }
    }
    
    while(iterations != 0 && convergence(centroids_difference, k, EPS) == 0){   /* main while loop for calculating the centroids */
        curr_vec = head_vec;
        while(curr_vec != NULL){       /* while loop for going over all the vectors */
            for(i=0; i<k; i++){    /*for each vector we calculate its eucllidean distance from every centroids */
                vector_clusters_d[i] = euclidean_distance(curr_vec->cords, centroids[i], dimension);
                
            }
            min_cluster = index_of_min(vector_clusters_d, k);   /* the minimum cluster is the closest cluster to the current vector */
            centroids_updates[min_cluster] += 1;                /* we added the vector to the cluster so we add 1 to the number of vectors in the relevant cluster */
            sum_two_vectors(curr_vec->cords, clusters_sum[min_cluster], dimension);     /* adding the vector to the cluster by summming it element-wise eith the othe vectors */
            curr_vec = curr_vec->next;   /* do it again for the next vector */
        }
        
        for(i=0; i<k; i++){                     /* building a copy of the current centroids for further calculations */
            for(j=0; j<dimension; j++){
                before_centroids[i][j] = centroids[i][j];
            }
        }
        for(i=0; i<k; i++){                     /* calculation the new centroids by dividing the current sum of each cluster with the number of vectors in the cluster */
            for(j=0; j<dimension; j++){
                if(centroids_updates[i] == 0.0){    /* preventing division by zero */
                    break;
                }
                centroids[i][j] = clusters_sum[i][j]/centroids_updates[i];
            }
        }
        
        for(i=0; i<k; i++){                     /* calculating the difference (euclidean distance) between the old centroids and the new centroids, for the convergence check */
            centroids_difference[i] = euclidean_distance_without_struct(centroids[i], before_centroids[i], dimension);
        }
        
        for(i=0; i<k; i++){            /* initializing the centroids update array for the next iterations */
            centroids_updates[i] = 0.0;
        }
        
        for(i=0; i<k; i++){            /* initializing the clusters sum array for the next iterations */
            for(j=0; j<dimension; j++){
                clusters_sum[i][j] = 0.0;
            }
        }
    
        iterations -= 1;  
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

    for(i=0; i<k; i++){                  /* printing the centroids in the correct format */
        for(j=0; j<dimension; j++){
            if(j != dimension-1){
                printf("%0.4f,", centroids[i][j]);
            }
            else{
                printf("%0.4f\n", centroids[i][j]);
            }
        }
    }

    free(centroids);     /* setting unused memory free */
    free(cord_centroid);
    return 0;
}
