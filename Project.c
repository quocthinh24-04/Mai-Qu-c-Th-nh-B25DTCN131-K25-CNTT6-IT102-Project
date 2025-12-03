#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_BOOK 1000       
#define MAX_BORROW 100     
#define MAX_TITLE 100      
#define MAX_AUTHOR 100      
#define BOOKS_PER_PAGE 10   

// Khai bao cau truc Book
struct Book{
    int bookId;        
    char title[MAX_TITLE];  
    char author[MAX_AUTHOR]; 
    int publishYear;   
    int quantity;       
};

// Khai bao cau truc Date
struct Date{
    int day;    
    int month;
    int year;   
};

// Khai bao cau truc Borrow (Phieu muon/tra)
struct Borrow{
    int borrowId;     
    int bookId;         
    struct Date borrowDate; 
    struct Date borrowReturn; 
    char borrowerName[50];
    int status;        
};

// Khai bao bien toan cuc luu tru du lieu
struct Book books[MAX_BOOK];
int bookCount = 0;

struct Borrow borrows[MAX_BORROW];
int borrowCount = 0; 

// Khai bao prototype cac ham chinh
void addBook();
void updateBook();
void listBook();
void deleteBook();
void searchBook();
void borrowBook();
void returnBook();
void listBorrowRecords();

// Khai bao prototype cac ham tien ich/validation
void cleanInputBuffer(void);
int checkUniqueTitle(const struct Book books[], int bookCount, const char *title);
void readNonEmptyString(char *dest, int maxSize, const char *prompt, int allowDigit);
void readStringAllowEmpty(char *dest, int maxSize, const char *prompt);
int getValidIntegerRange(const char *prompt, int minValue, int allowZeroKeep);
int containsDigit(const char *s);
int isBookCurrentlyBorrowed(int bookId);
int getValidMenuChoice(const char *prompt, int min, int max);

// Ham kiem tra chuoi co chua chuoi con khong phan biet chu hoa/thuong (case-insensitive substring search)
int stringContainsCaseInsensitive(const char *haystack, const char *needle) {
    char h[MAX_TITLE + 1], n[MAX_TITLE + 1];
    size_t needle_len = strlen(needle);
    size_t haystack_len = strlen(haystack);

    if (needle_len > haystack_len) {
        return 0; 
    }

    for (size_t i = 0; i < needle_len; i++) {
        n[i] = tolower((unsigned char)needle[i]);
    }
    n[needle_len] = '\0';

    for (size_t i = 0; i <= haystack_len - needle_len; i++) {
        char sub_h[MAX_TITLE + 1];
        strncpy(sub_h, haystack + i, needle_len);
        sub_h[needle_len] = '\0';
        for (size_t j = 0; j < needle_len; j++) {
            sub_h[j] = tolower((unsigned char)sub_h[j]);
        }

        if (strcmp(sub_h, n) == 0) {
            return 1; 
        }
    }
    return 0;
}

// Ham kiem tra xem sach co dang duoc muon va chua tra khong
int isBookCurrentlyBorrowed(int bookId) {
    extern struct Borrow borrows[];
    extern int borrowCount; 

    for (int i = 0; i < borrowCount; i++) {
        if (borrows[i].bookId == bookId && borrows[i].status == 1) {
            return 1; 
        }
    }
    return 0; 
}

// Khai bao prototype cac ham xu ly Date
int isLeapYear(int year);
int daysInMonth(int month, int year);
struct Date getValidDate(const char *prompt);
int isValidDate(struct Date d);
int isDateAfter(struct Date d1, struct Date d2);

// Ham kiem tra nam nhuan
int isLeapYear(int year) {
    if (year % 400 == 0) return 1;
    if (year % 100 == 0) return 0;
    if (year % 4 == 0) return 1;
    return 0;
}

// Ham tra ve so ngay trong thang
int daysInMonth(int month, int year) {
    if (month < 1 || month > 12) return 0;
    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            return isLeapYear(year) ? 29 : 28; 
    }
    return 0; 
}

// Ham kiem tra ngay thang co hop le khong
int isValidDate(struct Date d) {
    if (d.year < 1900) return 0;
    if (d.month < 1 || d.month > 12) return 0; 
    if (d.day < 1 || d.day > daysInMonth(d.month, d.year)) return 0; 
    return 1;
}

