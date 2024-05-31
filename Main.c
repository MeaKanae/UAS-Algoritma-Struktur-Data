#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*

Binary Search Tree (Account)
- Search    OK
- Add       OK
- Delete    OK

Binary Heap (Book)
- Add       OK
- Delete    OK

Sorting (Book)
- Heap sort (Ref-Number Sorting)                            OK
- Bubble sort (Title Sorting (Ascending & Descending))      OK

Searching
- Binary Search     (Title, Ascending & Descending)         OK
- Interpolation Search (Ref-Number)                         OK

FREE MEMORY     OK

*/

typedef struct Date{
    int day;
    int month;
    int year;
}Date;

typedef struct Account{
    char userType[8];
    char email[40];
    char password[30];

    char name[30];
    int NIM;

    struct Account *left, *right;
}Account;

typedef struct Book{
    char title[40];
    char author[30];
    char publisher[30];
    int ref_number;
    int release;

    int stock;
    int avaiable;
}Book;

typedef struct Shelf{
    Book *book;
    int capacity;
    int heapSize;
} Shelf;

typedef struct BorrowBook{
    char name[30];
    int NIM;

    char title[40];
    int ref_number;

    char status[20];
    Date borrow, due;
}BorrowBook;

typedef struct BorrowList{
    BorrowBook *book;
    int size;
}BorrowList;



//========================================================================================================
//                                          Prototype Function


void getDate(Date *date);
double check_date(Date date, Date newDate);
void setDate(Date *date);
Date addTime(Date date, int days);


Account* newNode(char* userType, char* email, char* password, char *name, int NIM);
Account *insert(Account *node, char* userType, char* email, char* password, char *name, int NIM);
Account* getAccountData();
Account* validate_account_input(Account *account, char *email_input, char *password_input);
Account* inorder(Account* acc);
Account* login_page(Account *account);
Account* updateAccountFile(Account *acc, FILE* fp);
void addNewAccountMenu(Account *account);
Account* minValueNode(Account *root);
Account *deleteNode(Account *root, Account *currentUser, char* key, int *flag);
void removeAccountMenu(Account *root, Account *currentUser);
void searchAccountMenu(Account *accountList);

Shelf createShelf(int cap);
int parent(int i);
int left(int i);
int right(int i);
void swap(Book *x, Book *y);
void printBook(Shelf shelf);
void addNewBook(Shelf *mHeap, char* title,char* author, char* publisher,int ref_number, int release, int stock, int avaiable);
Book getMin(Shelf *mHeap);
void MinHeapify(Shelf *mHeap, int i);
Book extractMin(Shelf *mHeap);
void decreaseKey(Shelf *mHeap, int i, char* newValue);
void deleteKey(Shelf *mHeap, int index);
void fetchBookData(Shelf *shelf);
Shelf copyShelf(Shelf shelf);
void bubbleSort(Book book[], int size, char* mode);
int heapify(Book book[], int size, int i);
void heapSort(Book book[], int size);
int binarySearch(char* targetBook, Book *bookList, int size, char* mode);
int interpolationSearch(int targetBook, Book *bookList, int size);
void showAllBook(Shelf shelf);
void addBookMenu(Shelf *shelf);
void removeBookMenu(Shelf *shelf);
void updateBookFile(Book *bookList, int size);


void addNewBorrow(BorrowList *borrowList, char* name, int NIM, char* title, int ref_number, char* status, Date borrow, Date due);
void fetchBorrowData(BorrowList *borrowList, Account *account);
int checkDuplicate(char* target, BorrowBook *bookList, int size);
void updateBorrowFile(BorrowList borrowList, Account *user);
void borrowBook(Shelf *shelf, Date dateNow, Account *user, BorrowList *borrowList);
void showAllBorrow(BorrowList borrowList);
void updateBorrowData(BorrowList *borrowList, Account *user, Date date);
void returnBookMenu(BorrowList *borrowList, Account *user);


void freeMemory(Shelf *shelf, BorrowList *borrowList);
void freeAccount(Account *account);


int studentPage(Account *user);
void adminPage(Account *user, Account *accountList);



//========================================================================================================
//                                          Time Management


void getDate(Date *date){
    FILE *fp = fopen("Time.txt","r");
    if (fp == NULL){ //File ada
        time_t rawtime;
        time (&rawtime); //waktu sekarang
        struct tm* current_time = localtime(&rawtime); //Buat misahin date-month-year
        date->day = current_time->tm_mday ;
        date->month = current_time->tm_mon+1;
        date->year = current_time->tm_year + 1900;
        fp = fopen("Time.txt", "w");
        fprintf(fp,"%d/%d/%d", date->day, date->month, date->year);
        fclose(fp);
    }
    else{ //File ga ada, bikin baru
        fscanf(fp,"%d/%d/%d",&date->day, &date->month, &date->year);
        fclose(fp);
    }
}

double check_date(Date date, Date newDate){
    struct tm oldTime, newTime;
    oldTime.tm_mday = date.day;
    oldTime.tm_mon = date.month-1;
    oldTime.tm_year = date.year - 1900;
    oldTime.tm_hour = 0;
    oldTime.tm_min = 0;
    oldTime.tm_sec = 1;

    newTime.tm_mday = newDate.day;
    newTime.tm_mon = newDate.month-1;
    newTime.tm_year = newDate.year - 1900;
    newTime.tm_hour = 0;
    newTime.tm_min = 0;
    newTime.tm_sec = 1;

    double difference = difftime(mktime(&newTime), mktime(&oldTime)); //difftime buat return selisih waktu
    return difference;
}

