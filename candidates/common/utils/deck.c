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
 * \brief
 * Implements a deck of cards randomizer
 */
#include <unistd.h>
#include <stdlib.h>
#include "deck.h"

/** \defgroup deck "deck_" Deck of cards API
 * \brief
 * Provides a "deck of cards" random selection interface
 *
 * "Deck of cards" randomization ensures that all elements of a population
 * are used once before any are used more than once.  This is useful when
 * an exact population profile is desired, not just one the is probablistic
 * over a sufficient sample set.
 *
 * A T_deck contains the deck state and an array of T_deck_card cards. By
 * default the T_deck_card structs contain a unique ordinal and a data pointer
 * into a data array of constant stride (or NULL if not supplied).  
 *
 * \section deck_howto_population Controlling the population 
 *
 * The T_deck can be stacked by altering either the distribution of the
 * ordinals or of the data pointers.  The size of the T_deck should be
 * defined as the size of the total population, and the appropriate number of
 * cards set to match the population for each value of the distribution
 *
 * \section deck_todo  TODO
 *	- Another creator which would stack the deck according to a supplied
 *	  distribution would be handy. (simple examples would include a
 *	  multi-deck "shoe")
 *	- Use of a better random number generator (rand_r is weak).
 *	- Repeatable shuffles (return back the seed state that started the deck)
 *	- Enhancement to "blackjack" mode which would reshuffle when the
 *	  deck reaches some "almost dealt" state (which violates the
 *	  logic of have a deck, unless you \b really do want to use it
 *	  for a game of chance ;-) )
 */

/*@{*/
/**
 * \brief
 * Allocates an deck of cards and initializes them with ordinal and data.
 * 
 * The ord (ordinal) field of each card is set to a zero based index of the 
 * card in the deck of card array. This value can freely be changed to suit
 * the needs of the calling program, and is initialized as a convenience.
 *
 * If parameter \b data is non-null, the data field of each card is set to
 * data + ordinal * dsize. Thus, each card points to consectutive entries in
 * an array of data elements of size dsize.  Again this data can be freely
 * modified after create to suit the needs of the calling program
 *
 * \param size number of cards in the deck created
 * \param seed the seed to use for this deck
 * \param data array of data to map to T_deck_card::data, NULL indicates none
 * \param dsize the stride of the data array (if non-NULL)
 * \return the created deck
 */
T_deck *deck_create(size_t size, unsigned int seed, void *data, size_t dsize) {
	T_deck *d = malloc(sizeof(T_deck));
	size_t i;
	d->seed = seed;
	d->size = size;
	d->cards = malloc(size *sizeof(T_deck_card));
	for ( i = 0; i < size; i++ ) {
		d->cards[i].ord = i;
		if ( data )
			d->cards[i].data = data + i * dsize;
		else
			d->cards[i].data = NULL;
	}
	d->dealt = size; /* will force shuffle on next deal */
	return d;
}

/**
 * \brief
 * Deletes the deck a \b all \b cards \b in \b it.  Does not delete data
 * referenced by the cards.
 *
 * \param d the deck delete
 */
void deck_destroy(T_deck *d)
{
	if (d) {
		if (d->cards)
			free(d->cards);
		free(d);	
	}
}

/**
 * \brief
 * shuffles the given T_deck d, and  resets dealt field. Does not
 * modify the card ordinal or data fields.
 * \param d the deck to shuffle
 */
void deck_shuffle(T_deck *d)
{
	size_t i,j;
	size_t n;
	T_deck_card t;
	double r;
	if ( !d )
		return;
	n = d->size;
	for ( i=0; i < (n-1); i++ ) {
		r = (1.0*rand_r(&d->seed))/(RAND_MAX+1.0);
		j = i + 1 + (size_t)(r * (n-i-1));

		t = d->cards[j];
		d->cards[j] = d->cards[i];
		d->cards[i] = t;
	}
	d->dealt = 0;
}	

/**
 * \brief
 * returns the next card in the deck, advances the dealt field, 
 * automatically reshuffling, as needed unless "blackjack" mode is set
 * \param d the deck to deal
 * \param blackjack don't shuffle at end of deck, instead return NULL
 * \return the card on the top of the deck or NULL if blackjack and out
 *         of cards.
 */
T_deck_card *deck_deal(T_deck *d, int blackjack)
{
	T_deck_card *c = NULL;
	if ( d->dealt >= d->size )  {
		if ( !blackjack ) {
			deck_shuffle(d);
			c = &d->cards[d->dealt++];
		}
	} else {
		c = &d->cards[d->dealt++];
	}
	return c;
}

/**
 * \brief
 * returns the next card in the deck without advancing "dealt". Allows the
 * caller peek at the next card without actually dealing it. This can result
 * in the deck shuffling unless blackjack is set, or NULL if blackjack is set
 * and the deck is out of cards.
 * \param d the deck to deal
 * \param blackjack don't shuffle at end of deck, instead return NULL
 * \return the card on the top of the deck
 */
T_deck_card *deck_peek(T_deck *d, int blackjack)
{
	T_deck_card *c = NULL;
	if ( d->dealt >= d->size )  {
		if ( !blackjack ) {
			deck_shuffle(d);
			c = &d->cards[d->dealt];
		}
	} else {
		c = &d->cards[d->dealt];
	}
	return c;
}

/*@}*/