// Ham nhap ngay thang hop le tu nguoi dung
struct Date getValidDate(const char *prompt) {
    struct Date d;
    printf("\n--- Nhap %s ---\n", prompt);
    while (1) {
        d.year = getValidIntegerRange("Nhap nam (>=1900): ", 1900, 0);
        d.month = getValidIntegerRange("Nhap thang (1-12): ", 1, 0);
        d.day = getValidIntegerRange("Nhap ngay: ", 1, 0);
        
        if (isValidDate(d)) {
            return d;
        } else {
            printf("Loi: Ngay/thang/nam khong hop le (Vui long kiem tra nam nhuan, so ngay trong thang).\n");
        }
    }
}

// Ham kiem tra ngay d1 co sau hoac bang ngay d2 khong
int isDateAfter(struct Date d1, struct Date d2) {
    if (d1.year > d2.year) return 1;
    if (d1.year < d2.year) return 0;
    if (d1.month > d2.month) return 1;
    if (d1.month < d2.month) return 0;
    return d1.day >= d2.day; 
}

// Ham kiem tra chuoi co chua ky tu so khong
int containsDigit(const char *s) {
    for (size_t i = 0; i < strlen(s); i++) {
        if (isdigit((unsigned char)s[i])) {
            return 1;
        }
    }
    return 0;
}

// Ham xoa bo dem nhap lieu (khong dung o day do dung fgets)
void cleanInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Ham doc chuoi bat buoc phai co noi dung (khong rong, co the kiem tra co chu so hay khong)
void readNonEmptyString(char *dest, int maxSize, const char *prompt, int allowDigit) {
    char buffer[200];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
                len--;
            }
            
            size_t start = 0;
            while (start < len && isspace((unsigned char)buffer[start])) {
                start++;
            }
            size_t end = len;
            while (end > start && isspace((unsigned char)buffer[end - 1])) {
                end--;
            }
            
            if (end > start) { 
                char trimmed_str[maxSize];
                size_t trimmed_len = end - start;
                size_t copy_len = trimmed_len < maxSize ? trimmed_len : maxSize - 1;
                memmove(trimmed_str, buffer + start, copy_len);
                trimmed_str[copy_len] = '\0';
                
                if (allowDigit == 0 && containsDigit(trimmed_str)) {
                    printf("Loi: Khong duoc nhap ky tu so (0-9). Nhap lai!\n");
                    continue;
                }
                
                strcpy(dest, trimmed_str);
                return;
            }
            printf("Loi: Khong duoc de trong. Nhap lai!\n");
        }
    }
}

// Ham doc chuoi cho phep bo trong
void readStringAllowEmpty(char *dest, int maxSize, const char *prompt) {
    char buffer[200];
    printf("%s", prompt);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        dest[0] = '\0';
        return;
    }
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
        len--;
    }
    
    size_t start = 0;
    while (start < len && isspace((unsigned char)buffer[start])) {
        start++;
    }
    size_t end = len;
    while (end > start && isspace((unsigned char)buffer[end - 1])) {
        end--;
    }
    
    if (end > start) { 
        len = end - start;
        size_t copy_len = len < maxSize ? len : maxSize - 1;
        memmove(dest, buffer + start, copy_len);
        dest[copy_len] = '\0';
    } else {
        dest[0] = '\0'; 
    }
}

// Ham kiem tra tieu de sach co bi trung trong danh sach sach hien co khong
int checkUniqueTitle(const struct Book books[], int bookCount, const char *title) {
    for (int i = 0; i < bookCount; i++) {
        if (strcmp(books[i].title, title) == 0) {
            return 0; 
        }
    }
    return 1; 
}

// Ham doc so nguyen hop le trong mot pham vi nhat dinh
int getValidIntegerRange(const char *prompt, int minValue, int allowZeroKeep) {
    char buffer[50];
    int number;
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
                len--;
            }
            
            if (len == 0) {
                if (allowZeroKeep == 1) return 0;
                printf("Loi: Khong duoc de trong!\n");
                continue;
            }
            
            int scan_result = sscanf(buffer, "%d", &number);
            if (scan_result == 1) {
                int consumed_len = 0;
                sscanf(buffer, "%d%n", &number, &consumed_len);
                size_t i = consumed_len;
                while (i < strlen(buffer) && isspace((unsigned char)buffer[i])) {
                    i++;
                }
                
                if (i == strlen(buffer)) {
                    if (number >= minValue) return number; 
                    printf("Loi: Phai >= %d\n", minValue);
                } else {
                    printf("Loi: Khong hop le! Hay nhap so nguyen.\n");
                }

            } else {
                printf("Loi: Khong hop le! Hay nhap so.\n");
            }
        }
    }
}