void setDate(Date *date){
    int flag = 0; //Flag > 0 == inputnya salah
    Date newDate;
    printf("Date now : %d/%d/%d\n",date->day,date->month,date->year);
    printf("Change to: ");
    scanf("%d/%d/%d",&newDate.day,&newDate.month,&newDate.year);

    if ((newDate.day <= 0) || (newDate.month <= 0)){
        flag++;
    }

    if (newDate.month > 12){
        flag++;
    }
    //Cek tgl yg 31
    else if ((newDate.month == 1) || (newDate.month == 3) || (newDate.month == 5) || (newDate.month == 7) || (newDate.month == 8) || (newDate.month == 10) || (newDate.month == 12)){
        if (newDate.day > 31)
            flag++;
    }

    //Cek tgl yg 28-30
    else{
        //Kabisat = 29
        if (((newDate.year % 4) == 0) && (newDate.month == 2) && (newDate.day > 29)){
            flag++;
        }

        //Bukan kabisat
        else if (((newDate.year % 4) != 0) && (newDate.day > 28)){
            flag++;
        }
        else if (newDate.day > 30){
            flag++;
        }
    }

    if (flag > 0){
        printf("Invalid time!\n");
        return;
    }

    if (check_date(*date, newDate) > 0){ //Cek dulu time nya mundur atau ga
        date->day = newDate.day;
        date->month = newDate.month;
        date->year = newDate.year;

        FILE *fp = fopen("Time.txt", "w");
        fprintf(fp,"%d/%d/%d", date->day, date->month, date->year);
        fclose(fp);
        printf("Time changed!");
    }
    else{
        printf("You can't turn back time!\n");
    }
}

Date addTime(Date date, int days){
    int result = date.day + days;
    if ((date.month == 1) || (date.month == 3) || (date.month == 5) || (date.month == 7) || (date.month == 8) || (date.month == 10) || (date.month == 12)){
        if (result > 31){
            date.day = result % 31;
            date.month += 1;
        }
        else{
            date.day = result;
        }
    }
    else{
        //Kabisat = 29
        if (((date.year % 4) == 0) && (date.month == 2) && (date.day > 29)){
            date.day = result % 29;
            date.month += 1;
        }

        //Bukan kabisat
        else if (((date.year % 4) != 0) && (date.day > 28)){
            date.day = result % 28;
            date.month += 1;
        }
        else if (date.day > 30){
            date.day = result % 30;
            date.month += 1;
        }
        else{
            date.day = result;
        }
    }

    if (date.month > 12){
        date.month = date.month % 12;
        date.year++;
    }

    return date;

}



//========================================================================================================
//                                          Account Management


Account* newNode(char* userType, char* email, char* password, char *name, int NIM){
    Account *temp = (Account*) malloc(sizeof(Account));
    strcpy(temp->userType, userType);
    strcpy(temp->email, email);
    strcpy(temp->password, password);
    strcpy(temp->name, name);
    temp->NIM = NIM;
    temp->left = temp->right = NULL;
    return temp;
}

Account *insert(Account *node, char* userType, char* email, char* password, char *name, int NIM){
    if (node == NULL) return newNode(userType, email, password, name, NIM);
    if (strcmp(email, node->email) < 0) node->left = insert(node->left, userType, email, password, name, NIM);

    else if (strcmp(email, node->email) > 0) node->right = insert(node->right, userType, email, password, name, NIM);

    return node;
}

Account* getAccountData(){
    Account *account;
    account = NULL;
    FILE *fp;

    char userType[8];
    char email[40];
    char password[30];

    char name[30];
    int NIM;


    if ((fp = fopen("Account.txt","r")) != NULL){

        while(fscanf(fp,"%[^#]#%[^#]#%[^#]#%[^#]#%d\n",userType,email,password,name,&NIM) != EOF){
            account = insert(account, userType, email, password, name, NIM);
        }
        fclose(fp);
    }
    else{
        printf("Error: Account data file not found\n");
        exit(1); //account belum di malloc, jadi aman
    }

    return account;
}

Account* validate_account_input(Account *account, char *email_input, char *password_input){
    Account *node = account;
    while(node != NULL){
        if (strcmp(email_input, node->email) < 0){
            node = node->left;
        }
        else if (strcmp(email_input, node->email) > 0){
            node = node->right;
        }
        else{
            if (strcmp(password_input, node->password) != 0) break;
            Account *user = NULL;
            user = insert(user, node->userType, node->email, node->password, node->name, node->NIM);
            return user;
        }
    }
    return NULL;
}

Account* inorder(Account* acc){
    if (acc != NULL){
        printf("%s %s\n",acc->email, acc->password);
        inorder(acc->left);
        inorder(acc->right);
    }
}

Account* login_page(Account *account){

    inorder(account);


    char email_input[40];
    char password_input[30];

    while(1){
        fflush(stdin);
        printf("=======================================\n");
        printf("             Halaman Log in            \n");
        printf("=======================================\n");
        printf("Masukkan data\n");
        printf("Email: ");
        scanf("%[^\n]",email_input);
        fflush(stdin);
        printf("Password: ");
        scanf("%[^\n]",password_input);
        fflush(stdin);

        Account *user = validate_account_input(account, email_input,password_input);

        if (user == NULL){
            printf("Wrong Email/Password, try again\n");
        }
        else{
            return user;
        }
    }

}

Account* updateAccountFile(Account *acc, FILE* fp){
    if (acc != NULL){
        //printf("%s#%s#%s#%s#%d\n",acc->userType, acc->email, acc->password, acc->name, acc->NIM);
        fprintf(fp, "%s#%s#%s#%s#%d\n",acc->userType, acc->email, acc->password, acc->name, acc->NIM);
        updateAccountFile(acc->left, fp);
        updateAccountFile(acc->right, fp);
    }
}

