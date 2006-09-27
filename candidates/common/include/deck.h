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
#ifndef _DECK_H_
#define _DECK_H_

/**
 * \file
 * \brief
 * Type and function declarations for \ref deck
 */

/**
 * \brief
 * A single card in the T_deck
 */
typedef struct {
	size_t ord; /*!< the ordinal number of the card, initially unique */
	void *data; /*!< pointer to this card's data, or NULL if none */
} T_deck_card;

/**
 * \brief
 * A deck of cards including "dealt" state and seed tracking
 */
typedef struct {
	unsigned int seed;	/*!< the rand_r state data */
	size_t size;		/*!< number of cards in the deck */
	size_t dealt;		/*!< how many cards have been dealt */
	T_deck_card *cards;	/*!< The deck of cards array */
} T_deck;

/* The API */
T_deck *deck_create(size_t size, unsigned int seed, void *data, size_t dsize);
void deck_destroy(T_deck *d);
void deck_shuffle(T_deck *d);
T_deck_card *deck_deal(T_deck *d, int blackjack);
T_deck_card *deck_peek(T_deck *d, int blackjack);





#endif