// Ham doc lua chon menu hop le trong mot pham vi nhat dinh
int getValidMenuChoice(const char *prompt, int min, int max) {
    char buffer[50];
    int number;
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // Kiem tra bo trong
            if (buffer[0] == '\n' || strlen(buffer) <= 1) {
                printf("Loi: Khong duoc de trong!\n");
                continue;
            }
            
            int scan_result = sscanf(buffer, "%d", &number);
            if (scan_result == 1) {
                int consumed_len;
                sscanf(buffer, "%d%n", &number, &consumed_len);
                size_t i = consumed_len;
                while (i < strlen(buffer) && isspace((unsigned char)buffer[i])) {
                    i++;
                }
                
                if (i == strlen(buffer)) { 
                    if (number >= min && number <= max) {
                        return number;
                    }
                    printf("Loi: Lua chon phai la so tu %d den %d.\n", min, max);
                } else {
                    printf("Loi: Khong hop le! Hay nhap so nguyen.\n");
                }
            } else {
                printf("Loi: Khong hop le! Hay nhap so.\n");
            }
        }
    }
}

int main(){
    int choice;
    
    books[0] = (struct Book){1, "Lap trinh C co ban", "Nguyen Van A", 2018, 5};
    books[1] = (struct Book){2, "Toan", "Tran Thi B", 2020, 4};
    books[2] = (struct Book){3, "Lich su Viet Nam", "Pham Van C", 1995, 3};
    books[3] = (struct Book){4, "Tieng Viet ", "Le Van D", 2023, 8};
    books[4] = (struct Book){5, "Giai tich 1", "Dinh Cong E", 2019, 12};
    books[5] = (struct Book){6, "Kinh te vi mo", "Doan Thi F", 2021, 6};
    books[6] = (struct Book){7, "Co so du lieu", "Hoang Van G", 2017, 9};
    books[7] = (struct Book){8, "Marketing can ban", "Cao Xuan H", 2022, 2};
    books[8] = (struct Book){9, "Phap luat dai cuong", "Vo Thi K", 2016, 7};
    books[9] = (struct Book){10, "Vat ly dai cuong 1", "Bui Van L", 2015, 10};
    books[10] = (struct Book){11, "Van hoc hien dai", "Ngo Thi M", 2024, 1}; 
    books[11] = (struct Book){12, "Dien toan dam may", "Ly Van N", 2023, 5};
    books[12] = (struct Book){13, "Ky thuat lap trinh", "Trinh Thi P", 2018, 0};
    books[13] = (struct Book){14, "Giao trinh Tieng Anh", "Hoang Van Q", 2020, 3};
    books[14] = (struct Book){15, "Tri tue nhan tao", "Nguyen Duc R", 2024, 15};
    books[15] = (struct Book){16, "Thi truong chung khoan", "Le Van S", 2021, 4};
    books[16] = (struct Book){17, "Quan ly du an", "Vu Thi T", 2019, 6};
    books[17] = (struct Book){18, "Linh kien dien tu", "Tran Van U", 2017, 2};
    books[18] = (struct Book){19, "Am thuc vung mien", "Hoang Thi V", 2022, 5};
    bookCount = 19;
    
    borrows[0] = (struct Borrow){1, 1, {15, 11, 2025}, {0, 0, 0}, "Ng Van A", 1}; 
    borrows[1] = (struct Borrow){2, 2, {10, 10, 2025}, {20, 10, 2025}, "Tran Thi B", 0}; 
    borrows[2] = (struct Borrow){3, 6, {01, 12, 2025}, {0, 0, 0}, "Pham Van T", 1}; 
    borrows[3] = (struct Borrow){4, 5, {25, 11, 2025}, {30, 11, 2025}, "Le Thi H", 0}; 
    borrows[4] = (struct Borrow){5, 7, {01, 12, 2025}, {0, 0, 0}, "Doan Van P", 1}; 
    borrows[5] = (struct Borrow){6, 10, {18, 11, 2025}, {28, 11, 2025}, "Ngo Thi Z", 0};
    borrows[6] = (struct Borrow){7, 15, {01, 12, 2025}, {0, 0, 0}, "Tran Xuan Y", 1};
    borrows[7] = (struct Borrow){8, 16, {10, 11, 2025}, {0, 0, 0}, "Nguyen Cong X", 1}; 
    borrows[8] = (struct Borrow){9, 1, {20, 11, 2025}, {25, 11, 2025}, "Hoang Van W", 0}; 
    borrows[9] = (struct Borrow){10, 18, {02, 12, 2025}, {0, 0, 0}, "Le Thi V", 1}; 
    borrows[10] = (struct Borrow){11, 5, {15, 11, 2025}, {0, 0, 0}, "Pham Van K", 1}; 
    borrows[11] = (struct Borrow){12, 3, {25, 10, 2025}, {01, 11, 2025}, "Ly Thi J", 0};
    borrowCount = 12;
    
    do {
        printf("|======================================================|");
        printf("\n|                      MENU                            |\n");
        printf("|======================================================|\n");
        printf("|1. Them moi sach                                      |\n");
        printf("|2. Cap nhat thong tin sach                            |\n");
        printf("|3. Hien thi danh sach (co phan trang)                 |\n");
        printf("|4. Xoa thong tin sach                                 |\n");
        printf("|5. Tim kiem sach                                      |\n");
        printf("|6. Them moi phieu muon                                |\n");
        printf("|7. Tra sach                                           |\n");
        printf("|8. Hien thi danh sach phieu muon (co phan trang)      |\n");
        printf("|9. Thoat                                              |\n");
        printf("|======================================================|\n");
        choice = getValidMenuChoice("Lua chon cua ban: ", 1, 9);
        switch (choice){
            case 1: addBook(); break;
            case 2: updateBook(); break;
            case 3: listBook(); break;
            case 4: deleteBook(); break;
            case 5: searchBook(); break;
            case 6: borrowBook(); break;
            case 7: returnBook(); break;
            case 8: listBorrowRecords(); break;
            case 9:
                printf("Thoat chuong trinh...\n");
                break;
            default:
                printf("Lua chon khong hop le!\n");
        }
    } while (choice != 9);

    return 0;
}

