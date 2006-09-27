/*
 * Copyright (c) 2006, Advanced Micro Devices, Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 *    * Redistributions of source code must retain the above copyright 
 *      notice, this list of conditions and the following disclaimer.
 *    * Neither the name of Advanced Micro Devices, Inc. nor the names
 *      of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written
 *      permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 * Unit test for  \ref deck
 */

#include <unistd.h>
#include <stdio.h>
#include "deck.h"

#define SEED 1
#define REP 3
#define SIZE 32
size_t data[SIZE];
void check_card(T_deck_card *c, size_t i) {
	if ( c-> ord != i ) 
		printf("card %ld wrong ord\n",i);
	if ( c->data != &data[i] )
		printf("card %ld wrong data\n",i);
}

int main()
{
	unsigned int save_seed;
	size_t i;
	size_t dealt[SIZE];
	T_deck_card *c;
	T_deck *d = deck_create(SIZE,SEED,data,sizeof(size_t));

	for ( i = 0; i < SIZE ; i++ )
		check_card(&d->cards[i], i);

	if ( deck_deal(d,1)  ) 
		printf("deck init and or blackjack mode broken\n");

	if ( !deck_deal(d,0) )
		printf("deck init and or blackjack mode broken\n");
		
	save_seed = d->seed;
	deck_shuffle(d); /* with the card dealt above if shuffle doesn't
				work, then the REP loop will fail */

	if ( save_seed == d->seed )
		printf("seed not updating\n");


	bzero(dealt,sizeof(dealt));
	for ( i = 0; i < SIZE * REP ; i++ ) {
		c = deck_deal(d,0);
		printf("%ld ",c->ord);
		if ( !c )
			printf("\nNULL card without blackjack mode\n");
		else {
			check_card(c, c->ord);
			dealt[c->ord]++;
		}
	}
	printf("\n");

	for (i = 0; i < SIZE; i++ ) {
		if ( dealt[i] != REP ) {
			printf("card %ld not dealt enough\n",i);
		}
	}

	deck_destroy(d);
}
	

		
	