void addNewAccountMenu(Account *account){
    char userType[8];
    char email[40];
    char password[30];

    char name[30];
    int NIM;

    fflush(stdin);
    printf("=======================================\n");
    printf("              Add Account              \n");
    printf("=======================================\n");
    int choose = 0;
    printf("Choose user type:\n[1] Admin\n[2]Student\nChoose: ");
    scanf("%d", &choose);
    if (choose == 1){
        strcpy(userType, "ADMIN");
    }
    else if (choose == 2){
        strcpy(userType, "STUDENT");
    }
    else{
        printf("Invalid input\n");
        return;
    }

    fflush(stdin);
    printf("Name: ");
    scanf("%[^\n]", name);
    fflush(stdin);

    printf("NIM: ");
    scanf("%d", &NIM);
    fflush(stdin);
    printf("Email: ");
    scanf("%[^\n]", email);
    fflush(stdin);
    printf("Password: ");
    scanf("%[^\n]", password);
    fflush(stdin);

    account = insert(account, userType, email, password, name, NIM);
    FILE *fp = fopen("Account.txt", "w");
    updateAccountFile(account, fp);
    fclose(fp);
    printf("Account created!\n");
}

Account* minValueNode(Account *root){
    Account *curr = root, *parent = NULL;

    while(curr != NULL){
        //printf("check\n");
        parent = curr;
        curr = curr->left;
    }

    return parent;
}

Account *deleteNode(Account *root, Account *currentUser, char* key, int *flag){
    if (root == NULL)
        return root;

    if (strcmp(key, root->email) < 0)
        root->left = deleteNode(root->left, currentUser, key, flag);
    else if (strcmp(key, root->email) > 0)
        root->right = deleteNode(root->right,currentUser ,key, flag);

    else{
        if (strcmp(root->email, currentUser->email) == 0){
            *flag = -1;
            return root;
        }

        if (root->left == NULL){
            Account* temp = root->right;
            free(root);
            *flag = 1;
            return temp;
        }
        else if (root->right == NULL){
            Account* temp = root->left;
            free(root);
            *flag = 1;
            return temp;
        }

        Account *temp = minValueNode(root->right);
        strcpy(root->email, temp->email);
        strcpy(root->name, temp->name);
        root->NIM = temp->NIM;
        strcpy(root->password, temp->password);
        strcpy(root->userType, temp->userType);
        root->right = deleteNode(root->right, currentUser, temp->email, flag);
        *flag = 1;
    }
    return root;
}

void removeAccountMenu(Account *root, Account *currentUser){
    char email[40];
    int flag = 0;
    fflush(stdin);
    printf("=======================================\n");
    printf("             Remove Account            \n");
    printf("=======================================\n");
    printf("Email: ");
    scanf("%[^\n]",email);
    fflush(stdin);
    deleteNode(root, currentUser, email, &flag);
    if (flag == 1){
        FILE *fp = fopen("Account.txt", "w");
        updateAccountFile(root, fp);
        fclose(fp);
        printf("Account deleted\n");
    }
    else if (flag == 0){
        printf("Account not found");
    }
    else if (flag == -1){
        printf("You can't delete your account!\n");
    }
}

void searchAccountMenu(Account *accountList){
    char email[40];
    fflush(stdin);
    printf("=======================================\n");
    printf("             Search Account            \n");
    printf("=======================================\n");
    printf("Email: ");
    scanf("%[^\n]",email);
    fflush(stdin);

    Account *ptr = accountList;
    while(ptr != NULL){
        if (strcmp(email,ptr->email) < 0){
            ptr = ptr->left;
        }
        else if (strcmp(email,ptr->email) > 0){
            ptr = ptr->right;
        }
        else{
            break;
        }
    }

    if (ptr != NULL){
        printf("\nAccount found!\n");
        printf("Name      : %s\n", ptr->name);
        printf("NIM       : %d\n", ptr->NIM);
        printf("Email     : %s\n", ptr->email);
        printf("User Type : %s\n", ptr->userType);
    }
    else{
        printf("Account not found!\n");
    }

    printf("\n");

}

//========================================================================================================
//                                          Book Management


Shelf createShelf(int cap){
    Shelf newHeap;
    newHeap.heapSize = 0;
    newHeap.capacity = cap;
    newHeap.book = malloc(sizeof(Book) * cap);

    int i;
    for (i = 0; i < cap; i++){
        newHeap.book[i].title[0] = 0;
        newHeap.book[i].title[1] = '\0';
    }

    return newHeap;
}

int parent(int i){
    return (i - 1) / 2;
}

int left(int i){
    return 2 * i + 1;
}

int right(int i){
    return 2 * i + 2;
}

void swap(Book *x, Book *y){
    Book temp = *x;
    *x = *y;
    *y = temp;

    //printf("Swapped %s and %s\n",x->title, y->title);
}

void printBook(Shelf shelf){
    for (int i = 0; i < shelf.heapSize; i++){
        printf("Title       : %s\n", shelf.book[i].title);
        printf("Author      : %s\n", shelf.book[i].author);
        printf("Publisher   : %s\n", shelf.book[i].publisher);
        printf("Ref. Number : %d\n", shelf.book[i].ref_number);
        printf("Release     : %d\n", shelf.book[i].release);
        printf("Stock       : %d\n", shelf.book[i].stock);
        printf("Avaiable    : %d\n", shelf.book[i].avaiable);
    }
    printf("\n");
}

void addNewBook(Shelf *mHeap, char* title,char* author, char* publisher,int ref_number, int release, int stock, int avaiable){
    //printf("Current Heap Size = %d\n\n", mHeap->heapSize);
    //printf("Inserting %s to heap\n", title);
    if (mHeap->heapSize == mHeap->capacity){
        printf("\nOverflow: Could not insertKey\n");
        return;
    }

    mHeap->heapSize++;
    int i = mHeap->heapSize - 1;
    strcpy(mHeap->book[i].title, title);
    strcpy(mHeap->book[i].author, author);
    strcpy(mHeap->book[i].publisher, publisher);
    mHeap->book[i].ref_number = ref_number;
    mHeap->book[i].release = release;
    mHeap->book[i].stock = stock;
    mHeap->book[i].avaiable = avaiable;

    while (i != 0 && strcmp(mHeap->book[parent(i)].title, mHeap->book[i].title) > 0){
        swap(&mHeap->book[i], &mHeap->book[parent(i)]);
        i = parent(i);
    }
}

