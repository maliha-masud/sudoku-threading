#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

//store the multiple values needed
struct loc
{
	int row;
	int col;
};

int invalidEntries = 0;

//global matrix to store grid puzzle values
int grid[9][9] = { 6,2,4,5,3,9,1,8,7,
		   5,1,9,7,2,8,6,3,4,
		   8,3,7,6,1,4,2,9,-1,
		   1,4,3,8,6,5,7,2,9,
		   9,5,8,2,4,7,3,6,1,
		   7,6,2,3,9,1,4,5,8,
		   3,5,1,9,5,6,8,4,2,
		   4,9,6,1,8,2,5,7,3,
		   2,8,5,4,7,3,9,1,6 };
void displayGrid(int arr[9][9]);

pthread_mutex_t m1; //mutex to synchronize writing of threads to array counts[]
int counts[10];

sem_t sem;
int moves=0;
int counter=0;   //to calculate invalid entries 
int array1[9]={0}; 
int wrong[9][9]={  
          6,2,4,5,3,9,1,8,7,
		   5,1,9,7,2,8,6,3,4,
		   8,3,7,6,1,4,2,9,-1,
		   1,4,3,8,6,5,7,2,9,
		   9,5,8,2,4,7,3,6,1,
		   7,6,2,3,9,1,4,5,8,
		   3,5,1,9,5,6,8,4,2,
		   4,9,6,1,8,2,5,7,3,
		   2,8,5,4,7,3,9,1,6 
        };
        
int soln[9][9] = {  
            {6, 2, 4, 5, 3, 9, 1, 8, 7},
            {5, 1, 9, 7, 2, 8, 6, 3, 4},
            {8, 3, 7, 6, 1, 4, 2, 9, 5},
            {1, 4, 3, 8, 6, 5, 7, 2, 9},
            {9, 5, 8, 2, 4, 7, 3, 6, 1},
            {7, 6, 2, 3, 9, 1, 4, 5, 8},
            {3, 7, 1, 9, 5, 6, 8, 4, 2},
            {4, 9, 6, 1, 8, 2, 5, 7, 3},
            {2, 8, 5, 4, 7, 3, 9, 1, 6}
        };
typedef struct 
{
   //attributes
    int row;
    int col;
    
} parameters;   //object

//phase 1 - functions
void* validateRows(void* args)
{
	//return if row checked by thread is valid
	bool* valid = malloc(sizeof(bool));
	*valid = true;

	struct loc *data = (struct loc *) args;
	
	pthread_t tmpthr = pthread_self();
	
	pthread_mutex_lock(&m1);
	for(int i = 1; i < 10; i++)
		counts[i] = 0;
	
	for(int i = data->col; i < 9; i++)
	{
		int val = grid[data->row][i];
		if(val > 0 && val < 10)
		{
			if(counts[val] == 0)
				counts[val]++;
			else
			{
				printf("-------------------------\n");
				printf("Thread id: %li\n", pthread_self());
				printf("%i is repeated\n", val);
				printf("Row: %i, Column: %i\n", data->row+1, i+1);
				invalidEntries++;
				//pthread_cancel(pthread_self());
			}
		}
		else
		{
			printf("-------------------------\n");
			printf("Thread id: %li\n", pthread_self());
			printf("Invalid digit %i\n", val);
			printf("Row: %i, Column: %i\n", data->row+1, i+1);
			invalidEntries++;
		}
	}
	
	for(int i = 1; i < 10; i++)
	{
		if(counts[i] == 1)
			*valid = true;
		else
		{
			*valid = false;
			break;
		}
	}
	pthread_mutex_unlock(&m1);
	
	if(*valid != true)
	{
		printf("Thread %li has canceled\n", tmpthr);
		pthread_cancel(tmpthr);
 	}
	
	pthread_exit((void*)valid); //return bool value
}

