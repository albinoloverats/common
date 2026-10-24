#include "test_support.h"

static void test_ecc_round_trip_and_single_byte_correction(void **state)
{
	(void)state;
	uint8_t message[ECC_PAYLOAD];
	uint8_t encoded[ECC_CAPACITY];
	uint8_t damaged[ECC_CAPACITY];
	uint8_t decoded[ECC_CAPACITY];
	for (size_t i = 0; i < sizeof message; i++)
		message[i] = (uint8_t)(i * 37u + 11u);

	ecc_encode(message, encoded);
	memcpy(damaged, encoded, sizeof damaged);
	int errors = -1;
	ecc_decode(damaged, decoded, &errors);
	assert_int_equal(errors, 0);
	assert_memory_equal(decoded, message, sizeof message);

	memcpy(damaged, encoded, sizeof damaged);
	damaged[117] ^= 0x5a;
	ecc_decode(damaged, decoded, &errors);
	assert_int_equal(errors, 1);
	assert_memory_equal(decoded, message, sizeof message);
}


static void test_ecc_corrects_two_bytes(void **state)
{
	(void)state;
	uint8_t message[ECC_PAYLOAD];
	uint8_t encoded[ECC_CAPACITY];
	uint8_t damaged[ECC_CAPACITY];
	uint8_t decoded[ECC_CAPACITY];
	for (size_t i = 0; i < sizeof message; i++)
		message[i] = (uint8_t)(i * 23u + 5u);
	ecc_encode(message, encoded);
	memcpy(damaged, encoded, sizeof damaged);
	damaged[0] ^= 0x41;
	damaged[254] ^= 0x92;
	int errors = -1;
	ecc_decode(damaged, decoded, &errors);
	assert_int_equal(errors, 2);
	assert_memory_equal(decoded, message, sizeof message);
}


static void test_ecc_corrects_three_bytes_and_reports_uncorrectable_data(void **state)
{
	(void)state;
	uint8_t message[ECC_PAYLOAD];
	uint8_t encoded[ECC_CAPACITY];
	uint8_t damaged[ECC_CAPACITY];
	uint8_t decoded[ECC_CAPACITY];
	for (size_t i = 0; i < sizeof message; i++)
		message[i] = (uint8_t)(i * 19u + 3u);

	ecc_encode(message, encoded);
	memcpy(damaged, encoded, sizeof damaged);
	damaged[1] ^= 0x35;
	damaged[67] ^= 0xa2;
	damaged[201] ^= 0x7c;
	int errors = -1;
	ecc_decode(damaged, decoded, &errors);
	assert_int_equal(errors, 3);
	assert_memory_equal(decoded, message, sizeof message);

	memcpy(damaged, encoded, sizeof damaged);
	damaged[1] ^= 0x35;
	damaged[67] ^= 0xa2;
	damaged[201] ^= 0x7c;
	damaged[254] ^= 0x81;
	ecc_decode(damaged, decoded, &errors);
	assert_int_equal(errors, 4);
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_ecc_round_trip_and_single_byte_correction),
	cmocka_unit_test(test_ecc_corrects_two_bytes),
	cmocka_unit_test(test_ecc_corrects_three_bytes_and_reports_uncorrectable_data)
};

REGISTER_TESTS(ecc, tests)
