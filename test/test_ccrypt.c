#include "test_support.h"

static void test_crypto_name_mappings_and_modes(void **state)
{
	(void)state;
	init_crypto();
	assert_int_equal(cipher_id_from_name("aes"), GCRY_CIPHER_AES);
	assert_int_equal(hash_id_from_name("sha256"), GCRY_MD_SHA256);
	assert_int_equal(mode_id_from_name("cbc"), GCRY_CIPHER_MODE_CBC);
	assert_int_equal(mac_id_from_name("hmac_sha256"), GCRY_MAC_HMAC_SHA256);
	assert_int_equal(cipher_id_from_name("not-a-cipher"), GCRY_CIPHER_NONE);
	assert_int_equal(hash_id_from_name("not-a-hash"), GCRY_MD_NONE);
	assert_int_equal(mode_id_from_name("not-a-mode"), GCRY_CIPHER_MODE_NONE);
	assert_int_equal(mac_id_from_name("not-a-mac"), GCRY_MAC_NONE);
	assert_string_equal(cipher_name_from_id(GCRY_CIPHER_AES), "AES");
	assert_string_equal(hash_name_from_id(GCRY_MD_SHA1), "SHA160");
	assert_string_equal(mode_name_from_id(GCRY_CIPHER_MODE_CBC), "CBC");
	assert_null(mode_name_from_id(GCRY_CIPHER_MODE_NONE));
	assert_true(mode_valid_for_cipher(GCRY_CIPHER_AES, GCRY_CIPHER_MODE_CBC));
	assert_false(mode_valid_for_cipher(GCRY_CIPHER_AES, GCRY_CIPHER_MODE_STREAM));
	assert_true(mode_valid_for_cipher(GCRY_CIPHER_CHACHA20, GCRY_CIPHER_MODE_STREAM));
	assert_true(mode_valid_for_cipher(GCRY_CIPHER_CHACHA20, GCRY_CIPHER_MODE_POLY1305));
	assert_false(mode_valid_for_cipher(GCRY_CIPHER_AES, GCRY_CIPHER_MODE_POLY1305));
	assert_true(mode_valid_for_cipher(GCRY_CIPHER_AES, GCRY_CIPHER_MODE_GCM));
	assert_false(mode_valid_for_cipher(GCRY_CIPHER_3DES, GCRY_CIPHER_MODE_GCM));
	assert_true(mode_valid_for_cipher(GCRY_CIPHER_AES128, GCRY_CIPHER_MODE_AESWRAP));
	assert_false(mode_valid_for_cipher(GCRY_CIPHER_3DES, GCRY_CIPHER_MODE_AESWRAP));
	assert_true(list_size(list_of_ciphers()) > 0);
	assert_true(list_size(list_of_hashes()) > 0);
	assert_true(list_size(list_of_modes()) > 0);
	assert_true(list_size(list_of_macs()) > 0);
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_crypto_name_mappings_and_modes)
};

REGISTER_TESTS(ccrypt, tests)
