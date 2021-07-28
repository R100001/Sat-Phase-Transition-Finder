
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define TIMEOUT		 60		// Program terminates after TIMEOUT secs
#define RESTARTSTEPS 10     // Walksat algorithm restarts after RESTARTSTEPS steps

int N;			// Number of propositions
int M;			// Number of sentences
int K;			// Number of propositions per sentence

int *Problem;	// This is a table to keep all the sentences of the problem

// Structure used for the stack of the depth-first search algorithm
struct stack_item {
	int *vector;
	struct stack_item *next;
};

//The head of the stack
struct stack_item *head=NULL;

// Reading the input file
int readfile(char *filename) {
	int i,j;

	FILE *infile;
	int err;

	// Opening the input file
	infile=fopen(filename,"r");
	if (infile==NULL) {
		printf("Cannot open input file. Now exiting...\n");
		fclose(infile);
		return -1;
	}

	// Reading the number of propositions
	err=fscanf(infile, "%d", &N);
	if (err<1) {
		printf("Cannot read the number of propositions. Now exiting...\n");
		fclose(infile);
		return -1;
	}

	if (N<1) {
		printf("Small number of propositions. Now exiting...\n");
		fclose(infile);
		return -1;
	}

	// Reading the number of sentences
	err=fscanf(infile, "%d", &M);
	if (err<1) {
		printf("Cannot read the number of sentences. Now exiting...\n");
		fclose(infile);
		return -1;
	}

	if (M<1) {
		printf("Low number of sentences. Now exiting...\n");
		fclose(infile);
		return -1;
	}

	// Reading the number of propositions per sentence
	err=fscanf(infile, "%d", &K);
	if (err<1) {
		printf("Cannot read the number of propositions per sentence. Now exiting...\n");
		fclose(infile);
		return -1;
	}

	if (K<2) {
		printf("Low number of propositions per sentence. Now exiting...\n");
		fclose(infile);
		return -1;
	}

	// Allocating memory for the sentences...
	Problem=(int*) malloc(M*K*sizeof(int));

	// ...and read them
	for (i=0;i<M;i++)
		for(j=0;j<K;j++) {
			err=fscanf(infile,"%d", Problem+i*K+j);
			if (err<1) {
				printf("Cannot read the #%d proposition of the #%d sentence. Now exiting...\n",j+1,i+1);
				fclose(infile);
				return -1;
			}
			if (Problem[i*K+j]==0 || Problem[i*K+j]>N || Problem[i*K+j]<-N) {
				printf("Wrong value for the #%d proposition of the #%d sentence. Now exiting...\n",j+1,i+1);
				fclose(infile);
				return -1;
			}
		}

	fclose(infile);

	return 0;
}

// Auxiliary function that displays all the sentences of the problem
void display_problem() {
	int i,j;

	printf("The current problem:\n");
	printf("====================\n");
	for (i=0;i<M;i++) {
		for(j=0;j<K;j++) {
			if (j>0)
				printf(" or ");
			if (Problem[i*K+j]>0)
				printf("P%d",Problem[i*K+j]);
			else
				printf("not P%d",-Problem[i*K+j]);
		}
		printf("\n");
	}
}

// Initializing a vector of values for the propositions with random values (used by the hill-climbing gready search algorithm)
void initialize(int *vector){
	int i,r;
	for(i=0;i<N;i++)
	{
		r=rand()%100;
		if (r>=50)
			vector[i]=1;
		else
			vector[i]=-1;
	}
#ifdef SHOW_COMMENTS
		printf("*");
#endif
}

// This function counts how many of the sentences are not satisfied by the current assignment of truth values to the propositions.
int count(int *vector){
	int c=0, i, j, satisfied;
	for(i=0;i<M;i++)
	{
		satisfied=0;
		for(j=0;j<K;j++)
			if ((Problem[i*K+j]>0 && vector[Problem[i*K+j]-1]==1)
				||
				(Problem[i*K+j]<0 && vector[-Problem[i*K+j]-1]==-1))
				satisfied=1;

		c+=satisfied;
	}

	return M-c;
}

