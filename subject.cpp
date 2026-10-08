#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable:4996)

struct oras;
struct companie;
typedef struct lista_orase {

	oras* o;
	lista_orase* next;

}lista_orase;

typedef struct lista_companie {

	companie* c;
	lista_companie* next;

}lista_companie;

typedef struct companie {

	char* nume;
	lista_orase* o;
	int nr_orase;
	int order_index;
	companie* next;
	int actual_index;

}companie;

typedef struct zbor {

	oras* sursa;
	oras* dest;
	zbor* next;
	int cost;
	char* nume_companie;

}zbor;

typedef struct oras {

	char* nume;
	int nr_zboruri;
	int nr_companii;
	zbor* z;
	lista_companie* c;
	oras* next;
	int order;

}oras;

typedef struct sistem{

	int nr;
	oras** city;
	int nr_companii;
	companie** company;

}sistem;

void eroare(FILE* f) {
	if (f)
		return;
	perror("eroare deschdiere fisier");
	exit(1);
}

int get_hash(char* nume, int nr) {

	int val = 0;
	int len = strlen(nume);
	for (int i = 0; i < len; i++)
		val += nume[i] * 31;
	return val % nr;
}

bool find_company_in_city(oras*& main, char* nume) {

	for (lista_companie* c = main->c; c; c = c->next)
		if (!strcmp(c->c->nume, nume))
			return 1;
	return 0;

}

oras* find_oras(sistem* s, char* nume, int *index) {

	*index = get_hash(nume, s->nr);
	for (oras* a = s->city[*index]; a; a = a->next)
		if (!strcmp(a->nume, nume))
			return a;
	return 0;
}

oras* create_oras(sistem*& s, char* nume, int* index) {

	int index_main = 0;
	oras* temp = find_oras(s, nume,&index_main);
	if (!temp) {
		temp = (oras*)malloc(sizeof(oras));
		temp->nume = _strdup(nume);
		temp->nr_zboruri = 0;
		temp->nr_companii = 0;
		temp->c = 0;
		temp->z = 0;
		temp->order = *index;
		temp->next = s->city[*index];
		s->city[*index] = temp;
		if (s->city[index_main])
			s->city[index_main]->next = temp;
		else 
			s->city[index_main] = temp;
		(*index)++;
	}
	return temp;
}

companie* find_company(sistem*s, char* nume, int* index){

	*index = get_hash(nume, s->nr_companii);
	for (companie* c = s->company[*index]; c; c = c->next)
		if (!strcmp(c->nume, nume))
			return c;
	return 0;
}

companie* create_company(sistem*&s,oras* main, char* nume, int* order_companie) {

	int index = 0;
	companie* temp = find_company(s, nume, &index);
	if (!temp) {
		temp = (companie*)malloc(sizeof(companie));
		temp->nume = _strdup(nume);
		temp->o = 0;
		temp->order_index = *order_companie;
		s->company[*order_companie] = temp;
		(*order_companie)++;
		temp->nr_orase = 0;
	}
	lista_companie* temp_list = (lista_companie*)malloc(sizeof(lista_companie));
	temp_list->c = temp;
	temp_list->next = main->c;
	main->c = temp_list;
	main->nr_companii++;
	return temp;
}

void add_zbor(sistem* &s,oras*& sursa,char* dest,char* nume_companie, int * index, int cost) {

	zbor* temp = (zbor*)malloc(sizeof(zbor));
	temp->sursa = sursa;
	temp->dest = create_oras(s, dest, index);
	temp->next = sursa->z;
	temp->cost = cost;
	temp->nume_companie = _strdup(nume_companie);
	sursa->z = temp;
	sursa->nr_zboruri++;
	

	oras* second_oras = temp->dest;

	zbor* second = (zbor*)malloc(sizeof(zbor));
	second->sursa = second_oras;
	second->dest = sursa;
	second->next = second_oras->z;
	second_oras->z = second;
	second->nume_companie = _strdup(nume_companie);
	second->cost = cost;
	second_oras->nr_zboruri++;

}

void add_oras(sistem *&s, char* nume, int* index, int* order_companie) {

	FILE* f = fopen(nume, "r");
	eroare(f);
	
	char buffer[256];
	fscanf(f, "%256s", buffer);
	oras* temp = create_oras(s, buffer, index);
	int nr = 0;
	fscanf(f, "%d", &nr);
	for (int i = 0; i < nr; i++) {
		fscanf(f, "%256s", buffer);
		create_company(s, temp, buffer, order_companie);
	}

	char nume_companie[30];
	int cost;
	while (fscanf(f, "%256s %30s %d", buffer, nume_companie,&cost) == 3)
		add_zbor(s,temp, buffer, nume_companie,index,cost);
	fclose(f);
}

sistem* citire(const char* filename) {

	FILE* f = fopen(filename, "r");
	eroare(f);

	sistem* s = (sistem*)malloc(sizeof(sistem));
	fscanf(f, "%d %d", &s->nr,&s->nr_companii);
	s->city = (oras**)calloc(s->nr, sizeof(oras*));
	s->company = (companie**)calloc(s->nr_companii, sizeof(companie*));
	char buffer[256];
	int index = 0;
	int order_companie=0;
	for (int i = 0; i < s->nr; i++) {
		fscanf(f, "%256s", buffer);
		add_oras(s, buffer,&index,&order_companie);
	}
	fclose(f);
	return s;
}