Book getMin(Shelf *mHeap){
    return mHeap->book[0];
}

void MinHeapify(Shelf *mHeap, int i){
    int l = left(i);
    int r = right(i);
    int smallest = i;

    if (l < mHeap->heapSize && strcmp(mHeap->book[l].title, mHeap->book[smallest].title) < 0)
    {
        smallest = l;
    }

    if (r < mHeap->heapSize && strcmp(mHeap->book[r].title, mHeap->book[smallest].title) < 0)
    {
        smallest = r;
    }

    if (smallest != i)
    {
        swap(&mHeap->book[i], &mHeap->book[smallest]);
        MinHeapify(mHeap, smallest);
    }
}

Book extractMin(Shelf *mHeap){
    if (mHeap->heapSize == 1){
        mHeap->heapSize--;
        return mHeap->book[0];
    }

    Book root = mHeap->book[0];
    mHeap->book[0] = mHeap->book[mHeap->heapSize - 1];
    mHeap->heapSize--;
    MinHeapify(mHeap, 0);

    return root;
}

void decreaseKey(Shelf *mHeap, int i, char* newValue){
    strcpy(mHeap->book[i].title, newValue);
    while (i != 0 && strcmp(mHeap->book[parent(i)].title, mHeap->book[i].title) > 0){
        swap(&mHeap->book[i], &mHeap->book[parent(i)]);
        i = parent(i);
    }
}

void deleteKey(Shelf *mHeap, int index){
    char a[2];

    a[0] = 0;
    a[1] = '\0';

    //ganti title nya jadi "(null)", trs angkat ke root
    decreaseKey(mHeap, index, a);

    //hapus root
    extractMin(mHeap);
}

void fetchBookData(Shelf *shelf){
    FILE *fp = fopen("Book data.txt", "r");
    char title[40];
    char author[30];
    char publisher[30];
    int ref_number;
    int release;
    int stock;
    int avaiable;

    if (fp != NULL){
        while(fscanf(fp,"%[^#]#%[^#]#%[^#]#%d#%d#%d#%d\n",title,author,publisher,&ref_number,&release,&stock,&avaiable) != EOF){
            addNewBook(shelf, title,author,publisher,ref_number,release,stock,avaiable);
        }
        fclose(fp);
    }
    else{
        return;
    }
}

Shelf copyShelf(Shelf shelf){
    Shelf cp = createShelf(shelf.capacity);
    cp.heapSize = shelf.heapSize;
    for(int i = 0; i < shelf.heapSize; i++){
        strcpy(cp.book[i].title, shelf.book[i].title);
        strcpy(cp.book[i].author, shelf.book[i].author);
        strcpy(cp.book[i].publisher, shelf.book[i].publisher);
        cp.book[i].ref_number = shelf.book[i].ref_number;
        cp.book[i].release = shelf.book[i].release;
        cp.book[i].stock = shelf.book[i].stock;
        cp.book[i].avaiable = shelf.book[i].avaiable;
    }
    return cp;
}

void bubbleSort(Book book[], int size, char* mode){
     int i,j;

    if (strcmp(mode, "ASCENDING") == 0){
        for(i=1;i<size;i++){
            for(j=size-1;j>=1;j--){
                if(strcmp(book[j].title, book[j-1].title) < 0){
                    swap(&book[j], &book[j-1]);
                }
            }
        }
    }
    if (strcmp(mode, "DESCENDING") == 0){
        for(i=1;i<size;i++){
            for(j=size-1;j>=1;j--){
                if(strcmp(book[j].title, book[j-1].title) > 0){
                    swap(&book[j], &book[j-1]);
                }
            }
        }
    }
}

int heapify(Book book[], int size, int i){
    int maks = i;
    int l = 2*i + 1;
    int r = 2*i + 2;
    if(l<size && book[l].ref_number > book[maks].ref_number){
        maks = l;
    }
    if(r<size && book[r].ref_number > book[maks].ref_number){
        maks = r;
    }

    if (maks != i){
        swap(&book[i],&book[maks]);
        heapify(book,size,maks);
    }
}

void heapSort(Book book[], int size){
    int i;
    for(i = size/2 - 1; i >= 0; i--){
        heapify(book, size, i);
    }
    for(i=size-1;i>=0;i--){
        swap(&book[0],&book[i]);
        heapify(book, i, 0);
    }
}

int binarySearch(char* targetBook, Book *bookList, int size, char* mode){
    int low,mid,high;
    low = 0;
    high = size -1;
    while (low <= high){
        mid = (low + high) / 2;
        if (strcmp(mode, "ASCENDING") == 0){
            if (strcmp(bookList[mid].title, targetBook) == 0) return mid;
            else if (strcmp(targetBook, bookList[mid].title) < 0) high = mid-1;
            else low = mid + 1;
        }
        else if (strcmp(mode, "DESCENDING") == 0){
            if (strcmp(bookList[mid].title, targetBook) == 0) return mid;
            else if (strcmp(targetBook, bookList[mid].title) > 0) high = mid-1;
            else low = mid + 1;
        }
    }
    return -1;
}

int interpolationSearch(int targetBook, Book *bookList, int size){
    int low,pos,high;
    low = 0;
    high = size -1;
    //printf("Size of arr = %d\n",high);

    do{
        pos = ((targetBook - bookList[low].ref_number) * (high - low) / (bookList[high].ref_number - bookList[low].ref_number)) + low;

        if (bookList[pos].ref_number == targetBook) return pos;
        if (bookList[pos].ref_number > targetBook) high = pos -1;
        else low = pos +1;
    }while(targetBook >= bookList[low].ref_number && targetBook <= bookList[high].ref_number);

    return -1;
}

