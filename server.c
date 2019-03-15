/* 
 * echoservert.c - A concurrent echo server using threads
 */
/* $begin echoservertmain */
#include "csapp.h"
#include "gofish.h"
#include "player.h"
#include "deck.h"
#include "card.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void echo(int connfd)
{
	rio_t rio;
	char buf[MAXLINE];
    

    Rio_readinitb(&rio, connfd); //initialize fd
	
	char c = 'Y'; //signal to play another game or not
	while(c == 'Y'){ //game loop
		int turn = 0; // 0 for user turn, 1 for computer turn
		shuffle();
		reset_player(&user);
		reset_player(&computer);
		deal_player_cards(&user);
		deal_player_cards(&computer);
	 	
		
	    strcpy(buf,"Shuffling deck...\n");
		Rio_writen(connfd, buf, strlen(buf));
		
		
		while((game_over(&user) == 0) && (game_over(&computer) == 0)){ //turn loop start
			 
			if(turn == 0){ //user turn
				if(user.card_list == NULL) add_card(&user, next_card());

				//report()!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
				
				strcpy(buf, "\nPlayer 1's Hand-");
				Rio_writen(connfd, buf, strlen(buf));
				struct hand* iterator = user.card_list;
	
	
				while(iterator != NULL){
					if(iterator->top.rank == '1') {
						sprintf(buf,"10%c ", iterator->top.suit);
						Rio_writen(connfd, buf, strlen(buf));
					}	
					else{
						sprintf(buf, "%c%c ", iterator->top.rank, iterator->top.suit);
						Rio_writen(connfd, buf, strlen(buf));
					}
					iterator = iterator->next;
				}

	
				strcpy(buf, "\nPlayer 1's Book-");	
				Rio_writen(connfd, buf, strlen(buf));
				int i = 0;
				while(user.book[i] != 0){
				if(user.book[i] == '1'){
					strcpy(buf, "10 ");
					Rio_writen(connfd, buf, strlen(buf));
				}
				else{
					sprintf(buf, "%c ", user.book[i]);
					Rio_writen(connfd, buf, strlen(buf));
				}
				i++;
				}
				i = 0;
				strcpy(buf, "\nPlayer 2's Book-");
				Rio_writen(connfd, buf, strlen(buf));
				while(computer.book[i] != 0){
				if(computer.book[i] == '1'){
					strcpy(buf, "10 ");
					Rio_writen(connfd, buf, strlen(buf));
				}
				else{
					sprintf(buf, "%c ", computer.book[i]);
					Rio_writen(connfd, buf, strlen(buf));
				}
				i++;
				}
				
		

				//userplay() !!!!!
				char c; //user asks for a rank he has          
				do{
					strcpy(buf,"\nPlayer 1's turn, enter a Rank:\n");
					Rio_writen(connfd, buf, strlen(buf));

					strcpy(buf, "stop\n");
					Rio_writen(connfd, buf, strlen(buf));
	
					Rio_readlineb(&rio, buf, strlen(buf));
					//Fputs(buf, stdout);

					c = buf[0];
					
 					if(search(&user, c) == 0){
						strcpy(buf,"Error - must have at least one card from rank to play\n");
						Rio_writen(connfd, buf, strlen(buf));
					}
				}	
				while(search(&user, c) == 0);
				char uc = c;

				//Rio_readlineb(&rio, buf, MAXLINE);
				if(search(&computer, uc) == 0){ //computer dont have the rank user ask for, gofish
					if(uc == '1') {
						strcpy(buf, "    - Player 2 has no 10's\n");
						Rio_writen(connfd, buf, strlen(buf));
					}
					else {
						sprintf(buf,  "    - Player 2 has no %c's\n", uc);
						Rio_writen(connfd, buf, strlen(buf));
					}
					struct card* newcard = next_card(); //pick a new card
					
					add_card(&user, newcard); //add the new card to user's hand
					if(newcard->rank == '1'){
						sprintf(buf, "    - Go Fish, Player 1 draws 10%c\n", newcard->suit);
						Rio_writen(connfd, buf, strlen(buf));
					}
					else {
						sprintf(buf,  "    - Go Fish, Player 1 draws %c%c\n", newcard->rank, newcard->suit); //report the new card
						Rio_writen(connfd, buf, strlen(buf));
					}
					
					char mm = check_add_book(&user);
					if( mm != 0){
						if(mm == '1') {
							strcpy(buf, "    - Player 1 books 10\n");
							Rio_writen(connfd, buf, strlen(buf));
						}
						else {
							sprintf(buf,  "    - Player 1 books %c\n", mm);
							Rio_writen(connfd, buf, strlen(buf));
							}
						strcpy(buf, "    - Player 1 gets another turn\n");
						Rio_writen(connfd, buf, strlen(buf));
					} 
					else{ //no book added
						if(newcard->rank != uc) { //new card is not the one required, switch turn
							turn = 1; //swith to computer turn
							strcpy(buf, "    - Player 2's turn");
							Rio_writen(connfd, buf, strlen(buf));
						}
						else { //new card is the one required, user get another turn
							strcpy(buf, "    - Player 1 gets another turn\n");
							Rio_writen(connfd, buf, strlen(buf));							
						}
					}
					continue;	
					
						
					
				}
				else{  //computer has the rank user ask for
					strcpy(buf, "    - Player 2 has ");					
					Rio_writen(connfd, buf, strlen(buf));

					//reportrank###################################3
					iterator = computer.card_list;
					while(iterator != NULL){
						if(iterator->top.rank == uc){
							sprintf(buf,"%c%c ", uc, iterator->top.suit);
							Rio_writen(connfd, buf, strlen(buf));
						}
						iterator = iterator->next;
					}
					
					strcpy(buf, "\n    - Player 1 has ");

					//reportrank###################################3
					struct hand* iterator = user.card_list;
					while(iterator != NULL){
						if(iterator->top.rank == uc){
							sprintf(buf,"%c%c ", uc, iterator->top.suit);
							Rio_writen(connfd, buf, strlen(buf));
						}
						iterator = iterator->next;
					}

					transfer_cards(&computer, &user, uc);
					if(check_add_book(&user) == uc){
						if(uc == '1') {strcpy(buf, "\n    - Player 1 books 10"); Rio_writen(connfd, buf, strlen(buf));}
						else {sprintf(buf,  "\n    - Player 1 books %c", uc); Rio_writen(connfd, buf, strlen(buf));}
						strcpy(buf, "\n    - Player 1 gets another turn\n");
						Rio_writen(connfd, buf, strlen(buf));
					}
					else{
							strcpy(buf, "\n    - Player 2's turn");
							Rio_writen(connfd, buf, strlen(buf));
							turn = 1;
						} //switch to computer's turn
					continue;
				}
			}

			else{ //computer turn $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
				if(computer.card_list == NULL) add_card(&computer, next_card());
				//report()!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
				strcpy(buf, "\n");
				Rio_writen(connfd, buf, strlen(buf));
				strcpy(buf, "\nPlayer 1's Hand-");
				Rio_writen(connfd, buf, strlen(buf));
				struct hand* iterator = user.card_list;
	
	
				while(iterator != NULL){
					if(iterator->top.rank == '1') {
						sprintf(buf,"10%c ", iterator->top.suit);
						Rio_writen(connfd, buf, strlen(buf));
					}	
					else{
						sprintf(buf, "%c%c ", iterator->top.rank, iterator->top.suit);
						Rio_writen(connfd, buf, strlen(buf));
					}
					iterator = iterator->next;
				}

				strcpy(buf, "\nPlayer 1's Book-");	
				Rio_writen(connfd, buf, strlen(buf));
				int i = 0;
				while(user.book[i] != 0){
				if(user.book[i] == '1'){
					strcpy(buf, "10 ");
					Rio_writen(connfd, buf, strlen(buf));
				}
				else{
					sprintf(buf, "%c ", user.book[i]);
					Rio_writen(connfd, buf, strlen(buf));
				}
				i++;
				}
				i = 0;
				strcpy(buf, "\nPlayer 2's Book-");
				Rio_writen(connfd, buf, strlen(buf));
				while(computer.book[i] != 0){
				if(computer.book[i] == '1'){
					strcpy(buf, "10 ");
					Rio_writen(connfd, buf, strlen(buf));
				}
				else{
					sprintf(buf, "%c ", computer.book[i]);
					Rio_writen(connfd, buf, strlen(buf));
				}
				i++;
				}

				//computer_play!!!!!!!!!!!!!!!!!!!!!!1
				int random;	
				char ranklist[14] = "234567891JKQA";
				srand(time(NULL));
				do{
					random = rand() % 12;
				}
				while(search(&computer, ranklist[random]) == 0);
				if(ranklist[random] == '1') {strcpy(buf,"\nPlayer 2's turn, enter a Rank:10");Rio_writen(connfd, buf, strlen(buf));}
				else {sprintf(buf, "\nPlayer 2's turn, enter a Rank:%c", ranklist[random]);Rio_writen(connfd, buf, strlen(buf));}
				char cc = ranklist[random];




				if(search(&user, cc) == 0){ //user dont have the rank user ask for, gofish
					if(cc == '1') {strcpy(buf, "\n    - Player 1 has no 10's");Rio_writen(connfd, buf, strlen(buf));}
					else {sprintf(buf, "\n    - Player 1 has no %c's", cc);Rio_writen(connfd, buf, strlen(buf));}
					struct card* newcard = next_card(); //pick a new card
					add_card(&computer, newcard); //add the new card to user's hand
					strcpy(buf, "\n    - Go Fish, Player 2 draws a card"); 
					Rio_writen(connfd, buf, strlen(buf));
					char mm = check_add_book(&computer);
					if( mm != 0){
						if(mm == '1') {strcpy(buf, "\n    - Player 2 books 10");Rio_writen(connfd, buf, strlen(buf));}
						else {sprintf(buf,"\n    - Player 2 books %c", mm);Rio_writen(connfd, buf, strlen(buf));}
						strcpy(buf, "\n    - Player 2 gets another turn");
						Rio_writen(connfd, buf, strlen(buf));
					} 
					else{
						if(newcard->rank != cc){
							turn = 0; //swith to user turn
							strcpy(buf, "\n    - Player 1's turn\n");
							Rio_writen(connfd, buf, strlen(buf));
						}
						else {strcpy(buf, "\n    - Player 2 gets another turn\n");Rio_writen(connfd, buf, strlen(buf));}
					}
					continue;
				}
				else{  //user has the rank user ask for
					strcpy(buf, "\n    - Player 1 has ");			
					Rio_writen(connfd, buf, strlen(buf));
					//reportrank###############################
					iterator = user.card_list;
					while(iterator != NULL){
						if(iterator->top.rank == cc){
							sprintf(buf, "%c%c ", cc, iterator->top.suit);
							Rio_writen(connfd, buf, strlen(buf));
						}
						iterator = iterator->next;
					}
						
					strcpy(buf, "\n    - Player 2 has ");
					Rio_writen(connfd, buf, strlen(buf));
					//reportrank###############################
					struct hand* iterator = computer.card_list;
					while(iterator != NULL){
						if(iterator->top.rank == cc){
							sprintf(buf, "%c%c ", cc, iterator->top.suit);
							Rio_writen(connfd, buf, strlen(buf));
						}
						iterator = iterator->next;
					}
					
					transfer_cards(&user, &computer, cc);
					if(check_add_book(&computer) == cc){
						if(cc == '1') {
							strcpy(buf, "\n    - Player 2 books 10");
							Rio_writen(connfd, buf, strlen(buf));
						}
						else {
							sprintf(buf,"\n    - Player 2 books %c", cc);
							Rio_writen(connfd, buf, strlen(buf));
						}
						strcpy(buf, "\n    - Player 2 gets another turn\n");
						Rio_writen(connfd, buf, strlen(buf));
					}
					else{
							strcpy(buf, "\n    - Player 1's turn\n");
							Rio_writen(connfd, buf, strlen(buf));
							turn = 0;
						}//switch to user's turn
					
					continue;
				}

			}
		}//turn loop ends*/
	 	
		//report in the end of the game!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
				//strcpy(buf, "\n");
				//Rio_writen(connfd, buf, strlen(buf));
				strcpy(buf, "\nPlayer 1's Hand-");
				Rio_writen(connfd, buf, strlen(buf));
				struct hand* iterator = user.card_list;
	
	
				while(iterator != NULL){
					if(iterator->top.rank == '1') {
						sprintf(buf,"10%c ", iterator->top.suit);
						Rio_writen(connfd, buf, strlen(buf));
					}	
					else{
						sprintf(buf, "%c%c ", iterator->top.rank, iterator->top.suit);
						Rio_writen(connfd, buf, strlen(buf));
					}
					iterator = iterator->next;
				}

	
				strcpy(buf, "\nPlayer 1's Book-");	
				Rio_writen(connfd, buf, strlen(buf));
				int i = 0;
				while(user.book[i] != 0){
				if(user.book[i] == '1'){
					strcpy(buf, "10 ");
					Rio_writen(connfd, buf, strlen(buf));
				}
				else{
					sprintf(buf, "%c ", user.book[i]);
					Rio_writen(connfd, buf, strlen(buf));
				}
				i++;
				}
				i = 0;
				strcpy(buf, "\nPlayer 2's Book-");
				Rio_writen(connfd, buf, strlen(buf));
				while(computer.book[i] != 0){
				if(computer.book[i] == '1'){
					strcpy(buf, "10 ");
					Rio_writen(connfd, buf, strlen(buf));
				}
				else{
					sprintf(buf, "%c ", computer.book[i]);
					Rio_writen(connfd, buf, strlen(buf));
				}
				i++;
				}
				




	
		if(user.book[6] != 0){
			sprintf(buf, "\nPlayer 1 Wins! %d-%d", book_count(&user), book_count(&computer)); //user wins
			Rio_writen(connfd, buf, strlen(buf));
		}
		else{
			sprintf(buf,  "\nPlayer 2 Wins! %d-%d", book_count(&computer), book_count(&user)); //computer wins
			Rio_writen(connfd, buf, strlen(buf));
		}
		strcpy(buf, "\n");
		Rio_writen(connfd, buf, strlen(buf));
		strcpy(buf, "\nDo you want to play again [Y/N]: \n"); //whether to play again
		Rio_writen(connfd, buf, strlen(buf));

		strcpy(buf, "stop\n");
		Rio_writen(connfd, buf, strlen(buf));

		Rio_readlineb(&rio, buf, strlen(buf));
		//Fputs(buf, stdout);
		c = buf[0];
		if(c == 'N') {
		strcpy(buf, "Exiting.\n");
		Rio_writen(connfd, buf, strlen(buf));
		break;
		}
		else if(c == 'Y'){
			strcpy(buf, "\n");
			Rio_writen(connfd, buf, strlen(buf));
		}
 		
	} // game loop ends
	

}

int main(int argc, char **argv) 
{
    int listenfd, *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
	//char client_hostname[MAXLINE], client_port[MAXLINE];
	
    if (argc != 1) {
	fprintf(stderr, "usage: %s\n", argv[0]);
	exit(0);
    }
    listenfd = Open_listenfd("5000");

    while (1) {
        clientlen=sizeof(struct sockaddr_storage);
		connfdp = Malloc(sizeof(int)); //line:conc:echoservert:beginmalloc
		*connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen); //line:conc:echoservert:endmalloc
		
		printf("@Player1 joined the game\n");
		printf("Ready to start!\n");
		printf("\n");
		echo(*connfdp);
		Close(*connfdp);
	
    }
	Free(connfdp);

	
}


/* $end echoservertmain */