// Ham them sach moi
void addBook() {
    printf("\nTHEM MOI SACH\n");

    if (bookCount >= MAX_BOOK) {
        printf("Kho sach da day!\n");
        return;
    }
    
    struct Book newBook;
    newBook.bookId = bookCount + 1; 
    printf("Ma sach tu dong: %d\n", newBook.bookId);
    
    while (1) {
        readNonEmptyString(newBook.title, sizeof(newBook.title), "Nhap tieu de: ",0);
        if (checkUniqueTitle(books, bookCount, newBook.title)) break;
        printf("Loi: Tieu de da ton tai!\n");
    }
    
    readNonEmptyString(newBook.author, sizeof(newBook.author), "Nhap ten tac gia: ",0);
    newBook.publishYear = getValidIntegerRange("Nhap nam xuat ban (>1900): ", 1901, 0);
    newBook.quantity = getValidIntegerRange("Nhap so luong (>0): ", 1, 0);
    
    books[bookCount++] = newBook;
    printf("Them sach thanh cong!\n");
}

// Ham cap nhat thong tin sach
void updateBook() {
    printf("\nCAP NHAT THONG TIN SACH\n");
    if (bookCount == 0) {
        printf("Chua co sach de cap nhat!\n");
        return;
    }
    
    int bookId = getValidIntegerRange("Nhap ID sach can cap nhat: ", 1, 0);
    int idx = -1;
    // Tim sach theo ID
    for (int i = 0; i < bookCount; i++)
        if (books[i].bookId == bookId) {
            idx = i;
            break;
        }
        
    if (idx == -1) {
        printf("Khong tim thay sach!\n");
        return;
    }
    
    struct Book *b = &books[idx];
    char tmp[MAX_TITLE];
    while (1) {
        readNonEmptyString(tmp, sizeof(tmp), "Nhap Tieu de moi : ",0);
        if (checkUniqueTitle(books, bookCount, tmp) || strcmp(b->title, tmp) == 0) {
            strcpy(b->title, tmp);
            break;
        }
        printf("Loi: Tieu de da ton tai hoac bi trung. Hay nhap lai!\n");
    }
    
    readNonEmptyString(b->author, sizeof(b->author), "Nhap Tac gia moi: ",0);
    b->publishYear = getValidIntegerRange("Nhap Nam XB moi (>1900): ", 1901, 0);
    b->quantity = getValidIntegerRange("Nhap So luong moi (>0): ", 1, 0);
    
    printf("\nCap nhat sach thanh cong!\n", bookId);
}