void showAllBook(Shelf shelf){
    Shelf temp = copyShelf(shelf);

    Book bookList[temp.heapSize];
    int i = 0;
    while(temp.heapSize > 0){
        bookList[i] = extractMin(&temp); //ini data yng udh urut awalnya ascending
        i++;
    }
    free(temp.book);


    char SortingMode[10] = "ASCENDING";
    while(1){
        fflush(stdin);
        int choose = -1;
        printf("================================================================================================================================\n");
        printf("| %-25s | %-18s | %-15s | %-25s | %-7s | %-5s | %-11s |\n", "Title", "Author", "Publisher", "Ref-number", "Release", "Stock", "Avaiable");
        printf("================================================================================================================================\n");
        for(i = 0; i < shelf.heapSize; i++){
            printf("| %-25s | %-18s | %-15s | %-25d | %-7d | %-5d |      %d      |\n", bookList[i].title, bookList[i].author, bookList[i].publisher, bookList[i].ref_number, bookList[i].release, bookList[i].stock, bookList[i].avaiable);
        }
        printf("================================================================================================================================\n");
        while(1){
            printf("[1] Sort %s\n[2] Sort by Ref-Number\n[3] Search Book by Title\n[4] Search Book by Reference Number\n[5] Back\nOption: ", strcmp(SortingMode, "ASCENDING") == 0? "Descending" : "Ascending");
            scanf("%d", &choose);
            fflush(stdin);
            if (choose == 1){
                bubbleSort(bookList, shelf.heapSize, strcmp(SortingMode, "ASCENDING") == 0? "DESCENDING" : "ASCENDING");
                strcpy(SortingMode, strcmp(SortingMode, "ASCENDING") == 0? "DESCENDING" : "ASCENDING");
                break;
            }
            else if (choose == 2){
                heapSort(bookList, shelf.heapSize);
                strcpy(SortingMode, "REFERENCE");
                break;
            }
            else if (choose == 3){
                int flag = 0;
                if (strcmp(SortingMode, "REFERENCE") == 0){
                    bubbleSort(bookList, shelf.heapSize, "DESCENDING");
                    flag = 1;
                }

                fflush(stdin);
                char searchBook[40];
                printf("Title: ");
                scanf("%[^\n]",searchBook);
                fflush(stdin);
                int foundIndex = binarySearch(searchBook, bookList, shelf.heapSize, strcmp(SortingMode, "ASCENDING") == 0? "ASCENDING" : "DESCENDING");
                if (foundIndex != -1){
                    printf("\nBook found!\n");
                    printf("Title: %s\nAuthor: %s\nPublisher: %s\nRef. Number: %d\nRelease: %d\nStock: %d\nAvaiable: %d\n\n",bookList[foundIndex].title, bookList[foundIndex].author, bookList[foundIndex].publisher, bookList[foundIndex].ref_number, bookList[foundIndex].release, bookList[foundIndex].stock, bookList[foundIndex].avaiable);
                }
                else{
                    printf("Book not found\n");
                }

                if (flag == 1){
                    heapSort(bookList, shelf.heapSize);
                }
            }
            else if (choose == 4){
                int flag = 0;
                if (strcmp(SortingMode, "REFERENCE") != 0){
                    heapSort(bookList, shelf.heapSize);
                    flag = strcmp(SortingMode, "ASCENDING") == 0 ? 1 : 2;
                }

                int searchBook;
                printf("Ref. Number: ");
                scanf("%d", &searchBook);
                fflush(stdin);
                int foundIndex = interpolationSearch(searchBook, bookList, shelf.heapSize);
                if (foundIndex != -1){
                    printf("\nBook found!\n");
                    printf("Title: %s\nAuthor: %s\nPublisher: %s\nRef. Number: %d\nRelease: %d\nStock: %d\nAvaiable: %d\n\n",bookList[foundIndex].title, bookList[foundIndex].author, bookList[foundIndex].publisher, bookList[foundIndex].ref_number, bookList[foundIndex].release, bookList[foundIndex].stock, bookList[foundIndex].avaiable);
                }
                else{
                    printf("Book not found\n");
                }

                if (flag == 1){
                    bubbleSort(bookList, shelf.heapSize, "ASCENDING");
                }
                else if (flag == 2){
                    bubbleSort(bookList, shelf.heapSize, "DESCENDING");
                }

            }

            else if (choose == 5){
                return;
            }
            else{
                printf("Invalid input\n\n");
            }
        }
    }

}

void addBookMenu(Shelf *shelf){
    Book newBook;
    fflush(stdin);
    printf("=======================================\n");
    printf("            Add/Change Book            \n");
    printf("=======================================\n");
    printf("Title       : ");
    scanf("%[^\n]",newBook.title);
    fflush(stdin);
    printf("Author      : ");
    scanf("%[^\n]",newBook.author);
    fflush(stdin);
    printf("Publisher   : ");
    scanf("%[^\n]",newBook.publisher);
    fflush(stdin);
    printf("Ref-number  : ");
    scanf("%d",&newBook.ref_number);
    fflush(stdin);
    printf("Release year: ");
    scanf("%d", &newBook.release);
    fflush(stdin);
    printf("Stock: ");
    scanf("%d",&newBook.stock);
    newBook.avaiable = newBook.stock;
    fflush(stdin);

    Shelf temp = copyShelf(*shelf);
    Book bookList[shelf->heapSize];
    int i = 0;
    while(temp.heapSize > 0){
        bookList[i] = extractMin(&temp);
        i++;
    }
    free(temp.book);

    int duplicateIndex = binarySearch(newBook.title, bookList, shelf->heapSize, "ASCENDING");
    if (duplicateIndex != -1){
        printf("Book titled %s already exist, updating the information instead\n", newBook.title);
        bookList[duplicateIndex] = newBook;
        for(int i = 0; i < shelf->heapSize; i++){
            if (strcmp(shelf->book[i].title, newBook.title) == 0){
                printf("copied");
                shelf->book[i] = newBook;
                break;
            }
        }
        printf("Book's information updated!\n");
    }
    else{
        printf("Book added to library!\n");
        addNewBook(shelf, newBook.title, newBook.author, newBook.publisher, newBook.ref_number, newBook.release, newBook.stock, newBook.avaiable);
    }
    updateBookFile(bookList, shelf->heapSize);
}

