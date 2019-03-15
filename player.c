#include "deck.h"
#include <stdlib.h>
#include <stdio.h>
#include "player.h"
#include <time.h>
/*
 * Function: add_card
 * -------------------
 *  Add a new card to the player's hand. 
 *
 *  target: the target player
 *  new_card: pointer to the new card to add
 *
 *  returns: return 0 if no error, non-zero otherwise
 */
int add_card(struct player* target, struct card* new_card){
	struct hand* new = (struct hand*)malloc(sizeof(struct hand)); 
	new->top.suit = new_card->suit;
	new->top.rank = new_card->rank;
	
	new->next = target->card_list;
	target->card_list = new; 
	(target->hand_size)++;
	return 0;
};
/*
 * Function: remove_card
 * ---------------------
 *  Remove a card from the player's hand. 
 *
 *  target: the target player
 *  old_card: pointer to the old card to remove
 *
 *  returns: return 0 if no error, non-zero otherwise
 */
int remove_card(struct player* target, struct card* old_card){ //could also funciton as search method for a card, return 1 if player don't have the card, return 0 if he has and removed it.
	struct hand* iterator = target->card_list;
	struct hand* previous = NULL;
	if(iterator == NULL) return 1;
	while((iterator->top).suit != old_card->suit || (iterator->top).rank != old_card->rank){
		previous = iterator;
		iterator = iterator->next;
		if(iterator == NULL) return 1;
	}
	//found item, break out of loop
	if(previous != NULL) 
		previous->next = iterator->next; //remove the target item
	else{
		target->card_list = iterator->next; //the target item was the first item, set the start to the second item,
	}
	(target->hand_size)--;
	free(iterator); //free the target item memory
	return 0;                    
};                  
/*
 * Function: check_add_book
 * ------------------------
 *  Check if a player has all 4 cards of the same rank.
 *  If so, remove those cards from the hand, and add the rank to the book.
 *  Returns after finding one matching set of 4, so should be called after adding each a new card.
 * 
 *  target: pointer to the player to check
 *  
 *  Return: a char that indicates the book that was added; return 0 if no book added.
 */
char check_add_book(struct player* target){
	char ranklist[14] = "234567891JKQA";  //use 1 to represent 10
	for(int i = 0; i < 13; i++){ //rank loop
		int COUNT = 0;
		
		struct hand* iterator = target->card_list;
		
		while(iterator != NULL){ //traverse player's cards for every rank
			if(iterator->top.rank == ranklist[i]) COUNT++;
			iterator = iterator->next;
		}
		if(COUNT != 4) continue; //the current rank is not booked, continue to next rank
		else{ //current rank is booked
			struct hand* previous = NULL;
			struct hand* follow = NULL;
			iterator = target->card_list; 
			if(iterator == NULL) break;//?????
			follow = iterator->next;
			while(iterator != NULL){ //traverse player's cards 
				
				if(iterator->top.rank == ranklist[i]){
					if(previous == NULL){ //remove the first item, so previous remain NULL
						target->card_list = follow;
						free(iterator);
						iterator = follow;
						if(follow == NULL) break;
						follow = follow->next;
					}
					else{
						previous->next = follow;
						free(iterator);
						iterator = follow;
						if(iterator == NULL) break;
						follow = follow->next;
					}
				}
				else{ //current iterator is not target rank, move to the next
					if(previous == NULL){
						previous = iterator;
						iterator = follow;
						if(follow == NULL) break;
						follow = follow->next;
					}
					else{
						previous = iterator;
						iterator = follow;
						if(iterator == NULL) break;
						follow = follow->next;
					}
				}
				
			}
		 
			int j = 0;
			while(target->book[j] != 0){ //break loop when found first empty slot; target->book[j] might also cause bug
 				j++;		
  			} //might cause bug if book is full!!
			target->book[j] = ranklist[i]; //add new rank to book array
			return ranklist[i];
		}
	}
};
/*
 * Function: search
 * ----------------
 *  Search a player's hand for a requested rank.
 *  
 *  rank: the rank to search for
 *  target: the player (and their hand) to search
 *
 *  Return: If the player has a card of that rank, return 1, else return 0
 */