void* validateCols(void* args)
{
	//return if column checked by thread is valid
	bool* valid = malloc(sizeof(bool));
	*valid = true;
	
	//struct loc *data = (struct loc *) malloc(sizeof(struct loc));
	struct loc *data = (struct loc *) args;
	//*data = *args;
	pthread_t tmpthr = pthread_self();
	
	pthread_mutex_lock(&m1);
	for(int i = 1; i < 10; i++)
		counts[i] = 0;
	
	for(int i = data->row; i < 9; i++)
	{
		int val = grid[i][data->col];
		if(val > 0 && val < 10)
		{
			if(counts[val] == 0)
				counts[val]++;
			else
			{
				printf("-------------------------\n");
				printf("Thread id: %li\n", pthread_self());
				printf("%i is repeated\n", val);
				printf("Row: %i, Column: %i\n", i+1, data->col+1);
			}
		}
		else
		{
			printf("-------------------------\n");
			printf("Thread id: %li\n", pthread_self());
			printf("Invalid digit %i\n", val);
			printf("Row: %i, Column: %i\n", i+1, data->col+1);
		}
	}
	
	for(int i = 1; i < 10; i++)
	{
		if(counts[i] == 1)
			*valid = true;
		else
		{
			*valid = false;
			break;
		}
	}
	pthread_mutex_unlock(&m1);
	
	if(*valid != true)
	{
		printf("Thread %li has canceled\n", tmpthr);
		pthread_cancel(tmpthr);
 	}
	
	pthread_exit((void*)valid); //return bool value
}

void* validateBoxes(void* args)
{
	//return if sub-grid checked by thread is valid
	bool* valid = malloc(sizeof(bool));
	*valid = true;
	
	struct loc *data = (struct loc *) args;
	
	pthread_t tmpthr = pthread_self();
	
	pthread_mutex_lock(&m1);
	for(int i = 0; i < 9; i++)
		counts[i] = 0;
	
	for(int i = data->row; i < data->row + 3; i++)
	{
		for(int j = data->col; j < data->col + 3; j++)
		{
			int val = grid[i][j];
			if(val > 0 && val < 10)
			{
				if(counts[val] == 0)
					counts[val]++;
				else
				{
					printf("-------------------------\n");
					printf("Thread id: %li\n", pthread_self());
					printf("%i is repeated\n", val);
					printf("Row: %i, Column: %i\n", i+1, j+1);
				}
			}
			else
			{
				printf("-------------------------\n");
				printf("Thread id: %li\n", pthread_self());
				printf("Invalid digit %i\n", val);
				printf("Row: %i, Column: %i\n", i+1, j+1);
			}
		}
	}
	
	for(int i = 1; i < 10; i++)
	{
		if(counts[i] == 1)
			*valid = true;
		else
		{
			*valid = false;
			break;
		}
	}
	pthread_mutex_unlock(&m1);
	
	if(*valid != true)
	{
		printf("Thread %li has canceled\n", tmpthr);
		pthread_cancel(tmpthr);
 	}
	
	pthread_exit((void*)valid); //return bool value
}

//----------------------------------------------------------------PHASE 2-------------------------------------------------------------------------------------------------

//Function implemented by invalid entry threads
void *canFunc(void *params) {

parameters * data = (parameters *) params;
    int i = data->row;  
    int j = data->col;   
    
    
    
printf("Invalid entry at index [%i][%i]= %i\n",i,j,wrong[i][j]);
printf("Thread against entry : %li\n",pthread_self());

//semaphore
 sem_wait(&sem);
 wrong[i][j] = soln[i][j];
 moves++;
 printf("New entry at index [%i][%i]= %i\n\n",i,j,wrong[i][j]);
 sem_post(&sem);
pthread_t x = pthread_self();

pthread_cancel(x);
}

//function to check the rows and find invalid entries

void *row(void *args) {
   
    parameters * data = (parameters *) args;
    int Row = data->row;  
    int Col = data->col;   
    
  
    for (int i = Row; i < 9; i++) 
    {
        for (int j = Col; j < 9; j++) 
        {
            
            if(soln[i][j]!= wrong[i][j])
            {
             
             counter++;
            parameters * temp = (parameters *) malloc(sizeof(parameters));
            temp->row = i;
            temp->col = j;
            
             pthread_t a;
             pthread_create(&a,NULL,canFunc,(void *)temp);
             pthread_join(a,NULL);           
            }
              
        }
    }
    return NULL;
}

//function to check the columns and find invalid entries
void *col(void * args) {
    
    parameters * data = (parameters *) args;
    int Row = data->row;
    int Col = data->col;
    for (int i = Col; i < 9; ++i) {
       
        for (int j = Row; j < 9; ++j) {
        
        if(soln[j][i]!= wrong[j][i])
            {
            counter++;
            parameters * temp = (parameters *) malloc(sizeof(parameters));
            temp->row = j;
            temp->col = i;
            
             pthread_t a;
             pthread_create(&a,NULL,canFunc,(void *)temp);
             pthread_join(a,NULL);           
            }
        
        }
    return NULL;         
}
}
//--------------------------------------------------------------------------MAIN----------------------------------------------------------------------------------------
int main()