void removeBookMenu(Shelf *shelf){
    char target[40];

    fflush(stdin);
    printf("=======================================\n");
    printf("              Remove Book              \n");
    printf("=======================================\n");
    printf("Title: ");
    scanf("%[^\n]", target);
    fflush(stdin);

    int found = 0;

    for(int i = 0; i < shelf->heapSize; i++){//Iterasi dari index 0 sampe ketemu
        if (strcmp(target, shelf->book[i].title) == 0){//Kalo ketemu
            found = 1;
            deleteKey(shelf, i); //masuk ke sini dulu

            Shelf temp = copyShelf(*shelf);
            Book bookList[shelf->heapSize];
            int i = 0;
            while(temp.heapSize > 0){
                bookList[i] = extractMin(&temp);
                i++;
            }
            free(temp.book);

            updateBookFile(bookList, shelf->heapSize);

            printf("Book removed from list\n");
            break;
        }
    }
    if (found == 0){
        printf("Book not found!\n");
    }
}

void updateBookFile(Book *bookList, int size){
    if (size == 0){
        return;
    }
    FILE *fp = fopen("Book data.txt", "w");

    for(int i = 0; i < size; i++){
        fprintf(fp,"%s#%s#%s#%d#%d#%d#%d\n",bookList[i].title,bookList[i].author,bookList[i].publisher,bookList[i].ref_number,bookList[i].release,bookList[i].stock,bookList[i].avaiable);
    }
    fclose(fp);
}



//========================================================================================================
//                                        Borrow Book Management


void addNewBorrow(BorrowList *borrowList, char* name, int NIM, char* title, int ref_number, char* status, Date borrow, Date due){
    borrowList->size += 1;
    borrowList->book = realloc(borrowList->book, borrowList->size * sizeof(BorrowBook));
    strcpy(borrowList->book[borrowList->size-1].name, name);
    borrowList->book[borrowList->size-1].NIM = NIM;
    strcpy(borrowList->book[borrowList->size-1].title, title);
    borrowList->book[borrowList->size-1].ref_number = ref_number;
    strcpy(borrowList->book[borrowList->size-1].status, status);

    borrowList->book[borrowList->size-1].borrow.day = borrow.day;
    borrowList->book[borrowList->size-1].borrow.month = borrow.month;
    borrowList->book[borrowList->size-1].borrow.year = borrow.year;

    borrowList->book[borrowList->size-1].due.day = due.day;
    borrowList->book[borrowList->size-1].due.month = due.month;
    borrowList->book[borrowList->size-1].due.year = due.year;
}

void fetchBorrowData(BorrowList *borrowList, Account *account){
    char userType[8];
    char email[40];
    char password[30];

    char name[30];
    char NIM_file[12];
    int NIM;
    char title[40];
    int ref_number;
    char status[20];
    char filename[25];
    Date borrow, due;
    FILE *fp;

    if (strcmp(account->userType, "STUDENT") == 0){
        sprintf(NIM_file, "%d", account->NIM);
        //itoa(account.NIM, NIM_file, 10);
        strcpy(filename, NIM_file);
        strcat(filename, "-Borrowing data.txt");
        fp = fopen(filename, "r");
        if (fp != NULL){
            while (fscanf(fp,"%[^#]#%d#%[^#]#%d#%[^#]#%d/%d/%d#%d/%d/%d\n", name, &NIM, title, &ref_number, status, &borrow.day, &borrow.month, &borrow.year, &due.day, &due.month, &due.year) != EOF){
                addNewBorrow(borrowList, name, NIM, title, ref_number, status, borrow, due);
            }
            fclose(fp);
        }
    }
    else{
        FILE *fp_acc = fopen("Account.txt", "r");
        while(fscanf(fp_acc,"%[^#]#%[^#]#%[^#]#%[^#]#%d\n",userType,email,password,name,&NIM) != EOF){
            sprintf(NIM_file, "%d", NIM);
            //itoa(NIM, NIM_file, 10);
            strcpy(filename, NIM_file);
            strcat(filename, "-Borrowing data.txt");
            fp = fopen(filename, "r");
            if (fp != NULL){
                while (fscanf(fp,"%[^#]#%d#%[^#]#%d#%[^#]#%d/%d/%d#%d/%d/%d\n", name, &NIM, title, &ref_number, status, &borrow.day, &borrow.month, &borrow.year, &due.day, &due.month, &due.year) != EOF){
                    addNewBorrow(borrowList, name, NIM, title, ref_number, status, borrow, due);
                }
                fclose(fp);
            }
        }
        fclose(fp_acc);
    }
}

int checkDuplicate(char* target, BorrowBook *bookList, int size){
    for(int i = 0; i < size; i++){
        if (strcmp(target, bookList[i].title) == 0) return 1;
    }
    return 0;
}

