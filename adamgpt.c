#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_DICTIONARY_SIZE 50000
#define TRAIN_INSTANCE_NUM 160
#define TEST_INSTANCE_NUM  40

// Define model parameters
double w[MAX_DICTIONARY_SIZE]; // Initialize with random values

// Define hyperparameters
double beta1 = 0.8; // Exponential decay rates for moment estimates
double beta2 = 0.888;
double epsilon = 1e-15; // Small constant to prevent division by zero
double learning_rate = 0.001; // Initial learning rate
int epochs = 50;

// Define moment estimates
double m[MAX_DICTIONARY_SIZE] = {0};
double v[MAX_DICTIONARY_SIZE] = {0};
double dw[MAX_DICTIONARY_SIZE] = {0};

// Define cost function (binary cross-entropy)
double calculate_cost(double y, double y_hat) {
    double epsilon = 1e-8;
    return -y * log(y_hat + epsilon) - (1 - y) * log(1 - y_hat + epsilon);
}

// Implement forward propagation
double forward_propagation(double x[MAX_DICTIONARY_SIZE]) {
    double sum = 0;
    for (int i = 0; i < MAX_DICTIONARY_SIZE; i++) {
        sum += w[i] * x[i];
    }
    return tanh(sum);
}

void backward_propagation(double x[MAX_DICTIONARY_SIZE], double y, double y_hat) {
    // Calculate gradients using the chain rule
    for (int i = 0; i < MAX_DICTIONARY_SIZE; i++) {
	dw[i] = -(y - y_hat) * (1 - tanh(w[i] * x[i]) * tanh(w[i] * x[i])) * x[i];
    }
}

// Implement Adam optimization
void adam_optimizer(double x[MAX_DICTIONARY_SIZE], double y, int t) {

    printf("adam-loss 1 %lf\n",calculate_cost(y,forward_propagation(x)));
  
    double y_hat = forward_propagation(x);
    double error = y_hat - y;
    
    // Update biased first moment estimate
    for (int i = 0; i < MAX_DICTIONARY_SIZE; i++) {
	m[i] = beta1 * m[i] + (1 - beta1) * learning_rate * dw[i];
    }
    
    // Update biased second moment estimate
    for (int i = 0; i < MAX_DICTIONARY_SIZE; i++) {
        v[i] = beta2 * v[i] + (1 - beta2) * learning_rate * dw[i] * dw[i];
    }

    // Correct bias in moment estimates
    double m_corrected[MAX_DICTIONARY_SIZE];
    double v_corrected[MAX_DICTIONARY_SIZE];
    for (int i = 0; i < MAX_DICTIONARY_SIZE; i++) {
        m_corrected[i] = m[i] / (1 - pow(beta1, t));
        v_corrected[i] = v[i] / (1 - pow(beta2, t));
    }

    printf("adam-loss 2 %lf\n",calculate_cost(y,forward_propagation(x)));
    
    // Update parameters
    for (int i = 0; i < MAX_DICTIONARY_SIZE; i++) {
        w[i] -= learning_rate * m_corrected[i] / (sqrt(v_corrected[i]) + epsilon);
    }

    backward_propagation(x,y,y_hat);

    printf("adam-loss 3 %lf\n",calculate_cost(y,forward_propagation(x)));
}

