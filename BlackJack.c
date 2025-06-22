#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DECK_SIZE 52
#define MAX_HAND 12

typedef struct {
    char *name;
    char *suit;
    int value;
} Card;

typedef struct {
    Card cards[MAX_HAND];
    int count;
} Hand;

// Function Prototypes
void initialize_deck(Card deck[]);
void shuffle_deck(Card deck[]);
Card draw_card(Card deck[], int *top_of_deck);
int hand_value(Hand *hand);
void print_hand(Hand *hand, int hide_first_card);
void player_turn(Card deck[], int *top, Hand *player);
void dealer_turn(Card deck[], int *top, Hand *dealer);
void determine_winner(Hand *player, Hand *dealer);
void play_blackjack();
void clear_input_buffer();
int is_natural_blackjack(Hand *hand);

// Main Function
int main() {
    srand(time(NULL)); // Random seed
    char play_again;
    do {
        play_blackjack();
        printf("\nPlay again? (y/n): ");
        scanf(" %c", &play_again);
        clear_input_buffer();
    } while (play_again == 'y' || play_again == 'Y');
    return 0;
}

// Clears the input buffer
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Initialize deck with 52 cards
void initialize_deck(Card deck[]) {
    char *names[] = {
        "2", "3", "4", "5", "6", "7", "8", "9", "10",
        "Jack", "Queen", "King", "Ace"
    };
    int values[] = {
        2, 3, 4, 5, 6, 7, 8, 9, 10,
        10, 10, 10, 11
    };
    char *suits[] = {
        "Hearts", "Diamonds", "Clubs", "Spades"
    };

    int index = 0;
    for (int i = 0; i < 13; ++i) {
        for (int j = 0; j < 4; ++j) {
            deck[index].name = names[i];
            deck[index].suit = suits[j];
            deck[index].value = values[i];
            index++;
        }
    }
}

// Shuffle the deck using Fisher-Yates algorithm
void shuffle_deck(Card deck[]) {
    for (int i = DECK_SIZE - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

// Draw the top card from the deck
Card draw_card(Card deck[], int *top_of_deck) {
    // Check if we need to reshuffle
    if (*top_of_deck >= DECK_SIZE) {
        printf("Reshuffling deck...\n");
        shuffle_deck(deck);
        *top_of_deck = 0;
    }
    
    Card drawn = deck[*top_of_deck];
    (*top_of_deck)++;
    return drawn;
}

// Calculate the value of the hand
int hand_value(Hand *hand) {
    int total = 0;
    int ace_count = 0;

    for (int i = 0; i < hand->count; i++) {
        total += hand->cards[i].value;
        if (hand->cards[i].value == 11) {
            ace_count++;
        }
    }

    while (total > 21 && ace_count > 0) {
        total -= 10;
        ace_count--;
    }

    return total;
}

// Check if hand is a natural blackjack (21 with 2 cards)
int is_natural_blackjack(Hand *hand) {
    return (hand->count == 2 && hand_value(hand) == 21);
}

// Print the hand, optionally hiding the first card
void print_hand(Hand *hand, int hide_first_card) {
    printf("Hand: ");
    for (int i = 0; i < hand->count; i++) {
        if (i == 0 && hide_first_card) {
            printf("[Hidden] ");
        } else {
            printf("[%s of %s] ", hand->cards[i].name, hand->cards[i].suit);
        }
    }

    if (!hide_first_card) {
        printf("=> Total: %d", hand_value(hand));
    }

    printf("\n");
}

// Handle the player's turn
void player_turn(Card deck[], int *top, Hand *player) {
    char choice;

    // Check for natural blackjack first
    if (is_natural_blackjack(player)) {
        printf("You have a Blackjack!\n");
        return;
    }

    while (1) {
        printf("\nYour ");
        print_hand(player, 0);

        int total = hand_value(player);
        if (total > 21) {
            printf("You busted!\n");
            break;
        }

        printf("Hit or Stand? (h/s): ");
        scanf(" %c", &choice);
        clear_input_buffer();

        if (choice == 'h' || choice == 'H') {
            player->cards[player->count++] = draw_card(deck, top);
        } else if (choice == 's' || choice == 'S') {
            break;
        } else {
            printf("Invalid input. Please enter h or s.\n");
        }
    }
}

// Handle the dealer's turn
void dealer_turn(Card deck[], int *top, Hand *dealer) {
    printf("\nDealer's turn:\n");

    // Check for natural blackjack
    if (is_natural_blackjack(dealer)) {
        printf("Dealer has a Blackjack!\n");
        return;
    }

    while (hand_value(dealer) < 17) {
        dealer->cards[dealer->count++] = draw_card(deck, top);
        printf("Dealer hits...\n");
        print_hand(dealer, 0);
    }

    int total = hand_value(dealer);
    if (total > 21) {
        printf("Dealer busted with %d!\n", total);
    } else {
        printf("Dealer stands with %d.\n", total);
    }
}

// Determine and display the winner
void determine_winner(Hand *player, Hand *dealer) {
    int player_total = hand_value(player);
    int dealer_total = hand_value(dealer);
    int player_blackjack = is_natural_blackjack(player);
    int dealer_blackjack = is_natural_blackjack(dealer);

    printf("\nFinal Hands:\n");
    printf("Your ");
    print_hand(player, 0);
    printf("Dealer ");
    print_hand(dealer, 0);

    // Special case: natural blackjacks
    if (player_blackjack && dealer_blackjack) {
        printf("\nBoth have Blackjack! It's a push (tie)!\n");
        return;
    } else if (player_blackjack) {
        printf("\nYou win with a Blackjack! (Pays 3:2)\n");
        return;
    } else if (dealer_blackjack) {
        printf("\nDealer wins with a Blackjack!\n");
        return;
    }

    // Regular win conditions
    if (player_total > 21) {
        printf("\nYou busted. Dealer wins!\n");
    } else if (dealer_total > 21) {
        printf("\nDealer busted. You win!\n");
    } else if (player_total > dealer_total) {
        printf("\nYou win!\n");
    } else if (player_total < dealer_total) {
        printf("\nDealer wins!\n");
    } else {
        printf("\nIt's a push (tie)!\n");
    }
}

// Main gameplay loop
void play_blackjack() {
    Card deck[DECK_SIZE];
    int top = 0;

    Hand player = {.count = 0};
    Hand dealer = {.count = 0};

    printf("\n==== NEW GAME OF BLACKJACK ====\n");
    
    initialize_deck(deck);
    shuffle_deck(deck);

    // Deal initial 2 cards to each
    player.cards[player.count++] = draw_card(deck, &top);
    dealer.cards[dealer.count++] = draw_card(deck, &top);
    player.cards[player.count++] = draw_card(deck, &top);
    dealer.cards[dealer.count++] = draw_card(deck, &top);

    printf("\nDealer's ");
    print_hand(&dealer, 1);
    printf("Your ");
    print_hand(&player, 0);

    // Player's turn
    player_turn(deck, &top, &player);

    // If not busted, dealer plays
    if (hand_value(&player) <= 21) {
        dealer_turn(deck, &top, &dealer);
    }

    // Decide the winner
    determine_winner(&player, &dealer);
}