int search(struct player* target, char rank){
	struct hand* iterator = target->card_list;
	while(iterator != NULL){ //traverse player's cards for every rank
		if((iterator->top).rank == rank) return 1;
		iterator = iterator->next;
	}
	return 0;

};
/*
 * Function: transfer_cards
 * ------------------------
 *   Transfer cards of a given rank from the source player's 
 *   hand to the destination player's hand. Remove transferred
 *   cards from the source player's hand. Add transferred cards
 *   to the destination player's hand.
 *   
 *   src: a pointer to the source player
 *   dest: a pointer to the destination player
 *   rank: the rank to transfer
 *
 *   Return: 0 if no cards found/transferred, <0 if error, otherwise 
 *   return value indicates number of cards transferred
 */   
int transfer_cards(struct player* src, struct player* dest, char rank){ //assume src has the rank 
	char suitlist[5] = "CDHS";
	int COUNT = 0;
	struct card* targetcard = malloc(sizeof(struct card));
	targetcard->rank = rank;
	for(int i = 0; i < 4; i++){
		targetcard->suit = suitlist[i];
		if(remove_card(src, targetcard) == 0){ //src has this card and removed it
			add_card(dest, targetcard); //add this card to dest
			COUNT++;
		}
	}
	free(targetcard);	
	return COUNT;
}; 
/*
 * Function: game_over
 * -------------------
 *   Boolean function to check if a player has 7 books yet
 *   and the game is over
 *
 *   target: the player to check
 *   
 *   Return: 1 if game is over, 0 if game is not over
 */
int game_over(struct player* target){
	if((target->book[6]) != 0) return 1;
	else return 0;
};
/* 
 * Function: reset_player
 * ----------------------
 *
 *   Reset player by free'ing any memory of cards remaining in hand,
 *   and re-initializes the book.  Used when playing a new game.
 * 
 *   target: player to reset
 * 
 *   Return: 0 if no error, and non-zero on error.
 */
int reset_player(struct player* target){ // freeing but what about the front of the list 
	target->hand_size = 0;
	struct hand* iterator = target->card_list;
	struct hand* previous = NULL;
	while(iterator != NULL){
		previous = iterator;
		iterator = iterator->next;
		free(previous);	
	}
	target->card_list = NULL;
	for(int i = 0; i < 7; i++){
		target->book[i] = 0;
	}
	return 0;
};
/* 
 * Function: computer_play
 * -----------------------
 *
 *   Select a rank randomly to play this turn. The player must have at least
 *   one card of the selected rank in their hand.
 *
 *   target: the player's hand to select from
 *
 *   Rank: return a valid selected rank
 */
/*char computer_play(struct player* target){
	//generate a random rank; then check if target's hand has this hand, true: return rank; false: pick another rank;
	int random;	
	char ranklist[14] = "234567891JKQA";
	srand(time(NULL));
	do{
		random = rand() % 12;
	}
	while(search(target, ranklist[random]) == 0);
	if(ranklist[random] == '1') printf("\nPlayer 2's turn, enter a Rank:10");
	else printf("\nPlayer 2's turn, enter a Rank:%c", ranklist[random]);
	return ranklist[random];
};*/
/* 
 * Function: user_play
 * -------------------
 *
 *   Read standard input to get rank user wishes to play.  Must perform error
 *   checking to make sure at least one card in the player's hand is of the 
 *   requested rank.  If not, print out "Error - must have at least one card from rank to play"
 *   and then re-prompt the user.
 *
 *   target: the player's hand to check
 * 
 *   returns: return a valid selected rank
 */
/*char user_play(struct player* target){ //use 1 to represent 10
	char c;
	do{
		printf("\nPlayer 1's turn, enter a Rank:");
		c = getchar();
        	getchar();//skip the '\n'
		if(c == '1') getchar(); //skip the '\n' in the case of 10
 		if(search(target, c) == 0)
			printf("Error - must have at least one card from rank to play");
	}	
	while(search(target, c) == 0);
	return c;
};*/

int book_count(struct player* target){
	int i = 0;
	while(target->book[i] != 0){
		i++;
	}
	return i;
};

/*int reportrank(struct player* target, char rank){
	struct hand* iterator = target->card_list;
	while(iterator != NULL){
		if(iterator->top.rank == rank)
			printf("%c%c ", rank, iterator->top.suit);
		iterator = iterator->next;
	}
};*/