// Main training loop
void train(double **training_data, double labels[], int num_instances) {
    clock_t beginning = clock();

    FILE *eploss = fopen("adamepochloss.txt", "w+");
    FILE *timeloss = fopen("adamtimeloss.txt", "w+");
    FILE *weightplot = fopen("weightplot5.txt", "w+");

    printf("loss beginning of train = %lf\n",calculate_cost(labels[0],forward_propagation(training_data[0])));

    for (int batch = 0; batch < num_instances; batch++) {
        // Forward propagation
        double output = forward_propagation(training_data[batch]);

        // Backward propagation
        backward_propagation(training_data[batch], labels[batch], output);
    }
    
    for (int epoch = 0; epoch < epochs; epoch++) {
        if(epoch == 0)
        printf("loss in first epoch = %lf\n",calculate_cost(labels[0],forward_propagation(training_data[0])));

	for (int i = 0; i < num_instances; i++) {
 	   double *x = training_data[i];
    	   double y = labels[i];

   	 if (num_instances % 160 == 0 && epoch % 100 == 0) {
   	     clock_t t = clock() - beginning;
     	   double time_taken = ((double) t) / CLOCKS_PER_SEC;
      	  fprintf(timeloss, "%lf %lf", time_taken, calculate_cost(y, forward_propagation(x)));
      	  fprintf(timeloss, "\n");
    	}

   	 if (i == 0 && epoch == 0)
       	 printf("loss before adam run = %lf\n", calculate_cost(y, forward_propagation(x)));

    	adam_optimizer(x, y, epoch * num_instances + i + 1);

    	if (i == 0 && epoch == 0)
        printf("loss after adam run = %lf\n", calculate_cost(y, forward_propagation(x)));

   }
	fprintf(weightplot,"%d ",epoch);
	for(int i = 0;i < MAX_DICTIONARY_SIZE;i++){
	    fprintf(weightplot,"%lf ",w[i]);
	}
	fprintf(weightplot,"\n");                         

        double *avg = malloc(MAX_DICTIONARY_SIZE * sizeof(double));
        for (int i = 0; i < MAX_DICTIONARY_SIZE; i++) {
            double sum = 0;
            for (int j = 0; j < num_instances; j++) {
                sum += training_data[j][i];
            }
            avg[i] = (double) (sum / (double) num_instances);
        }

        double avgy = 0;
        for (int i = 0; i < num_instances; i++) {
            avgy += labels[i];
        }
        avgy = (double) (avgy / (double) num_instances);
	
        fprintf(eploss, "%d %lf", epoch, calculate_cost(avgy, forward_propagation(avg)));
        fprintf(eploss, "\n");
    }
    fclose(eploss);
    fclose(weightplot);	
}

// Main function
int main() {
    // Load training data, labels, and test data
    FILE *train_file = fopen("hotv_train.txt", "r");
    FILE *test_file = fopen("hotv_test.txt", "r");

    for(int i = 0;i<MAX_DICTIONARY_SIZE;i++){
	w[i] = 0.000002;
    }	

    // Read training data
    double **training_data = malloc(TRAIN_INSTANCE_NUM * sizeof(double *));
    for (int i = 0; i < TRAIN_INSTANCE_NUM; i++) {
        training_data[i] = malloc(MAX_DICTIONARY_SIZE * sizeof(double));
        for (int j = 0; j < MAX_DICTIONARY_SIZE; j++) {
            fscanf(train_file, "%lf", &training_data[i][j]);
        }
    }

    // first 80 instances are positive, last 80 instances are negative
    double labels[TRAIN_INSTANCE_NUM];
    for (int i = 0; i < TRAIN_INSTANCE_NUM; i++) {
        if (i < TRAIN_INSTANCE_NUM / 2) {
            labels[i] = 1;
        } else {
            labels[i] = 0;
        }
    }

    printf("loss = %lf\n",calculate_cost(labels[0],forward_propagation(training_data[0])));
    
    // Train the model
    train(training_data, labels, TRAIN_INSTANCE_NUM);

    printf("loss = %lf\n",calculate_cost(labels[0],forward_propagation(training_data[0])));
    
    // Test the model on the test set
        double **test_data = malloc(TEST_INSTANCE_NUM * sizeof(double *));
    for (int i = 0; i < TEST_INSTANCE_NUM; i++) {
        test_data[i] = malloc(MAX_DICTIONARY_SIZE * sizeof(double));
        for (int j = 0; j < MAX_DICTIONARY_SIZE; j++) {
            fscanf(test_file, "%lf", &test_data[i][j]);
        }
    }
    // first 20 instances are positive, last 20 instances are negative
    double test_labels[TEST_INSTANCE_NUM];
    for (int i = 0; i < TEST_INSTANCE_NUM; i++) {
        if (i < TEST_INSTANCE_NUM / 2) {
            test_labels[i] = 1;
        } else {
            test_labels[i] = 0;
        }
    }
    // Calculate accuracy
    int correct = 0;
    for (int i = 0; i < TEST_INSTANCE_NUM; i++) {
        double *x = test_data[i];
        double y = test_labels[i];
        double y_hat = forward_propagation(x);
        if (y_hat >= 0.5 && y == 1) {
            correct++;
        } else if (y_hat < 0.5 && y == 0) {
            correct++;
        }
    }
    printf("Accuracy: %lf\n", (double) correct / TEST_INSTANCE_NUM);

    fclose(train_file);
    fclose(test_file);

    free(training_data);

    return 0;
}
