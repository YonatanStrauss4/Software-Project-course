import sys
import pandas as pd
import numpy as np
import mykmeanssp as mk

def kmeans_pp():
    try:
        
        np.random.seed(1234)
        n = len(sys.argv)
        if ((n != 5) and (n != 6)):     # incorrect number of arguments
            print("An Error Has Occurred")
            exit(1)

        # correct number of arguments (5 or 6 args)
        # checking if arguments are valid and setting them
        try:
            k = float(sys.argv[1])
        except ValueError:
            print("Invalid number of clusters!")
            exit(1)

        if n == 6:
            try:
                iter = float(sys.argv[2])
            except ValueError:
                print("Invalid maximum iteration!")
                exit(1)
        else: iter = 300.0
        
        if n == 6:
            try:
                eps = float(sys.argv[3])
            except ValueError:
                print("Invalid epsilon!")
                exit(1)
        else: 
            try:
                eps = float(sys.argv[2])
            except ValueError:
                print("Invalid epsilon!")
                exit(1)
       
        file1 = open(sys.argv[4], 'r') if n == 6 else open(sys.argv[3], 'r')
        file2 = open(sys.argv[5], 'r') if n == 6 else open(sys.argv[4], 'r')
        
        # Reading files into DataFrames

        df1 = pd.read_csv(file1, header=None)
        df2 = pd.read_csv(file2, header=None)

        if (df1.shape[0] != df2.shape[0]):      # if input files don't contain the same number of vectors 
            print("An Error Has Occurred")
            exit(1)

        num_vectors = df1.shape[0]    # num_vectors is the number of vectors in both files
        df1.columns = ['key'] + [f'val1_{i}' for i in range(1, len(df1.columns))]
        df2.columns = ['key'] + [f'val2_{i}' for i in range(1, len(df2.columns))]
        
        # preforming inner join and sorting by keys
        
        combined_df = pd.merge(df1, df2, on='key', how='inner')
        combined_df = combined_df.sort_values(by='key')
        
        dim = combined_df.shape[1] - 1      # dim is the dimension of the vectors 
        
        if ((k % 1.0 != 0.0) or k <= 1 or k >= num_vectors):     # if k argument is invalid
            print("Invalid number of clusters!")
            exit(1)

        else: 
            k = int(k)

        if ((iter % 1.0 != 0.0) or iter <= 1 or iter >= 1000):    # if iter argument is invalid
            print("Invalid maximum iteration!")
            exit(1)

        else: 
            iter = int(iter)

        if (eps < 0):
            print("Invalid epsilon!")
            exit(1)
    
        # variables are set up and arguments are valid
        # adding the first centroid to centroids array
        
        vectors = combined_df.drop(columns=['key']).to_numpy()
        indices = []
        centroids = np.zeros((k,dim))
        
        random_key = np.random.choice(combined_df['key'])       # choosing first centroid by random
        indices.append(int(random_key))

        centroids[0] = combined_df[combined_df['key'] == random_key].drop('key', axis = 1).values.flatten()
        
        for i in range(1,k):

            distances = np.zeros(num_vectors)
            
            for j in range(vectors.shape[0]):

                min_d = min(np.linalg.norm(vectors[j] - c) for c in centroids[:i])
                distances[j] = min_d

            # choosing next centoid by random using weighted probability distribution
            probabilities = distances / distances.sum()
            random_key = np.random.choice(num_vectors, p = probabilities)

            # adding new centroid to centroids array, and saving it's key in indices
            indices.append(random_key)
            centroids[i] = combined_df[combined_df['key'] == random_key].drop(columns = ['key']).to_numpy()
            
    
        # centroids are ready to pass to HW1 code
        final_centroids = [[]] 
        vectors_lst = vectors.tolist()
        centroids_lst = centroids.tolist()
        final_centroids = mk.fit(k, iter, eps, num_vectors, dim, vectors_lst, centroids_lst)
        
        # printing output as instructed
        number_strings = [str(number) for number in indices]
        output_indices = ','.join(number_strings)
        
        print(output_indices)
        
        final_centroids = [['%.4f' % elem for elem in nested] for nested in final_centroids]  # setting and printing the centroids in the correct format
        
        for i in range(k):
            print(','.join(final_centroids[i]))
        

    except Exception as e:      # handling errors in the kmeans_pp function
        print("An Error Has Occurred")
        exit(1)

    finally:                    # closing input files in case of an error

        try: 

            file1.close()
            file2.close()

        except Exception as e:
            exit(1)

kmeans_pp()
  