// Ham hien thi danh sach sach (co phan trang)
void listBook() {
    printf("\nHIEN THI DANH SACH SACH (co phan trang)\n");
    if (bookCount == 0) {
        printf("Danh sach sach trong!\n");
        return;
    }
    
    int totalPages = (bookCount + BOOKS_PER_PAGE - 1) / BOOKS_PER_PAGE;
    int currentPage = 1;
    int choice;
    
    do {
        printf("\nTrang %d / %d\n", currentPage, totalPages);
        printf("+------+------------------------------------------+----------------------+------+----------+\n");
        printf("| %-4s | %-40s | %-20s | %-4s | %-8s |\n", "ID", "TIEU DE", "TAC GIA", "NAM", "SO LUONG");
        printf("+------+------------------------------------------+----------------------+------+----------+\n");
        
        int startIndex = (currentPage - 1) * BOOKS_PER_PAGE;
        int endIndex = startIndex + BOOKS_PER_PAGE;
        if (endIndex > bookCount) {
            endIndex = bookCount;
        }
        
        for (int i = startIndex; i < endIndex; i++) {
            printf("| %-4d | %-40s | %-20s | %-4d | %-8d |\n",
                         books[i].bookId, books[i].title, books[i].author, books[i].publishYear, books[i].quantity);
        }
        printf("+------+------------------------------------------+----------------------+------+----------+\n");
        
        printf("\nLUA CHON PHAN TRANG:\n");
        printf("1. Trang truoc \n" );
        printf("2. Quay lai Menu\n");
        printf("3. Trang sau \n" );
        choice = getValidMenuChoice("Lua chon cua ban (1, 2, 3): ", 1, 3);
        
        switch (choice) {
            case 1: 
                if (currentPage > 1) {
                    currentPage--;
                } else {
                    printf("Dang o trang dau tien!\n");
                }
                break;
            case 2: 
                printf("Quay lai Menu chinh\n");
                return;
            case 3: 
                if (currentPage < totalPages) {
                    currentPage++;
                } else {
                    printf("Dang o trang cuoi cung!\n");
                }
                break;
        }
    } while (1);
}

// Ham xoa thong tin sach
void deleteBook() {
    printf("\nXOA THONG TIN SACh\n");
    if (bookCount == 0) {
        printf("Chua co sach de xoa!\n");
        return;
    }
    
    int bookId = getValidIntegerRange("Nhap ID sach can xoa: ", 1, 0); 
    int idx = -1;
    
    for (int i = 0; i < bookCount; i++) {
        if (books[i].bookId == bookId) {
            idx = i;
            break;
        }
    }
    
    if (idx == -1) {
        printf("Loi: Khong tim thay sach voi ID %d!\n", bookId);
        return;
    }
    
    if (isBookCurrentlyBorrowed(bookId)) { 
        printf("Loi: Khong the xoa sach '%s' (ID %d) vi CO NGUOI DANG MUON CHUA TRA!\n", books[idx].title, bookId);
        return; 
    }
    
    for (int i = idx; i < bookCount - 1; i++) {
        books[i] = books[i + 1];
    }
    bookCount--;
    
    printf("Xoa sach co ID %d thanh cong!\n", bookId);
}

