/*
 * Common code for working with libgcrypt.
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

#if !defined _COMMON_CRYPT_H_
#define _COMMON_CRYPT_H_

#include <gcrypt.h>

#include "list.h"

#define NEED_LIBGCRYPT_VERSION "1.7.10"

/*!
 * \brief          Initialise libgcrypt library
 *
 * Initialise the libgcrypt library. Subsequent calls to the function
 * are ignored.
 */
extern void init_crypto(void);

/*!
 * \brief         Get list of usable ciphers
 * \return        A list of char* cipher names
 *
 * Return a process-lifetime list of names for cipher algorithms
 * supported by the active libgcrypt. Do not free or modify the list.
 */
extern list_t list_of_ciphers(void) __attribute__((pure));

/*!
 * \brief         Get list of usable hashes
 * \return        List of hash names
 *
 * Return a process-lifetime list of names for hash algorithms supported
 * by the active libgcrypt. Do not free or modify the list.
 */
extern list_t list_of_hashes(void) __attribute__((pure));

/*!
 * \brief         Get list of available cipher modes
 * \return        List of mode names
 *
 * Return a process-lifetime list of cipher modes that can be opened
 * with AES or ChaCha20 in the active libgcrypt. Do not free or modify
 * the list.
 */
extern list_t list_of_modes(void) __attribute__((pure));

/*!
 * \brief         Get list of available MAC algorithms
 * \return        List of MAC names
 *
 * Return a process-lifetime list of MAC algorithms supported by the
 * active libgcrypt. Do not free or modify the list.
 */
extern list_t list_of_macs(void) __attribute__((pure));

/*!
 * \brief         Get cipher ID, given its name
 * \param[in]  n  Cipher name
 * \return        The ID used by libgcrypt
 *
 * Return the matching ID, case-insensitively, or GCRY_CIPHER_NONE if
 * the name is not recognized or supported.
 */
extern enum gcry_cipher_algos cipher_id_from_name(const char * const restrict n) __attribute__((pure, nonnull(1)));

/*!
 * \brief         Get hash ID, given its name
 * \param[in]  n  Hash name
 * \return        The ID used by libgcrypt
 *
 * Return the matching ID, case-insensitively, or GCRY_MD_NONE if the
 * name is not recognized or supported.
 */
extern enum gcry_md_algos hash_id_from_name(const char * const restrict n) __attribute__((pure, nonnull(1)));

/*!
 * \brief         Get cipher mode ID, given its name
 * \param[in]  n  Mode name
 * \return        The ID used by libgcrypt
 *
 * Return the matching ID, case-insensitively, or
 * GCRY_CIPHER_MODE_NONE if the name is not recognized.
 */
extern enum gcry_cipher_modes mode_id_from_name(const char * const restrict n) __attribute__((pure, nonnull(1)));

/*!
 * \brief         Get MAC ID, given its name
 * \param[in]  n  MAC name
 * \return        The ID used by libgcrypt
 *
 * Return the matching ID, case-insensitively, or GCRY_MAC_NONE if the
 * name is not recognized or supported.
 */
extern enum gcry_mac_algos mac_id_from_name(const char * const restrict n) __attribute__((pure, nonnull(1)));

/*!
 * \brief         Get cipher name, given its ID
 * \param[in]  c  The libgcrypt cipher enum
 * \return        A string representation of the cipher
 *
 * Return the normalized libgcrypt name used by this project, or NULL
 * for an unsupported or duplicate alias.
 */
extern const char *cipher_name_from_id(enum gcry_cipher_algos c) __attribute__((pure));

/*!
 * \brief         Get hash name, given its ID
 * \param[in]  h  The libgcrypt hash enum
 * \return        A string representation of the hash
 *
 * Return the normalized libgcrypt name used by this project, or NULL
 * for an unsupported or duplicate alias.
 */
extern const char *hash_name_from_id(enum gcry_md_algos h) __attribute__((pure));

/*!
 * \brief         Get cipher mode name, given its ID
 * \param[in]  m  The libgcrypt mode enum
 * \return        A string representation of the mode
 *
 * Return this project's supported name for the mode ID, or NULL if the
 * mode is not in its supported mode table.
 */
extern const char *mode_name_from_id(enum gcry_cipher_modes m) __attribute__((pure));

/*!
 * \brief         Get MAC algorithm name, given its ID
 * \param[in]  m  The libgcrypt MAC enum
 * \return        A string representation of the MAC
 *
 * Return libgcrypt's name for the MAC ID.
 */
extern const char *mac_name_from_id(enum gcry_mac_algos m) __attribute__((pure));

/*!
 * \brief         Check whether a cipher and mode are considered compatible
 * \param[in]  c  The cipher to check
 * \param[in]  m  The cipher mode
 * \return        True if the mode is suitable for the cipher, false
 *                otherwise
 *
 * Apply the project's compatibility rules for stream, AEAD, and
 * block-cipher modes. This is a compatibility check, not a guarantee
 * that a later libgcrypt operation will succeed.
 */
extern bool mode_valid_for_cipher(enum gcry_cipher_algos c, enum gcry_cipher_modes m);

#endif /* _COMMON_CRYPT_H_ */
