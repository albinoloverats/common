/*
 * Common code for ECC calculations.
 * Copyright © 2005-2026, albinoloverats ~ Software Development
 * email: webmaster@albinoloverats.net
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#if !defined _ECC_H_
#define _ECC_H_

#include <inttypes.h>

/*
 * ecc Version 1.2 by Paul Flaherty (paulf@stanford.edu)
 * Copyright (C) 1993 Free Software Foundation, Inc.
 *
 * Basic Software Tool for Encoding and Decoding Files.
 *
 * This is a simple stream encoder. It takes a buffer of data 249 bytes
 * (encode) - or 255 bytes (decode) - and copies the corresponding
 * encoded/decoded block to the output buffer. An encoded block contains
 * 249 data bytes and 6 redundancy bytes.
 */

#define ECC_CAPACITY     255 /*!< Encoded Reed-Solomon codeword length, in bytes */
#define ECC_PAYLOAD      249 /*!< Message payload length, in bytes */
#define ECC_OFFSET      (ECC_CAPACITY - ECC_PAYLOAD) /*!< Redundancy bytes in each codeword */

/*!
 * \brief         Encode one payload into a Reed-Solomon codeword
 * \param[in]  m  Input payload of ECC_PAYLOAD bytes
 * \param[out] c  Output codeword buffer of ECC_CAPACITY bytes
 *
 * Adds ECC_OFFSET redundancy bytes to the payload.
 */
extern void ecc_encode(uint8_t m[ECC_PAYLOAD], uint8_t c[ECC_CAPACITY]);

/*!
 * \brief         Decode and attempt to correct one Reed-Solomon codeword
 * \param[in,out] code    Codeword buffer; the decoder reverses it in place
 * \param[out]    mesg    Output buffer of ECC_CAPACITY bytes; the first
 *                        ECC_PAYLOAD bytes contain the recovered message
 * \param[out]    errcode 0 for no detected errors, 1-3 for corrected
 *                        errors, or 4 when correction is unsuccessful
 *
 * When correction is unsuccessful, mesg still contains the best-effort
 * message extracted before error correction.
 */
extern void ecc_decode(uint8_t code[ECC_CAPACITY], uint8_t mesg[ECC_CAPACITY], int *errcode);

#endif /* _ECC_H_ */