// Ham tim kiem sach theo tieu de (tim kiem tuong doi)
void searchBook() {
    printf("\nTIM KIEM SACH(Tim kiem tuong doi)\n");
    if (bookCount == 0) {
        printf("Danh sach sach trong!\n");
        return;
    }

    char searchName[MAX_TITLE];
    readNonEmptyString(searchName, sizeof(searchName), "Nhap tieu de can tim: ", 1);

    printf("\nKET QUA TIM KIEM cho \"%s\":\n", searchName);

    struct Book foundBooks[MAX_BOOK];
    int foundCount = 0;

    for (int i = 0; i < bookCount; i++) {
        if (stringContainsCaseInsensitive(books[i].title, searchName)) {
            foundBooks[foundCount++] = books[i];
        }
    }

    if (foundCount == 0) {
        printf("Khong tim thay sach nao co tieu de chua \"%s\".\n", searchName);
        return;
    }

    printf("+------+------------------------------------------+----------------------+------+----------+\n");
    printf("| %-4s | %-40s | %-20s | %-4s | %-8s |\n", "ID", "TIEU DE", "TAC GIA", "NAM", "SO LUONG");
    printf("+------+------------------------------------------+----------------------+------+----------+\n");

    for (int i = 0; i < foundCount; i++) {
        printf("| %-4d | %-40s | %-20s | %-4d | %-8d |\n",
               foundBooks[i].bookId, foundBooks[i].title, foundBooks[i].author, 
               foundBooks[i].publishYear, foundBooks[i].quantity);
    }
    printf("+------+------------------------------------------+----------------------+------+----------+\n");
    printf("Tim thay tat ca %d cuon sach.\n", foundCount);
}

// Ham them phieu muon sach
void borrowBook() {
    printf("\nTHEM PHIEU MUON\n");
    if (borrowCount >= MAX_BORROW) {
        printf("Danh sach phieu muon da day!\n");
        return;
    }

    struct Borrow newBorrow;
    int bookIdx = -1;
    int bookId;
    
    newBorrow.borrowId = borrowCount + 1;
    printf("Ma phieu muon tu dong: %d\n", newBorrow.borrowId);
    
    do {
        bookId = getValidIntegerRange("Nhap Ma sach can muon: ", 1, 0);
        bookIdx = -1;
        for (int i = 0; i < bookCount; i++) {
            if (books[i].bookId == bookId) {
                bookIdx = i;
                break;
            }
        }
        
        if (bookIdx == -1) {
            printf("Loi: Ma sach khong ton tai. Nhap lai!\n");
        } else if (books[bookIdx].quantity <= 0) {
            printf("Loi: Sach '%s' da het trong kho (So luong: 0). Khong the tao phieu muon.\n", books[bookIdx].title);
            bookIdx = -1;
        }
    } while (bookIdx == -1);

    newBorrow.bookId = bookId;
    readNonEmptyString(newBorrow.borrowerName, sizeof(newBorrow.borrowerName), "Nhap Ten nguoi muon: ", 1);
    newBorrow.borrowDate = getValidDate("Ngay Muon");
    
    newBorrow.borrowReturn.day = 0;
    newBorrow.borrowReturn.month = 0;
    newBorrow.borrowReturn.year = 0;
    newBorrow.status = 1;
    
    books[bookIdx].quantity--;
    borrows[borrowCount++] = newBorrow;
    
    printf("\n-------------------------------------------------\n");
    printf("Them phieu muon #%d thanh cong cho sach '%s'.\n", newBorrow.borrowId, books[bookIdx].title);
    printf("So luong sach con lai trong kho: %d\n", books[bookIdx].quantity);
    printf("-------------------------------------------------\n");
}