// This function returns the first unsatisfied sentence as it is considered random and for time efficiency.
//
int get_unsatisfied_sentence(int *vector)
{
    int i, j, satisfied;
	for(i=0;i<M;i++)
	{
		satisfied=0;
		for(j=0;j<K;j++)
			if ((Problem[i*K+j]>0 && vector[Problem[i*K+j]-1]==1)
				||
				(Problem[i*K+j]<0 && vector[-Problem[i*K+j]-1]==-1))
				{
                    satisfied=1;
				    break;
				}
        if (!satisfied)
            return i*K;
	}

	// If the function gets to this step it means there is a logic problem with the code.
	printf("There is a logic problem with the code. Now exiting...");
	return -1;
}

// Auxiliary function that displays the current assignment of truth values to the propositions.
void display(int *vector){
	int i;
	for(i=0;i<N;i++)
		if(vector[i]==1)
			printf("P%d=true  ",i+1);
		else
			printf("P%d=false  ",i+1);
}

// The greedy algorithm
void WalkSat(char **argv)
{
	FILE *outfile;
	int *vector;
	int i, j, h, h1, h2, maxH;
	clock_t t, t1, t2;
	long restarts=0, steps=0, stepsOnMax;
	int best_change;
	int randSentence, randLiteral;

	vector=(int*) malloc(N*sizeof(int));

	t1=clock();

	while(1)
    {
        // Initialization steps
        initialize(vector);
        h = M - count(vector);
        maxH = h;
        stepsOnMax = steps;
        while (h!=M)
        {

            // Check for time limit
            t=clock();
            if (t-t1>CLOCKS_PER_SEC*TIMEOUT)
            {
                t2=clock();
                printf("\n\nNO SOLUTION found with walksat...\n");
                printf("Time spent: %f secs\n",((float) t2-t1)/CLOCKS_PER_SEC);
                printf("Number of restarts: %ld\n",restarts);
                printf("Number of steps: %ld\n",steps);
                return ;
            }

#ifdef SHOW_COMMENTS
            printf(".");
#endif

            steps++;

            randSentence = get_unsatisfied_sentence(vector); // Getting the pointer that points to the first unsatisfied sentence.
            if (randSentence == -1)
                return;

            if (rand() % 100 < 30) // Flipping a random variable.
            {
                randLiteral = rand() % K;
                vector[abs(Problem[randSentence+randLiteral])-1] = -vector[abs(Problem[randSentence+randLiteral])-1];
            }
            else // Looking for the best variable to flip.
            {
                h2=h;
                best_change=-1;
                for(j=0; j<K; j++) {    // For each literal in the randSentence...
                    vector[abs(Problem[randSentence+j])-1] = -vector[abs(Problem[randSentence+j])-1];	// ...if we flip its value...
                    h1 = M - count(vector);	// ...how many sentences are satisfied?
                    if (h1 > h2)			// If they are more than with previous flips or the current state...
                    {
                        h2 = h1;			// ...remember it!
                        best_change = j;
                    }
                    vector[abs(Problem[randSentence+j])-1] = -vector[abs(Problem[randSentence+j])-1];	// Cancel the last flip of value.
                }

                if (best_change>=0)
                    vector[abs(Problem[randSentence+best_change])-1] = -vector[abs(Problem[randSentence+best_change])-1];

            }

            h = M - count(vector);
            // Checking if we got to a better state that before.
            if (h > maxH)
            { // If yes
                maxH = h; // Remember the best state.
                stepsOnMax = steps; // Reset the steps from the last time we got to a better state.
            }
            else if(steps - stepsOnMax > RESTARTSTEPS) // If not and we did more steps that RESTARTSTEPS.
            {
                restarts++; // Restart.
                break;
            }
        }
        if(h==M) break;
    }

	t2=clock();
	printf("\n\nSolution found with walksat!\n"); display(vector); printf("\n");
	printf("Time spent: %f secs\n",((float) t2-t1)/CLOCKS_PER_SEC);
	printf("Number of restarts: %ld\n",restarts);
	printf("Number of steps: %ld\n",steps);

	outfile=fopen(argv[3],"w");
	if (outfile==NULL) {
		printf("Cannot open output file. Now exiting...\n");
		return;
	}
	for(i=0;i<N;i++)
		fprintf(outfile,"%d ",vector[i]);
	fclose(outfile);

}