void updateBorrowFile(BorrowList borrowList, Account *user){
    if (borrowList.size == 0){
        return;
    }
    char fileName[20];
    char userNIM[6];
    int NIM;

    char userType[8];
    char email[40];
    char password[30];

    char name[30];

    if (strcmp(user->userType, "STUDENT") == 0){
        sprintf(userNIM, "%d", user->NIM);
        //itoa(user.NIM, userNIM, 10);
        strcpy(fileName, userNIM);
        strcat(fileName,"-Borrowing data.txt");
        //printf("Sub checkpoint\n");
        FILE *fp = fopen(fileName, "w");

        for(int i = 0; i < borrowList.size; i++){
            fprintf(fp, "%s#%d#%s#%d#%s#%d/%d/%d#%d/%d/%d\n", borrowList.book[i].name, borrowList.book[i].NIM, borrowList.book[i].title, borrowList.book[i].ref_number, borrowList.book[i].status, borrowList.book[i].borrow.day, borrowList.book[i].borrow.month, borrowList.book[i].borrow.year, borrowList.book[i].due.day, borrowList.book[i].due.month, borrowList.book[i].due.year);
        }
        fclose(fp);
    }
    else{
        FILE *fp_acc = fopen("Account.txt", "r");
        while(fscanf(fp_acc,"%[^#]#%[^#]#%[^#]#%[^#]#%d\n",userType,email,password,name,&NIM) != EOF){
            printf("read %s\n",name);
            if (strcmp(userType, "ADMIN") == 0){
                continue;
            }
            //printf("write to %s\n", name);


            sprintf(userNIM, "%d",NIM);
            strcpy(fileName, userNIM);
            strcat(fileName, "-Borrowing data.txt");
            /*
            itoa(NIM, userNIM, 10);
            strcpy(fileName, userNIM);
            strcat(fileName, "-Borrowing data.txt");
            */
            FILE *fp = fopen(fileName, "w");
            for(int i = 0; i < borrowList.size; i++){
                if (borrowList.book[i].NIM == NIM){
                    fprintf(fp, "%s#%d#%s#%d#%s#%d/%d/%d#%d/%d/%d\n", borrowList.book[i].name, borrowList.book[i].NIM, borrowList.book[i].title, borrowList.book[i].ref_number, borrowList.book[i].status, borrowList.book[i].borrow.day, borrowList.book[i].borrow.month, borrowList.book[i].borrow.year, borrowList.book[i].due.day, borrowList.book[i].due.month, borrowList.book[i].due.year);
                }
            }
            fclose(fp);
            //printf("done\n");
        }
        fclose(fp_acc);
    }
}

void borrowBook(Shelf *shelf, Date dateNow, Account *user, BorrowList *borrowList){
    printf("=======================================\n");
    printf("              Borrow Book              \n");
    printf("=======================================\n");
    if (borrowList->size == 4){
        printf("You can only borrow 4 books at a time!\n");
        return;
    }

    //printf("Shelf size: %d\n",shelf->heapSize);

    Shelf temp = copyShelf(*shelf);
    Book bookList[shelf->heapSize];
    int i = 0;
    while(temp.heapSize > 0){
        bookList[i] = extractMin(&temp);
        i++;
    }
    free(temp.book);

    fflush(stdin);
    char target[40];
    printf("Title: ");
    scanf("%[^\n]",target);
    fflush(stdin);
    int foundIndex = binarySearch(target, bookList, shelf->heapSize, "ASCENDING");

    if (foundIndex > -1){

        if (bookList[foundIndex].avaiable == 0){
            printf("%s is not avaiable for loan, please try again later\n", bookList[foundIndex].title);
            return;
        }

        //Sequential Search
        if (checkDuplicate(bookList[foundIndex].title, borrowList->book, borrowList->size) == 0){
            Date dateDue = addTime(dateNow, 7);
            addNewBorrow(borrowList, user->name, user->NIM, bookList[foundIndex].title, bookList[foundIndex].ref_number, "Borrowing", dateNow, dateDue);
            updateBorrowFile(*borrowList, user);

            bookList[foundIndex].avaiable -= 1;
            for(int i = 0; i < shelf->heapSize; i++){
                if (strcmp(bookList[foundIndex].title, shelf->book[i].title) == 0){
                    shelf->book[i].avaiable -= 1;
                }
            }

            updateBookFile(bookList, shelf->heapSize);

            printf("Book borrowed!\n");
        }
        else{
            printf("You can't borrow more than 1 book with the same title!\n");
        }
    }
    else{
        printf("Book not found\n");
    }
}

void showAllBorrow(BorrowList borrowList){
    printf("=======================================\n");
    printf("               Loan List               \n");
    printf("=======================================\n");
    for(int i = 0; i < borrowList.size; i++){
        if (strcmp(borrowList.book[i].status, "Returned") == 0){
            continue;
        }
        printf("Loan #%d\n",i+1);
        printf("Borrower    : %s\n",borrowList.book[i].name);
        printf("Title       : %s\n", borrowList.book[i].title);
        printf("Ref. Number : %d\n", borrowList.book[i].ref_number);
        printf("Status      : %s\n", borrowList.book[i].status);
        printf("Loan date   : %d/%d/%d\n", borrowList.book[i].borrow.day, borrowList.book[i].borrow.month, borrowList.book[i].borrow.year);
        printf("Due date    : %d/%d/%d\n", borrowList.book[i].due.day, borrowList.book[i].due.month, borrowList.book[i].due.year);
        printf("=======================================\n");
    }
}

void updateBorrowData(BorrowList *borrowList, Account *user, Date date){
    bool isChanged = false;
    double time_remaining;

    for(int i = 0; i < borrowList->size; i++){
        time_remaining = check_date(date, borrowList->book[i].due);
        if (time_remaining <= 0 && (strcmp(borrowList->book[i].status, "Borrowing") == 0)){
            isChanged = true;
            strcpy(borrowList->book[i].status, "Overdue");
        }
    }
    if (isChanged){
        updateBorrowFile(*borrowList, user);
    }
}

