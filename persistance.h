#define LEN 30
#define MAX 100
#define path "database.bin"

typedef struct{
    char name[LEN];
    char password[LEN];
    
}pers;


int countdb;
pers tab[MAX];


void populate();
//void store();
int userAvailable(char *username);
void addPerson(char *username,char *password);
    
