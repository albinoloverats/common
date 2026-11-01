#include "test_support.h"

static void assert_sorted_names(list_t names)
{
	assert_true(list_size(names) > 0);
	for (size_t i = 1; i < list_size(names); i++)
		assert_true(strcmp(list_get(names, i - 1), list_get(names, i)) < 0);
}

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
	list_t ciphers = list_of_ciphers();
	list_t hashes = list_of_hashes();
	list_t modes = list_of_modes();
	list_t macs = list_of_macs();
	assert_sorted_names(ciphers);
	assert_sorted_names(hashes);
	assert_sorted_names(modes);
	assert_sorted_names(macs);
	assert_non_null(list_contains(ciphers, "AES"));
	assert_non_null(list_contains(hashes, "SHA256"));
	assert_non_null(list_contains(modes, "CBC"));
	assert_non_null(list_contains(macs, "HMAC_SHA256"));
	assert_ptr_equal(ciphers, list_of_ciphers());
	assert_ptr_equal(hashes, list_of_hashes());
	assert_ptr_equal(modes, list_of_modes());
	assert_ptr_equal(macs, list_of_macs());
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_crypto_name_mappings_and_modes)
};

REGISTER_TESTS(ccrypt, tests)