{
   char op;

   //---------------------------------------------------------------phase 1 CALL---------------------------------------------------------

    printf("Enter Your Choice (Phase1: 'A', Phase2: 'B'): ");
    scanf("%c", &op);
    
    switch(op)
    {
        case 'A':
{                // 
	printf("Initial matrix:");

	displayGrid(grid);

	

	bool* value; //to store result

	long int* tid;

	struct loc data;

	

	//declare required variables

	pthread_t row_threads[9]; //A thread to check that each row contains the digits 1 through 9

	pthread_t col_threads[9]; //A thread to check that each column contains the digits 1 through 9

	pthread_t box_threads[9]; //9 threads to check that each of the 3×3 sub-grids contains the digits 1 through 9

	pthread_mutex_init(&m1, NULL); //initialize mutex



	//pass to row threads

	data.col = 0;

	

	for(int i = 0; i < 9; i++)

	{

		data.row = i;

		pthread_create(&row_threads[i], NULL, validateRows, (void*)&data);

		pthread_join(row_threads[i], (void**)&value);

		

		if(*value == 1)

			printf("Row thread %i: %i\n", i, *value);

		else

			printf("-------------------------\n");

	}

	

	printf("-------------------------\n");

	data.row = 0;

	for(int i = 0; i < 9; i++)

	{

		data.col = i;

		pthread_create(&col_threads[i], NULL, validateCols, (void*)&data);

		pthread_join(col_threads[i], (void**)&value);

		

		if(*value == 1)

			printf("Col thread %i: %i\n", i, *value);

		else

			printf("-------------------------\n");

	}

	

	printf("-------------------------\n");

	data.row = 0; int tmp = 0; //to traverse through sub-grids

	for(int i = 0; i < 9; i++)

	{

		if(tmp > 6)

		{

			tmp = 0; //reset tmp

			data.row += 3; //move to next sub-grid

		}

		data.col = tmp;

		pthread_create(&box_threads[i], NULL, validateBoxes, (void*)&data);

		pthread_join(box_threads[i], (void**)&value);

		

		if(*value == 1)

			printf("Sub-grid thread %i: %i\n", i, *value);

		else

			printf("-------------------------\n");

		

		tmp += 3;

	}

	

	printf("-------------------------\n");

	printf("Total invalid entries = %i\n", invalidEntries);

	

	pthread_mutex_destroy(&m1); //destroy the mutex



	pthread_exit(NULL);

}



	//----------------------------------------------------------------- Phase 2 CALL----------------------------------
	case 'B':
	{

	  
	  
  //printing the unsolved suduku puzzle board
          
   printf("UnSolved Puzzleboard values:\n");    
   for(int i=0;i<9;i++)
   {
       for(int j=0;j<9;j++)
       {
        printf("%d",wrong[i][j]);  
        printf(" ");
       }
       printf("\n");
   }

printf("\n");    
    
   //setting the parameters of the struct 
    parameters * temp = (parameters *) malloc(sizeof(parameters));
    temp->row = 0;
    temp->col = 0;
   
   //initializing threads
    pthread_t thread_rows1;
    pthread_t thread_cols1;
    
    //semaphore
    sem_init(&sem,0,1);
    
    //creating the threads and passing the argument into function
    pthread_create(&thread_rows1, NULL, row, (void *) temp);
     pthread_create(&thread_cols1, NULL, col, (void *) temp);
     
     
     //joining the threads 
    pthread_join(thread_rows1, NULL);    
    pthread_join(thread_cols1, NULL);
    
    // Waiting for all the threads to finish their tasks 
   
     printf("Total invalid entries : %i\n",counter);    
      printf("Total moves : %i\n\n",moves);    

     //printing the unsolved suduku puzzle board
     
     
   printf("Solved Puzzleboard values:\n");    
   for(int i=0;i<9;i++)
   {
       for(int j=0;j<9;j++)
       {
        printf("%d",wrong[i][j]);  
        printf(" ");
       }
       printf("\n");
   }
     
}
}
}

//.........................................................
//function to display sudoku grid
void displayGrid(int arr[9][9])
{
	for(int i = 0; i < 9; i++)
	{
		if(i == 0 || i == 3 || i == 6)
		{
			printf("\n");
			for(int m = 0; m < 25; m ++)
				printf("-");
		}
		printf("\n");
		for(int j = 0; j < 9; j++)
		{
			if(j == 0 || j == 3 || j == 6)
				printf("| ");
			printf("%i ", grid[i][j]);
		}
		printf("| ");
	}
	
	printf("\n");
	for(int m = 0; m < 25; m ++)
		printf("-");
	printf("\n");
}