// Auxiliary function that copies the values of one vector to another
void copy(int *vector1, int *vector2) {
	int i;
	for(i=0;i<N;i++)
		vector2[i]=vector1[i];
}


// This function creates a new stack node and pushes it on the top of the stack.
// Memory for the vector of the stack node is allocated.
// Input vector is not altered.
int push(int *vector)
{
	struct stack_item *temp=(struct stack_item*) malloc(sizeof(struct stack_item));
	if (temp==NULL)
		return 0;
	temp->vector=(int*) malloc(N*sizeof(int));
	copy(vector,temp->vector);
	temp->next=head;
	head=temp;
	return 1;
}

// This function retracts the top node of the stack.
// The function returns the vector of this node, while it frees the memory of the 'stack_item' node.
int* pop()
{
	struct stack_item *temp;
	int *vector;
	if (!head)
		return NULL;
	vector=head->vector;
	temp=head;
	head=head->next;
	free(temp);
	return vector;
}

// This function checks whether a current partial assignment is already invalid.
// In order for a partial assignment to be invalid, there should exist a sentence such that
// all propositions in the sentence have already value and their values are such that
// the sentence is false.
int valid(int *vector){
	int i,j;
	for(i=0;i<M;i++)
	{
		int satisfiable=0;
		for(j=0;j<K;j++)
			if ((Problem[i*K+j]>0 && vector[Problem[i*K+j]-1]>=0)
				||
				(Problem[i*K+j]<0 && vector[-Problem[i*K+j]-1]<=0))
				satisfiable=1;
		if (satisfiable==0)
			return 0;
	}
	return 1;
}

// Check whether a vector is a complete assignment and it is also valid.
int solution(int *vector)
{
    int i,j;
	for(i=0;i<M;i++)
	{
		int satisfiable=0;
		for(j=0;j<K;j++)
			if ((Problem[i*K+j]>0 && vector[Problem[i*K+j]-1]>0)
				||
				(Problem[i*K+j]<0 && vector[-Problem[i*K+j]-1]<0))
				satisfiable=1;
		if (satisfiable==0)
			return 0;
	}
	return 1;
}

// Given a partial assignment vector, for which a subset of the first propositions have values,
// this function pushes up to two new vectors to the stack, which concern giving to the first unassigned
// proposition the values true=1 and false=-1, after checking that the new vectors are valid.
int generate_children(int *vector)
{
	int i,err;

	for(i=0;i<N && vector[i]!=0;i++);	// Find the first proposition with no assigned value.

	vector[i]=-1;
	if (valid(vector))				// Check whether a "false" assignment is acceptable...
	{
		err=push(vector);			// ...and push to the stack if yes.
		if (err==0)
			return 0;
	}

	vector[i]=1;
	if (valid(vector))				// Check whether a "true" assignment is acceptable...
	{
		err=push(vector);			// ...and push to the stack if yes.
		if (err==0)
			return 0;
	}

	free(vector);					// Since "push" generates new vectors, the current vector can be deleted from memory.
	return 1;
}

void pure_literal_elimination(int *vector)
{
    int i, j, literals[N], canWeEliminate[N];

    for (i=0; i<N; i++)
    {
        literals[i] = 0;
        canWeEliminate[i] = 1;
    }

    for (i=0; i<M; i++)
        for (j=0; j<K; j++)
        {
            if (literals[abs(Problem[K*i+j])-1] == 0)   // If we haven't encounter this literal yet
            {
                // We set the value that satisfies this clause
                if (Problem[K*i+j] > 0)
                    literals[Problem[K*i+j]-1] = 1;
                else
                    literals[-Problem[K*i+j]-1] = -1;
            }
            else // If in another clause...
                if ((Problem[K*i+j] > 0 && literals[Problem[K*i+j]-1] < 0) || // ..we have
                    (Problem[K*i+j] < 0 && literals[-Problem[K*i+j]-1] > 0))  // a different value.
                    canWeEliminate[abs(Problem[K*i+j])-1] = 0; // We won't initialize its value.
        }

    for (i=0; i<N; i++)
        if (canWeEliminate[i])
            vector[i] = literals[i];
}

