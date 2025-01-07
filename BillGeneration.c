#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEM_NAME 50
#define BILL_HISTORY_FILE "bill_history.txt"
#define BILL_DETAILS_FILE "bill_details.txt"

typedef struct {
    int item_id;
    char item_name[MAX_ITEM_NAME];
    float item_price;
    int quantity;
} Item;

typedef struct {
    int bill_number;
    int total_items;
    float total_amount;
    Item *items;
} Bill;

void printBill(Bill *bill) {
    printf("\nBill Number: %d\n", bill->bill_number);
    printf("-----------------------------------------\n");
    printf("%-10s %-15s %-10s %-10s\n", "ID", "Name", "Price", "Qty");
    printf("-----------------------------------------\n");
    for (int i = 0; i < bill->total_items; i++) {
        printf("%-10d %-15s %-10.2f %-10d\n",
               bill->items[i].item_id, bill->items[i].item_name,
               bill->items[i].item_price, bill->items[i].quantity);
    }
    printf("-----------------------------------------\n");
    printf("Total: %.2f\n\n", bill->total_amount);
}

void generateBill(Bill *bill) {
    printf("Bill Number: ");
    scanf("%d", &bill->bill_number);

    printf("Number of Items: ");
    scanf("%d", &bill->total_items);

    bill->items = (Item *)malloc(bill->total_items * sizeof(Item));
    if (!bill->items) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    bill->total_amount = 0;

    for (int i = 0; i < bill->total_items; i++) {
        printf("\nItem %d:\n", i + 1);
        printf("ID: ");
        scanf("%d", &bill->items[i].item_id);
        printf("Name: ");
        scanf("%s", bill->items[i].item_name);
        printf("Price: ");
        scanf("%f", &bill->items[i].item_price);
        printf("Qty: ");
        scanf("%d", &bill->items[i].quantity);

        bill->total_amount += bill->items[i].item_price * bill->items[i].quantity;
    }

    printBill(bill);

    FILE *file = fopen(BILL_DETAILS_FILE, "a");
    if (file) {
        fprintf(file, "\nBill Number: %d\n", bill->bill_number);
        fprintf(file, "%-10s %-15s %-10s %-10s\n", "ID", "Name", "Price", "Qty");
        for (int i = 0; i < bill->total_items; i++) {
            fprintf(file, "%-10d %-15s %-10.2f %-10d\n",
                    bill->items[i].item_id, bill->items[i].item_name,
                    bill->items[i].item_price, bill->items[i].quantity);
        }
        fprintf(file, "Total: %.2f\n\n", bill->total_amount);
        fclose(file);
    }

    file = fopen(BILL_HISTORY_FILE, "a");
    if (file) {
        fprintf(file, "Bill Number: %d\n", bill->bill_number);
        fclose(file);
    }
}

void deleteBillFromHistory(int bill_number) {
    FILE *file = fopen(BILL_DETAILS_FILE, "r");
    if (!file) return;

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        fclose(file);
        return;
    }

    char line[100];
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        int current_bill;
        sscanf(line, "Bill Number: %d", &current_bill);

        if (current_bill == bill_number) {
            found = 1;
            continue;
        }

        fputs(line, temp);
    }

    fclose(file);
    fclose(temp);

    if (found) {
        remove(BILL_DETAILS_FILE);
        rename("temp.txt", BILL_DETAILS_FILE);
    } else {
        remove("temp.txt");
    }
}

void printSpecificBill(int bill_number) {
    FILE *file = fopen(BILL_DETAILS_FILE, "r");
    if (!file) return;

    char line[100];
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Bill Number:")) {
            int current_bill;
            sscanf(line, "Bill Number: %d", &current_bill);

            if (current_bill == bill_number) {
                found = 1;
                printf("%s", line);
                while (fgets(line, sizeof(line), file) && !strstr(line, "Total:")) {
                    printf("%s", line);
                }
                printf("%s", line); // Print the total line
                break;
            }
        }
    }

    fclose(file);
    if (!found) {
        printf("Bill Number %d not found.\n", bill_number);
    }
}

int main() {
    int choice, bill_number;
    Bill bill;

    while (1) {
        printf("\n1. Generate Bill\n2. Delete Bill\n3. Print Bill\n4. Exit\nChoice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                generateBill(&bill);
                free(bill.items);
                break;
            case 2:
                printf("Bill Number to Delete: ");
                scanf("%d", &bill_number);
                deleteBillFromHistory(bill_number);
                break;
            case 3:
                printf("Bill Number to Print: ");
                scanf("%d", &bill_number);
                printSpecificBill(bill_number);
                break;
            case 4:
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
}