void afisare(sistem *s) {

	for (int i = 0; i < s->nr; i++) {
		oras* temp_oras = s->city[i];
		for (temp_oras; temp_oras; temp_oras = temp_oras->next) {

			printf("\n\nOras: %s\n------\nCompanii:\n", temp_oras->nume);
			for (lista_companie* l = temp_oras->c; l; l = l->next)
				printf("%s\n", l->c->nume);
			printf("--------\nLegaturi:\n");
			for (zbor* z = temp_oras->z; z; z = z->next)
				printf("%s -> %s : [%s] DURATA ~ %d\n", z->sursa->nume, z->dest->nume, z->nume_companie, z->cost);

		}
	}

}

void show_most_important(sistem* s) {

	oras* max = 0;
	for (int i = 0; i < s->nr; i++)
		for (oras* c = s->city[i]; c; c = c->next)
			if (!max || c->nr_zboruri > max->nr_zboruri)
				max = c;
	if(max)
		printf("Cel important oras este %s [%d CONEXIUNI]\n", max->nume,max->nr_zboruri);
}

void show_both_cities(sistem* s, char* main_nume, char* second_nume) {

	int index = 0;
	oras* main = find_oras(s, main_nume, &index);
	oras* second = find_oras(s, second_nume, &index);

	int check = 0;
	for (lista_companie* c = main->c; c; c = c->next)
		for (lista_companie* c2 = second->c; c2; c2 = c2->next)
			if (!strcmp(c->c->nume, c2->c->nume)) {
				printf("%s opereaza ruta %s -> %s\n", c->c->nume, main->nume, second->nume);
				check = 1;
			}
	if (!check)
		printf("Nu exista rute directe intre %s si %s\n", main->nume, second->nume);
}

int dfs(int* visited,oras* main) {

	visited[main->order] = 1;
	int cnt = 1;
	for (zbor* z = main->z; z; z = z->next)
		if (!visited[z->dest->order])
			cnt += dfs(visited, z->dest);
	return cnt;
}

void depth(sistem* s,char* nume) {

	int index = 0;
	oras* temp = find_oras(s, nume, &index);
	int* visited = (int*)calloc(s->nr, sizeof(int));
	if (dfs(visited, temp) == s->nr) {
		printf("se poate ajunge in toate orasele\n");
		return;
	}
	printf("nu se poate ajunge in toate orasele pornind din %s", nume);

}

int get_min(int* dist, int* visited,int nr) {

	int min_value = 999;
	int index = -1;
	for(int i=0;i<nr;i++)
		if (!visited[i] && dist[i] < min_value) {
			index = i;
			min_value = dist[i];
		}
	return index;
}

void afisare_dijkstra(int* dist,int* parent, int index) {

	if (parent[index] != -1)
		afisare_dijkstra(dist,parent, parent[index]);
	printf("%d : COST{%d}\n", index, dist[index]);
}

void dijktra(sistem* s, char* sursa, char* dest) {

	int index = 0;
	int index2 = 0;
	oras* main = find_oras(s, sursa, &index);
	oras* second = find_oras(s, dest, &index2);

	int* dist = (int*)malloc(sizeof(int) * s->nr);
	int* parents = (int*)malloc(sizeof(int) * s->nr);
	int* visited = (int*)calloc(s->nr, sizeof(int));

	for (int i = 0; i < s->nr; i++) {
		dist[i] = 999;
		parents[i] = -1;
	}
	dist[main->order] = 0;
	for (int i = 0; i < s->nr && !visited[second->order]; i++) {

		int min = get_min(dist, visited, s->nr);
		for (zbor* z = s->city[min]->z; z; z = z->next)
			if (!visited[z->dest->order] && dist[z->dest->order] > dist[z->sursa->order] + z->cost) {
				dist[z->dest->order] = dist[z->sursa->order] + z->cost;
				parents[z->dest->order] = z->sursa->order;
			}

	}

	if (dist[second->order] == 999)
		printf("Nu exista drum\n");
	afisare_dijkstra(dist, parents, second->order);

}

void sink(companie** c, int index, int size) {

	int left = index * 2 + 1;
	int right = index * 2 + 2;
	int largest = index;

	if (left < size && c[left]->nr_orase > c[largest]->nr_orase)
		largest = left;
	if (right<size && c[right]->nr_orase > c[largest]->nr_orase)
		largest = right;

	if (largest != index) {

		companie* aux = c[largest];
		c[largest] = c[index];
		c[index] = aux;
		sink(c, largest,size);

	}

}

void change_to_heap(sistem*& s, int k) {

	int size = s->nr_companii;
	for (int i = s->nr_companii - 1; i >= 0; i--)
		sink(s->company, i, size);

	for (int i = 0; i < k; i++) {
		companie* aux = s->company[size - 1];
		s->company[size-1] = s->company[0];
		s->company[0] = aux;
		size--;
		sink(s->company, 0, size);
	}

	for (int i = s->nr; i > size; i--)
		printf("%s -> %d CONEXIUNI\n", s->company[i]->nume, s->company[i]->nr_orase);
}

int main() {
	sistem* s = citire("bac.txt");
	//afisare(s);

	//show_most_important(s);
	//show_both_cities(s, (char*)"Bucuresti", (char*)"Frankfurt");
	//depth(s, (char*)"Sofia");
	//dijktra(s, (char*)"Bucuresti", (char*)"Frankfurt");
	change_to_heap(s, 4);
}