// Ham xu ly tra sach
void returnBook() {
    printf("\nTRA SACH\n");
    if (borrowCount == 0) {
        printf("Hien khong co phieu muon nao!\n");
        return;
    }
    
    int borrowId = getValidIntegerRange("Nhap ID phieu muon can tra: ", 1, 0);
    int borrowIdx = -1;
    
    // Tim phieu muon
    for (int i = 0; i < borrowCount; i++) {
        if (borrows[i].borrowId == borrowId) {
            borrowIdx = i;
            break;
        }
    }
    
    if (borrowIdx == -1) {
        printf("Loi: Khong tim thay phieu muon voi ID %d!\n", borrowId);
        return;
    }
    
    struct Borrow *b = &borrows[borrowIdx];
    
    if (b->status == 0) {
        printf("Loi: Phieu muon ID %d da duoc tra roi (Ngay tra: %d/%d/%d).\n", 
               borrowId, b->borrowReturn.day, b->borrowReturn.month, b->borrowReturn.year);
        return;
    }
    
    struct Date returnDate;
    while (1) {
        returnDate = getValidDate("Ngay Tra Sach");
        if (isDateAfter(returnDate, b->borrowDate)) {
            break;
        }
        printf("Loi: Ngay tra (%02d/%02d/%4d) phai sau ngay muon (%02d/%02d/%4d). Nhap lai!\n", 
               returnDate.day, returnDate.month, returnDate.year,
               b->borrowDate.day, b->borrowDate.month, b->borrowDate.year);
    }
    
    b->borrowReturn = returnDate;
    b->status = 0; 

    int bookIdx = -1;
    for (int i = 0; i < bookCount; i++) {
        if (books[i].bookId == b->bookId) {
            bookIdx = i;
            break;
        }
    }
    
    if (bookIdx != -1) {
        books[bookIdx].quantity++;
        printf("\n-------------------------------------------------\n");
        printf("Tra sach thanh cong cho phieu muon #%d.\n", borrowId);
        printf("Sach '%s' da duoc them vao kho.\n", books[bookIdx].title);
        printf("So luong sach con lai trong kho: %d\n", books[bookIdx].quantity);
        printf("-------------------------------------------------\n");
    } else {
        printf("Canh bao: Khong tim thay thong tin sach (ID: %d) de cap nhat so luong trong kho, nhung phieu muon #%d da duoc cap nhat trang thai 'Da tra'.\n", b->bookId, borrowId);
    }
}

// Ham hien thi danh sach phieu muon/tra (co phan trang)
void listBorrowRecords() {
    printf("\nDANH SACH PHIEU MUON (co phan trang)\n");
    if (borrowCount == 0) {
        printf("Danh sach phieu muon trong!\n");
        return;
    }
    
    int totalPages = (borrowCount + BOOKS_PER_PAGE - 1) / BOOKS_PER_PAGE;
    int currentPage = 1;
    int choice;
    
    do {
        printf("\nTrang %d / %d\n", currentPage, totalPages);
        printf("+--------+----------+----------------------+------------------+------------------+-------------+\n");
        printf("| %-6s | %-8s | %-20s | %-16s | %-16s | %-11s |\n", 
               "ID PM", "MA SACH", "TEN NGUOI MUON", "NGAY MUON", "NGAY TRA", "TRANG THAI");
        printf("+--------+----------+----------------------+------------------+------------------+-------------+\n");
        
        int startIndex = (currentPage - 1) * BOOKS_PER_PAGE;
        int endIndex = startIndex + BOOKS_PER_PAGE;
        if (endIndex > borrowCount) {
            endIndex = borrowCount;
        }
    
        for (int i = startIndex; i < endIndex; i++) {
            struct Borrow *b = &borrows[i];
            char borrowDateStr[11];
            char returnDateStr[11];
            char statusStr[15];
            
            sprintf(borrowDateStr, "%02d/%02d/%4d", b->borrowDate.day, b->borrowDate.month, b->borrowDate.year);
            
            if (b->status == 1) { 
                strcpy(returnDateStr, "Chua tra");
                strcpy(statusStr, "dang muon");
            } else { 
                sprintf(returnDateStr, "%02d/%02d/%4d", b->borrowReturn.day, b->borrowReturn.month, b->borrowReturn.year);
                strcpy(statusStr, "da tra");
            }
            
            printf("| %-6d | %-8d | %-20s | %-16s | %-16s | %-11s |\n",
                   b->borrowId, b->bookId, b->borrowerName, 
                   borrowDateStr, returnDateStr, statusStr);
        }
        
        printf("\nLUA CHON PHAN TRANG:\n");
        printf("1. Trang truoc \n" );
        printf("2. Quay lai Menu\n");
        printf("3. Trang sau \n" );
        choice = getValidMenuChoice("Lua chon cua ban (1, 2, 3): ", 1, 3);
        
        switch (choice) {
            case 1: 
                if (currentPage > 1) {
                    currentPage--;
                } else {
                    printf("Dang o trang dau tien!\n");
                }
                break;
            case 2: 
                printf("Quay lai Menu chinh\n");
                return;
            case 3: 
                if (currentPage < totalPages) {
                    currentPage++;
                } else {
                    printf("Dang o trang cuoi cung!\n");
                }
                break;
        }
    } while (1); 
}