void unit_propagation(int *vector)
{
    int i, j, satisfiable, count0, literalP;

    for (i=0; i<M; i++) // For every clause in the Problem.
    {
        satisfiable = 0;
        count0 = 0;
        for (j=0; j<K; j++)
        {
            if (vector[abs(Problem[i*K+j])-1] == 0) // Checking if there is only one literal with no value.
            {
                count0++;
                literalP = j;
            }
            if (count0 == 2) break;
            if ((Problem[i*K+j]>0 && vector[Problem[i*K+j]-1]>0) || // If there is a literal that
				(Problem[i*K+j]<0 && vector[-Problem[i*K+j]-1]<0))  // satisfies the clause.
				{
				    satisfiable = 1;
				    break;
				}
        }
        if (satisfiable == 0 && count0 == 1) // If a clause is not satisfied and there is only one literal with no value.
        {
            if (Problem[i*K+literalP] < 0)
                vector[-Problem[i*K+literalP]-1] = -1;
            else
                vector[Problem[i*K+literalP]-1] = 1;
        }
    }
}


int DPLL(char **argv)
{
    int steps=0,err;
	int i;
	clock_t t, t1, t2;

	int *vector=(int*) malloc(N*sizeof(int));
	for(i=0;i<N;i++)
		vector[i]=0;

    t1=clock();

    pure_literal_elimination(vector); // Setting values to pure literals.

    push(vector);	// Initializing the stack.
	free(vector);	// Since "push" copies the vector, we can delete the previous vector.

	// While the stack is not empty...
	while(head)
	{
		t=clock();		// Check for the time limit
		if (t-t1>CLOCKS_PER_SEC*TIMEOUT)
		{
			t2=t;
			printf("\n\nNO SOLUTION found with dpll within the time limit...\n");
			printf("Time spent: %f secs\n",((float) t2-t1)/CLOCKS_PER_SEC);
			printf("Number of steps: %d\n",steps);
			return 0;
		}

		vector=pop();		// Retract the top element from the stack.
		steps++;
		if (solution(vector))	// If it is a solution...
		{
			FILE *outfile;
			t2=clock();
			printf("\n\nSolution found with dpll!\n"); display(vector); printf("\n");
			printf("Time spent: %f secs\n",((float) t2-t1)/CLOCKS_PER_SEC);
			printf("Number of steps: %d\n",steps);
			outfile=fopen(argv[3],"w");
			if (outfile==NULL) {
				printf("Cannot open output file. Now exiting...\n");
				return 0;
			}
			for(i=0;i<N;i++)
				fprintf(outfile,"%d ",vector[i]);
			fclose(outfile);
			return 0;
		}

		unit_propagation(vector); // Checking if there are unsatisfied clauses with only one literal left to set.
		err = generate_children(vector);	// ...else generate its children.
		if (err == -1)
            return -1;
	}

	// If the stack is empty without any solution found...
	t2=clock();
	printf("\n\nNO SOLUTION EXISTS. Proved by dpll!\n");
	printf("Time spent: %f secs\n",((float) t2-t1)/CLOCKS_PER_SEC);
	printf("Number of steps: %d\n",steps);
	return 0;
}

void syntax_error(char **argv) {
	printf("Use the following syntax:\n\n");
	printf("%s <method> <inputfile> <outputfile>\n\n",argv[0]);
	printf("where:\n");
	printf("<method> is either 'walksat' or 'dpll' (without the quotes)\n");
	printf("<inputfile> is the name of the file with the problem description\n");
	printf("<outputfile> is the name of the output file with the solution\n");
}

int main(int argc, char **argv) {
	int err;

	srand( (unsigned)time( NULL ) );

	if (argc!=4) {
		printf("Wrong number of arguments. Now exiting...\n");
		syntax_error(argv);
		return -1;
	}

	err=readfile(argv[2]);
	if (err<0)
		return -1;

//	display_problem();

	if (strcmp(argv[1],"walksat")==0)
		WalkSat(argv);
	else if (strcmp(argv[1],"dpll")==0)
    {
		err = DPLL(argv);
        if (err == -1)
        {
            printf("Memory exhaustion. Now exiting...");
            return -1;
        }
    }
	else
		syntax_error(argv);

	return 0;
}

