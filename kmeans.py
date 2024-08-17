import sys

def kmeans():
    try:  # handling errors in the kmeans function
        n_args = len(sys.argv)   # check number of arguments
        if n_args == 4:                           # extract arguments and put iterations as 200 if not provided
            input_data = open(sys.argv[3], "r")
            k = float(sys.argv[1])
            iterations = float(sys.argv[2])
        elif n_args == 3:
            input_data = open(sys.argv[2], "r")
            k = float(sys.argv[1])
            iterations = 200.0        
        else:
            print("An Error Has Occurred")
            exit(1)

        line = input_data.readline()
        vectors = []
        while line:                                              # read the whole file line by line and put it in the vectors array as arrays of vector elements
            vectors.append([float(i) for i in line.split(',')])
            line = input_data.readline()
        input_data.close()
        n = len(vectors)
        if k%1 != 0.0 or k <=1 or k >= n:                                 # checking if arguments are valid
            print("Invalid number of clusters!")
            exit(1)
        if iterations%1 != 0.0 or iterations <=1 or iterations >= 1000:
            print("Invalid maximum iteration!")
            exit(1)
        k = int(k)
        iterations  = int(iterations)

        centroids = [[] for _ in range(k)]                             # initializing all the arrays we need for further calculations
        clusters_sum = [[0]*(len(vectors[0])+1) for _ in range(k)]    
        centroids_difference = [float('inf') for _ in range(k)]        # we start with infinity to make sure we meet the conditions of the main while loop 
        vector_clusters_d = [0 for _ in range(k)]
        centroids_updates = [0 for _ in range(k)]
        EPS = 0.001
        for i in range(k):                                             # initializing the centroids to be the first k vectors 
            centroids[i] = vectors[i].copy()
        while iterations != 0 and not all([x < EPS for x in centroids_difference]):   # main while loop for calculating the centroids
            for i in range(n):                 # for loop for going over all the vectors
                current_vector = vectors[i]   
                for j in range(k):             # for each vector we calculate its eucllidean distance from every centroids
                    vector_clusters_d[j] = euclidean_distance(centroids[j], current_vector)
                min_cluster = vector_clusters_d.index(min(vector_clusters_d))   # the minimum cluster is the closest cluster to the current vector
                centroids_updates[min_cluster] += 1                             # we added the vector to the cluster so we add 1 to the number of vectors in the relevant cluster
                clusters_sum[min_cluster] = [a_i + b_i for a_i, b_i in zip(clusters_sum[min_cluster], current_vector)]  # adding the vector to the cluster by summming it element-wise eith the othe vectors
            before_centroids = [x[:] for x in centroids]       # building a copy of the current centroids for further calculations
            for i in range(k):                                 # calculation the new centroids by dividing the current sum of each cluster with the number of vectors in the cluster
                for j in range(len(centroids[0])):
                    if centroids_updates[i] == 0.0:            # preventing division by zero
                        break
                    centroids[i][j] = clusters_sum[i][j]/centroids_updates[i]
            centroids_difference = [euclidean_distance(a_i, b_i) for a_i, b_i in zip(centroids, before_centroids)]    # calculating the difference (euclidean distance) between the old centroids and the new centroids, for the convergence check
            centroids_updates = [0 for _ in range(k)]                             # initializing the centroids update array for the next iterations
            clusters_sum = [[0]*(len(vectors[0])+1) for _ in range(k)]            # initializing the clusters sum array for the next iterations
            iterations -= 1
        centroids = [['%.4f' % elem for elem in nested] for nested in centroids]  # setting and printing the centroids in the correct format
        for i in range(k):
            print(','.join(centroids[i]))
        return
    except Exception as e:               # handling errors in the kmeans function
        print("An Error Has Occurred")
    finally:                             # closing the input file in case the error happens before we close it in the kmeans function
        input_data.close()

def euclidean_distance(v, w):  # a function to calculate the euclidean distance between two vectors
    d = 0
    for i in range(len(v)):
        d += (v[i] - w[i]) ** 2
    return d ** 0.5

kmeans()   # call the kmenas function