void returnBookMenu(BorrowList *borrowList, Account *user){
    int index = 0;

    showAllBorrow(*borrowList);
    printf("=======================================\n");
    printf("              Return Book              \n");
    printf("=======================================\n");
    printf("Loan Number: ");
    scanf("%d", &index);
    index--;

    if (index < 0 || index >= borrowList->size || strcmp(borrowList->book[index].status, "Borrowing") != 0){
        printf("Invalid Input\n");
        return;
    }
    else if(strcmp(borrowList->book[index].status, "Borrowing") == 0){
        strcpy(borrowList->book[index].status, "Returned");
    }
    updateBorrowFile(*borrowList,user);
    printf("Book Returned\n");
}


//========================================================================================================
//                                        Free Malloc Management


void freeMemory(Shelf *shelf, BorrowList *borrowList){
    free(shelf->book);
    free(borrowList->book);
}

void freeAccount(Account *account){
    if (account == NULL){
        return;
    }
    freeAccount(account->left);
    freeAccount(account->right);
    free(account);
}



//========================================================================================================
//                                             Home Page


int studentPage(Account *user){
    //Book
    Shelf shelf = createShelf(50);
    fetchBookData(&shelf);

    //Time
    Date date;
    getDate(&date);

    //Borrow
    BorrowList borrowList;
    borrowList.book = malloc(sizeof(BorrowBook));
    borrowList.size = 0;
    fetchBorrowData(&borrowList, user);

    int option;
    while(1){
        option = -1;
        fflush(stdin);
        printf("=======================================\n");
        printf("               Main Page               \n");
        printf("=======================================\n");
        printf("Welcome, %s\n",user->name);
        printf("Current time: %d/%d/%d\n",date.day, date.month, date.year);
        printf("[1] Show books list\n");
        printf("[2] Borrow a book\n");
        printf("[3] Show loan list\n");
        printf("[4] Log out\n");
        printf("[5] Exit\n");
        printf("[0] Change current time (for debugging)\n");
        printf("\nOption: ");
        scanf("%d",&option);
        printf("\n");

        if (option == 0){
            setDate(&date);
            updateBorrowData(&borrowList, user, date);
        }
        else if (option == 1){
            showAllBook(shelf);
        }
        else if (option == 2){
            borrowBook(&shelf, date, user, &borrowList);
        }
        else if (option == 3){
            showAllBorrow(borrowList);
        }
        else if (option == 4){
            freeMemory(&shelf, &borrowList);
            break;
        }
        else if (option == 5){
            freeMemory(&shelf, &borrowList);
            return 1;
        }
        else{
            printf("Invalid input\n");
        }
        printf("\n");
    }

    return 0;
}

void adminPage(Account *user, Account *accountList){
    //Book
    Shelf shelf = createShelf(50);
    fetchBookData(&shelf);

    //Time
    Date date;
    getDate(&date);

    //Borrow
    BorrowList borrowList;
    borrowList.book = malloc(sizeof(BorrowBook));
    borrowList.size = 0;
    fetchBorrowData(&borrowList, user);

    int option;
    while(1){
        fflush(stdin);
        option = -1;
        printf("=======================================\n");
        printf("               Main Page               \n");
        printf("=======================================\n");
        printf("Welcome, %s\n",user->name);
        printf("Current time: %d/%d/%d\n",date.day, date.month, date.year);
        printf("[1] Show books list\n");
        printf("[2] Add a book /change book's information\n");
        printf("[3] Remove a book\n");
        printf("[4] Show loan list\n");
        printf("[5] Return loan book\n");
        printf("[6] Search account\n");
        printf("[7] Add new account\n");
        printf("[8] Remove an account\n");
        printf("[9] Log out\n");
        printf("[10] Exit\n");
        printf("[0] Change current time (for debugging)\n");
        printf("\nOption: ");
        scanf("%d",&option);
        printf("\n");

        if (option == 0){
            setDate(&date);
            updateBorrowData(&borrowList, user, date);
        }
        else if (option == 1){
            showAllBook(shelf);
        }
        else if (option == 2){
            addBookMenu(&shelf);
        }
        else if (option == 3){
            removeBookMenu(&shelf);
        }
        else if (option == 4){
            showAllBorrow(borrowList);
        }
        else if (option == 5){
            returnBookMenu(&borrowList, user);
        }
        else if (option == 6){
            searchAccountMenu(accountList);
        }
        else if (option == 7){
            addNewAccountMenu(accountList);
        }
        else if (option == 8){
            removeAccountMenu(accountList, user);
        }

        else if (option == 9){
            freeMemory(&shelf, &borrowList);
            break;
        }
        else if (option == 10){
            freeMemory(&shelf, &borrowList);
            freeAccount(accountList);
            exit(0);
        }
        else{
            printf("Invalid input\n");
        }
        printf("\n");
    }
}

int main(){
    int option;
    Account *accountList, *currentUser;

    accountList = getAccountData();

    while(1){
        fflush(stdin);
        printf("=======================================\n");
        printf("              UMN Library              \n");
        printf("         Book-borrowing System         \n");
        printf("=======================================\n");
        printf("[1] Log in\n");
        printf("[2] Exit\n\n");
        printf("Option: ");
        scanf("%d",&option);
        if (option == 1){
            currentUser = login_page(accountList);
            if (strcmp(currentUser->userType,"ADMIN") == 0){
                adminPage(currentUser, accountList);
            }
            else if (strcmp(currentUser->userType, "STUDENT") == 0){
                int flag = 0;
                flag = studentPage(currentUser);
                if (flag == 1){
                    break;
                }
            }
            else{
                printf("Invalid login, try again\n");
            }
        }
        else if (option == 2){
            break;
        }
        else{
            printf("Invalid input\n");
        }
        printf("\n");
    }

    freeAccount(accountList);

    return 0;
}

