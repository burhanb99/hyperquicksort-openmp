#include <bits/stdc++.h>
#include <bits/stdc++.h>
using namespace std;
int main(int argc, char* argv[]){
	if(argc != 3){
		cout<<"./random <output_filename> <number_of_elements>"<<endl;
		exit(EXIT_FAILURE);
	}
	srand(time(0));
	unordered_map<int, int> umap;
	int n = stoi(argv[2]);
	vector<int> arr(n, 0);
	for(int i = 0 ; i < n ; i++){
		int temp = rand() % (n);
		arr[i] = temp;
	}
	FILE *file = fopen(argv[1],"w");
    if (file == NULL) {
		cout<<"Error while opening file"<<endl;
		exit(EXIT_FAILURE);
	}
	fprintf(file,"%d \n", n);
	for(int i = 0 ; i < n; i++)
		fprintf(file,"%d \n",arr[i]);
	fclose(file);
	return 0;
}