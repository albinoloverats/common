#include "test_support.h"

static void test_cli_output_and_speed_calculation(void **state)
{
	(void)state;
	FILE *output = tmpfile();
	assert_non_null(output);
	assert_int_equal(cli_fprintf(output, "value=%d\n", 42), 9);
	const uint8_t bytes[] = { 0x01, 0xab, 0xff };
	assert_int_equal(cli_fprintx(output, bytes, sizeof bytes), 18);
	assert_int_equal(fflush(output), 0);
	assert_int_equal(fseek(output, 0, SEEK_SET), 0);
	char text[64] = { 0 };
	assert_non_null(fgets(text, sizeof text, output));
	assert_string_equal(text, "value=42\n");
	assert_non_null(fgets(text, sizeof text, output));
	assert_string_equal(text, "00000000: 01ab ff\n");
	fclose(output);

	output = tmpfile();
	assert_non_null(output);
	uint8_t block[17];
	for (size_t i = 0; i < sizeof block; i++)
		block[i] = (uint8_t)i;
	assert_int_equal(cli_fprintx(output, block, sizeof block), 64);
	assert_int_equal(fseek(output, 0, SEEK_SET), 0);
	assert_non_null(fgets(text, sizeof text, output));
	assert_string_equal(text, "00000000: 0001 0203 0405 0607 0809 0a0b 0c0d 0e0f \n");
	assert_non_null(fgets(text, sizeof text, output));
	assert_string_equal(text, "00000010: 10\n");
	fclose(output);

	cli_bps_s samples[BPS];
	for (uint64_t i = 0; i < BPS; i++)
	{
		samples[i].time = i * MILLION;
		samples[i].bytes = i * 1024;
	}
	assert_true(fabs(cli_calc_bps(samples) - 1024.0) < 0.001);
}


static void test_cli_stream_helpers_and_ansi_stripping(void **state)
{
	(void)state;
	FILE *output = tmpfile();
	assert_non_null(output);
	int saved = dup(STDOUT_FILENO);
	assert_true(saved >= 0);
	fflush(stdout);
	assert_int_equal(dup2(fileno(output), STDOUT_FILENO), STDOUT_FILENO);
	int written = cli_printf("\x1b[31mred\x1b[0m\n");
	fflush(stdout);
	assert_int_equal(dup2(saved, STDOUT_FILENO), STDOUT_FILENO);
	close(saved);
	assert_int_equal(written, 4);
	assert_int_equal(fseek(output, 0, SEEK_SET), 0);
	char text[32] = { 0 };
	assert_non_null(fgets(text, sizeof text, output));
	assert_string_equal(text, "red\n");
	fclose(output);

	output = tmpfile();
	assert_non_null(output);
	saved = dup(STDERR_FILENO);
	assert_true(saved >= 0);
	fflush(stderr);
	assert_int_equal(dup2(fileno(output), STDERR_FILENO), STDERR_FILENO);
	written = logger("message %d", 7);
	fflush(stderr);
	assert_int_equal(dup2(saved, STDERR_FILENO), STDERR_FILENO);
	close(saved);
	assert_int_equal(written, 10);
	assert_int_equal(fseek(output, 0, SEEK_SET), 0);
	memset(text, 0, sizeof text);
	assert_non_null(fgets(text, sizeof text, output));
	assert_string_equal(text, "message 7\n");
	fclose(output);
}

static const struct CMUnitTest tests[] =
{
	cmocka_unit_test(test_cli_output_and_speed_calculation),
	cmocka_unit_test(test_cli_stream_helpers_and_ansi_stripping)
};

REGISTER_TESTS(cli, tests)
